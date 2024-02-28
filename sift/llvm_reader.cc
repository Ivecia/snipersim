#include "llvm_reader.h"

#include <iostream>
#include "llvm/Support/raw_ostream.h"

#define VERBOSE 100

Sift::LLVMReader::LLVMReader(const char *trace_name, uint32_t id)
{
  m_trace = strdup(trace_name);
  m_id = id;
}

Sift::LLVMReader::~LLVMReader()
{
  free(m_benchmark);
  free(m_trace);
}

bool Sift::LLVMReader::init(Diy::DiyTool *tool)
{
  diy = tool;

  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] LLVM Reader initializing..." << std::endl;
  #endif

  trace = new std::ifstream(m_trace, std::ios::in);

  if ((!trace->is_open()) || (!trace->good())) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot open " << m_trace << "\n";
    return false;
  }

  pc = diy->get_init_pc();

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
  if (find_next())
    return true;
  return false;
}