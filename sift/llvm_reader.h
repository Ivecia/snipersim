#ifndef __LIFT_READER_H
#define __LIFT_READER_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>
#include <sys/types.h>

#include "fixed_types.h"
#include "sift_reader.h"
#include <diy.h>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PassManager.h"

namespace Sift
{
  class LLVMReader
  {
    typedef int32_t (*HandleJoinFunc)(void* arg, int32_t thread);
    typedef int32_t (*HandleForkFunc)(void* arg);
    public:
      LLVMReader(Diy::DiyTool *tool, String trace_name, int32_t file_id, uint32_t id = 0);
      ~LLVMReader();

      bool init();
      bool Read(Instruction &inst);
      Diy::DiyTool* getDiy();
      uint64_t getPosition();
      uint64_t getLength();

      // fork and join
      void setHandleForkFunc(HandleForkFunc func, void* arg = NULL) { assert(func); handleForkFunc = func; handleForkArg = arg;}
      void setHandleJoinFunc(HandleJoinFunc func, void* arg = NULL) { assert(func); handleJoinFunc = func; handleJoinArg = arg; }
    
    private:
      // fork and join
      HandleForkFunc handleForkFunc;
      void *handleForkArg;
      HandleJoinFunc handleJoinFunc;
      void *handleJoinArg;

      uint32_t m_id;
      String m_trace_name;
      uint64_t filesize;

      llvm::Instruction *pc;
      uint64_t info;

      char *m_benchmark;
      char *m_trace;
      std::ifstream *bench;
      std::ifstream *trace;

      std::unordered_map<llvm::Instruction*, const StaticInstruction*> scache;

      Diy::DiyTool *diy;
      std::unordered_map<uint32_t, bool> newop;

      bool pc_forward();
      bool find_next();
      void make_inst(Instruction &inst);
  };
}

#endif