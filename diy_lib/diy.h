#ifndef _DIY_H_
#define _DIY_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
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

namespace Diy
{
  class DiyTool
  {
    public:
      DiyTool(const char *benchmark_name, const char *diy_name, uint32_t id = 0);
      ~DiyTool();

      bool init();

      // Reader
      llvm::Instruction* get_init_pc();
      uint32_t request_op(llvm::Instruction *inst);
      // Decoder
      llvm::Instruction* get_first_instruction(const uint32_t op);
      unsigned int num_operands(const uint32_t op);
      bool op_is_reg(const uint32_t op, unsigned int idx);
      llvm::Instruction* get_op_reg(const uint32_t op, unsigned int idx);
      // Performance Model
      uint32_t get_latency(uint32_t op);
    private:
      uint32_t m_id;
      char *m_benchmark;
      char *m_diy;

      std::ifstream *diy;

      llvm::LLVMContext *context;
      std::unique_ptr<llvm::Module> mod;

      std::vector<uint32_t> category;
      std::vector<uint32_t> latency;
      std::unordered_map<llvm::Instruction *, uint32_t> inst2op;
      std::unordered_map< uint32_t, std::vector<llvm::Instruction *> > op2inst;
      std::unordered_map< uint32_t, llvm::Instruction* > op2first;
  };
}

#endif