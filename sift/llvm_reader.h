#ifndef __LIFT_READER_H
#define __LIFT_READER_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>
#include <sys/types.h>

#include "sift_reader.h"

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
    public:
      LLVMReader(const char *benchmark_name, const char *trace_name, uint32_t id = 0);
      ~LLVMReader();
      bool init();
      bool Read(Instruction &inst);
    
    private:
      uint32_t m_id;

      llvm::LLVMContext *context;
      std::unique_ptr<llvm::Module> mod;
      llvm::Instruction *pc;
      uint64_t info;

      char *m_benchmark;
      char *m_trace;
      std::ifstream *bench;
      std::ifstream *trace;

      std::unordered_map<llvm::Instruction*, const StaticInstruction*> scache;
  };
}

#endif