#include "llvm/IR/PassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <vector>

using namespace llvm;
using namespace std;

namespace {
  struct TraceInfo : public PassInfoMixin<TraceInfo> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
      // Get the LLVM Context of Module M.
      auto &CTX = M.getContext();
      // Declare a function callee for printing memory access.
      FunctionCallee printMemoryAccess    = M.getOrInsertFunction("printMemoryAccess", FunctionType::get(
        Type::getVoidTy(CTX), {Type::getInt64Ty(CTX)}, false
      ));
      // Declare a function callee for printing condition of a conditional branch.
      FunctionCallee printBranchCondition = M.getOrInsertFunction("printBranchCondition", FunctionType::get(
        Type::getVoidTy(CTX), {Type::getInt1Ty(CTX)}, false
      ));
      // Declare a function callee for registering the id of the main thread.
      FunctionCallee registerMainThreadID = M.getOrInsertFunction("registerMainThreadID", FunctionType::get(
        Type::getVoidTy(CTX), false
      ));
      // Declare a function callee for entering a parallel function.
      FunctionCallee enterFunction        = M.getOrInsertFunction("enterFunction", FunctionType::get(
        Type::getVoidTy(CTX), false
      ));
      // Declare a function callee for exiting a parallel function.
      // Declare boolean constant for branch condition.
      IntegerType *TRUE  = IntegerType::get(CTX, true);
      IntegerType *FALSE = IntegerType::get(CTX, false);
      // printMemoryAccess & printBranchCondition
      for (auto &F : M) {
        // Ignore the declaration of functions.
        if (F.isDeclaration()) {
          continue;
        }
        for (auto &B : F) {
          for (auto &I : B) {
            // Create calls before load instruction.
            auto load = dyn_cast<LoadInst>(&I);
            if (load) {
              IRBuilder<> Builder(load);
              // Record the address of the load instruction.
              Value *arg = Builder.CreateBitOrPointerCast(load->getOperand(0), Type::getInt64Ty(CTX), "cast");
              Builder.CreateCall(printMemoryAccess, {arg});
            }
            // Create calls before store instruction.
            auto store = dyn_cast<StoreInst>(&I);
            if (store) {
              IRBuilder<> Builder(store);
              // Record the address of the store instruction.
              Value *arg = Builder.CreateBitOrPointerCast(store->getOperand(1), Type::getInt64Ty(CTX), "cast");
              Builder.CreateCall(printMemoryAccess, {arg});
            }
            // Create calls before br instruction.
            auto br = dyn_cast<BranchInst>(&I);
            if (br) {
              IRBuilder<> Builder(br);
              // If it is not a unconditional branch, then record whether it jumps.
              if (br->getOperand(0)->getType() != Type::getLabelTy(CTX)) {
                Builder.CreateCall(printBranchCondition, {br->getOperand(0)});
              }
            }
          }
        }
      }
      // enterFunction
      for (auto &F : M) {
        // Ignore the declaration of functions.
        if (F.isDeclaration() || F.getName() == "main") {
          continue;
        }
        IRBuilder<> Builder(F.getEntryBlock().getFirstNonPHI());
        Builder.CreateCall(enterFunction);
      }
      // registerMainThreadID
      Function *mainFunc = M.getFunction("main");
      IRBuilder<> Builder(M.getFunction("main")->getEntryBlock().getFirstNonPHI());
      Builder.CreateCall(registerMainThreadID);
      // Transform Pass returns PreservedAnalyses::none().
      return PreservedAnalyses::none();
    }
  };
}

extern "C" :: llvm :: PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "TraceInfo", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "trace-info") {
            MPM.addPass(TraceInfo());
            return true;
          }
          return false;
        } 
      );
    }
  };
}