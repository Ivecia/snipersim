#include "llvm_reader.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "llvm/Support/raw_ostream.h"

#define VERBOSE 0

Sift::LLVMReader::LLVMReader(Diy::DiyTool *tool, String trace_name, int32_t file_id, uint32_t id)
  : handleLLVMForkFunc(NULL)
  , handleLLVMForkArg(NULL)
  , handleLLVMJoinFunc(NULL)
  , handleLLVMJoinArg(NULL)
{
  m_trace_name = trace_name;
  char trace_file_name[2048];
  if (file_id == -1)
    sprintf(trace_file_name, "%s/trace_main.txt", trace_name.c_str());
  else
    sprintf(trace_file_name, "%s/trace_%d.txt", trace_name.c_str(), file_id);
  m_trace = strdup(trace_file_name);
  m_id = id;
  diy = tool;
  pc = diy->get_init_pc(file_id);
}

Sift::LLVMReader::~LLVMReader()
{
  free(m_benchmark);
  free(m_trace);
}

bool Sift::LLVMReader::init()
{
  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] LLVM Reader initializing..." << std::endl;
  #endif

  trace = new std::ifstream(m_trace, std::ios::in);

  if ((!trace->is_open()) || (!trace->good())) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot open " << m_trace << "\n";
    return false;
  }

  struct stat filestatus;
  stat(m_trace, &filestatus);
  filesize = filestatus.st_size;

  if (!pc) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot find the main function in " << m_benchmark << "\n";
    return false;
  }
  uint32_t op = diy->request_op(pc);
  if (op) {
    newop[op] = true;
  }

  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] LLVM Reader initialized without error..." << std::endl;
  #endif

  return true;
}

bool Sift::LLVMReader::pc_forward()
{
  unsigned int opcode = pc->getOpcode();
  switch (opcode) {
    case llvm::Instruction::Br:
      if (pc->getNumOperands() == 1) {
        // Unconditional branch
        pc = llvm::dyn_cast<llvm::Instruction>(llvm::dyn_cast<llvm::BasicBlock>(pc->getOperand(0))->begin());
      } else if (pc->getNumOperands() == 3) {
        // Conditional branch
        if (info) {
          // True
          pc = llvm::dyn_cast<llvm::Instruction>(llvm::dyn_cast<llvm::BasicBlock>(pc->getOperand(2))->begin());
        } else {
          // False
          pc = llvm::dyn_cast<llvm::Instruction>(llvm::dyn_cast<llvm::BasicBlock>(pc->getOperand(1))->begin());
        }
      } else {
        std::cerr << "[Sift-LLVMReader: " << m_id << "] llvm::Instruction::Br does not have neither 1 nor 3 operands..." << std::endl;
        assert(false);
      }
      // clear new op list
      newop.clear();
      break;
    default:
      pc = pc->getNextNode();
      break;
  }
  if (pc) return true;
  return false;
}

bool Sift::LLVMReader::find_next()
{
  if (!pc_forward()) {
    return false;
  }
  uint32_t op = diy->request_op(pc);
  while (op) {
    if (!newop[op]) {
      newop[op] = true;
      return true;
    }
    bool ret = pc_forward();
    if (ret) {
      op = diy->request_op(pc);
    } else {
      return false;
    }
  }
  return true;
}

void Sift::LLVMReader::make_inst(Instruction &inst)
{
  unsigned int opcode = pc->getOpcode();
  // StaticInstruction
  const StaticInstruction *sinst;
  if (scache.count(pc)) {
    sinst = scache[pc];
  } else {
    StaticInstruction *new_sinst = new StaticInstruction();
    new_sinst->addr = (uint64_t)pc;
    new_sinst->size = 4;
    new_sinst->next = NULL;
    sinst = new_sinst;
    scache[pc] = sinst;
  }
  // Instruction
  inst.sinst = sinst;
  inst.is_branch = false;
  inst.taken = false;
  inst.num_addresses = 0;
  inst.addresses[0] = inst.addresses[1] = 0;
  inst.is_predicate = false;
  inst.executed = true;
  inst.isa = 7;       // dl::DL_ISA_LLVM
  switch (opcode) {
    case llvm::Instruction::Br:
      if (pc->getNumOperands() == 3) {
        inst.is_branch = true;
        *trace >> info;
        if (info)
          inst.taken = true;
      }
      break;
    case llvm::Instruction::Load:
    case llvm::Instruction::Store:
      inst.num_addresses = 1;
      *trace >> info;
      inst.addresses[0] = info;
      break;
  }

  // std::string irString;
  // llvm::raw_string_ostream os(irString);
  // pc->print(os);
  // std::cerr << info << " " << irString << std::endl;
}

bool Sift::LLVMReader::Read(Instruction &inst)
{
  make_inst(inst);
  if (pc->getOpcode() == llvm::Instruction::Call) {
    bool ret;
    // multithreading
    auto now = llvm::dyn_cast<llvm::CallInst>(pc);
    if (now->getCalledFunction()->getName() == "pthread_create") {
      auto callee = llvm::dyn_cast<llvm::Function>(now->getOperand(2));
      diy->set_init_pc(trace_count, llvm::dyn_cast<llvm::Instruction>(callee->getEntryBlock().begin()));
      trace_queue.push(handleLLVMForkFunc(handleLLVMForkArg, trace_count));
      trace_count++;
      ret = find_next();
    } else if (now->getCalledFunction()->getName() == "pthread_join") {
      assert(trace_queue.size());
      handleLLVMJoinFunc(handleLLVMJoinArg, trace_queue.front());
      trace_queue.pop();
      ret = find_next();
    } else if (now->getCalledFunction()->isDeclaration() == false) {
      ret = find_next();
      func.push(pc);
      pc = llvm::dyn_cast<llvm::Instruction>(now->getCalledFunction()->getEntryBlock().begin());
    } else {
      ret = find_next();
    }
    return ret;
  } else {
    bool ret = find_next();
    if (ret == false && func.size()) {
      ret = true;
      pc = func.top();
      func.pop();
    }
    return ret;
  }
}

Diy::DiyTool* Sift::LLVMReader::getDiy()
{
  return diy;
}

uint64_t Sift::LLVMReader::getPosition()
{
  if (trace)
    return trace->tellg();
  return 0;
}

uint64_t Sift::LLVMReader::getLength()
{
  return filesize;
}