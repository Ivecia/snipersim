#include "core_model_llvm.h"
#include "interval_contention_llvm.h"
#include "rob_contention_llvm.h"
#include "dynamic_micro_op_llvm.h"
#include "log.h"
#include "config.hpp"
#include "simulator.h"

#include <decoder.h>
#include "llvm_meta.h"

CoreModelLLVM::CoreModelLLVM()
{
   printf("Enabled LLVM Core Model...");
   m_lll_cutoff = Sim()->getCfg()->getInt("perf_model/core/interval_timer/lll_cutoff");
}

unsigned int CoreModelLLVM::getInstructionLatency(const MicroOp *uop) const
{
   unsigned int instruction_type = (unsigned int) uop->getInstructionOpcode();
   LOG_ASSERT_ERROR(instruction_type > 0 && instruction_type < 68, "Invalid instruction type %d", instruction_type);
   return 1;
}

unsigned int CoreModelLLVM::getAluLatency(const MicroOp *uop) const
{
   switch(uop->uop_type)
   {
      case MicroOp::UOP_EXECUTE:
         switch (uop->getInstructionOpcode()) {
            case llvm_op_Mul:
               return 4;
            case llvm_op_UDiv:
            case llvm_op_SDiv:
            case llvm_op_URem:
            case llvm_op_SRem:
               return 8;
            case llvm_op_FNeg:
            case llvm_op_FAdd:
            case llvm_op_FSub:
               return 8;
            case llvm_op_FMul:
            case llvm_op_FDiv:
            case llvm_op_FRem:
               return 12;
            default:
               return 1;
         }
      default:
         return 1;
   }
}

unsigned int CoreModelLLVM::getBypassLatency(const DynamicMicroOp *uop) const
{
   return 0;
}

unsigned int CoreModelLLVM::getLongestLatency() const
{
   return 60;
}

IntervalContention* CoreModelLLVM::createIntervalContentionModel(const Core *core) const
{
   return new IntervalContentionLLVM(core, this);
}

RobContention* CoreModelLLVM::createRobContentionModel(const Core *core) const
{
   std::cerr << "Enabled ROB Model" << std::endl;
   return new RobContentionLLVM(core, this);
}

DynamicMicroOp* CoreModelLLVM::createDynamicMicroOp(Allocator *alloc, const MicroOp *uop, ComponentPeriod period) const
{
   DynamicMicroOpLLVM *info = DynamicMicroOp::alloc<DynamicMicroOpLLVM>(alloc, uop, this, period);
   info->uop_port = DynamicMicroOpLLVM::getPort(uop);
   info->uop_fu   = DynamicMicroOpLLVM::getFU(uop);
   return info;
}
