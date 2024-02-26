#include "llvm_reader.h"

#include <iostream>
#include "llvm/Support/raw_ostream.h"

#define VERBOSE 100

Sift::LLVMReader::LLVMReader(const char *benchmark_name, const char *trace_name, uint32_t id)
{
  m_benchmark = strdup(benchmark_name);
  m_trace = strdup(trace_name);
  m_id = id;
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

  bench = new std::ifstream(m_benchmark, std::ios::in);

  if ((!bench->is_open()) || (!bench->good())) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot open " << m_benchmark << "\n";
    return false;
  }

  trace = new std::ifstream(m_trace, std::ios::in);

  if ((!trace->is_open()) || (!trace->good())) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot open " << m_trace << "\n";
    return false;
  }

  context = new llvm::LLVMContext;
  llvm::SMDiagnostic err;
  mod = llvm::parseIRFile(m_benchmark, err, *context);

  if (!mod) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot parse IR file " << m_benchmark << "\n";
    return false;
  }

  for (auto &func : *mod) {
    if (func.isDeclaration() || func.getName() != "main")
      continue;
    pc = llvm::dyn_cast<llvm::Instruction>(func.getEntryBlock().begin());
  }

  if (!pc) {
    std::cerr << "[Sift-LLVMReader: " << m_id << "] Cannot find the main function in " << m_benchmark << "\n";
    return false;
  }

  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] LLVM Reader initialized without error..." << std::endl;
  #endif

  return true;
}

bool Sift::LLVMReader::Read(Instruction &inst)
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

  // Moving pc to the next instruction
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
      break;
    default:
      pc = pc->getNextNode();
      break;
  }
  if (pc)
    return true;
  return false;
}