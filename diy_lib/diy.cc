#include "diy.h"

#include <iostream>
#include "llvm/Support/raw_ostream.h"

#define VERBOSE 0

Diy::DiyTool::DiyTool(const char *benchmark_name, const char *diy_name, uint32_t id)
{
  m_benchmark = strdup(benchmark_name);
  m_diy = strdup(diy_name);
  m_id = id;

  // Initialize DiyTools
  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] LLVM DiyTool initializing..." << std::endl;
  #endif

  diy = new std::ifstream(m_diy, std::ios::in);
  
  if ((!diy->is_open()) || (!diy->good())) {
    std::cerr << "[DiyTool: " << m_id << "] Cannot open " << m_diy << "\n";
    exit(-1);
  }

  context = new llvm::LLVMContext;
  llvm::SMDiagnostic err;
  mod = llvm::parseIRFile(m_benchmark, err, *context);

  if (!mod) {
    std::cerr << "[DiyTool: " << m_id << "] Cannot parse IR file " << m_benchmark << "\n";
    exit(-1);
  }
  
  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] LLVM DiyTool initialized without error..." << std::endl;
  #endif

  // Reading Diy file
  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] Analysing Diy file and establishing Connections..." << std::endl;
  #endif

  // pre init for func_id and bb_id with bb
  uint32_t func_id = 0, bb_id = 0;
  std::map<std::pair<uint32_t, uint32_t>, llvm::BasicBlock*> bb_ptr;
  for (auto &func : *mod) {
    bb_id = 0;
    for (auto &bb : func) {
      bb_ptr[std::make_pair(func_id, bb_id)] = &bb;
      ++bb_id;
    }
    ++func_id;
  }

  // number of categories new ops
  uint32_t total;
  *diy >> total;
  latency.emplace_back(0);
  for (uint32_t i = 0; i < total; ++i) {
    uint32_t lat;
    *diy >> lat;
    latency.emplace_back(lat);
  }
  // number of new ops in bb
  *diy >> total;
  category.emplace_back(0);
  for (uint32_t cur = 0; cur < total; ++cur) {
    std::cerr << "===== Special Instruction " << cur + 1 << " =====" << std::endl;
    // category of new op
    uint32_t cate;
    *diy >> cate;
    category.emplace_back(cate);
    // function id and basic block id
    uint32_t func_id, bb_id;
    *diy >> func_id >> bb_id;
    // inst id
    uint32_t inst_num;
    *diy >> inst_num;
    std::vector<uint32_t> inst_vec;
    for (uint32_t i = 0; i < inst_num; ++i) {
      uint32_t inst_id;
      *diy >> inst_id;
      inst_vec.emplace_back(inst_id);
    }
    std::sort(inst_vec.begin(), inst_vec.end());
    // establish connection
    std::vector<llvm::Instruction*> inst_ptr;
    uint32_t inst_id = 0;
    for (auto &inst : *bb_ptr[std::make_pair(func_id, bb_id)]) {
      if (inst_id == *inst_vec.begin()) {
        if (inst2op.count(&inst)) {
          std::cerr << "[DiyTool " << m_id << "] an instruction is mapped to two new ops (ops may be same), with func_id=" << func_id << ", bb_id=" << bb_id << ", inst_id=" << *inst_vec.begin() << ", conflicts are " << inst2op[&inst] << " and " << cur << "." << std::endl;
          exit(-1);
        }
        inst.dump();
        inst2op[&inst] = cur + 1;
        inst_ptr.emplace_back(&inst);
        inst_vec.erase(inst_vec.begin());
      }
      ++inst_id;
    }
    op2inst[cur + 1] = inst_ptr;
  }

  #if VERBOSE > 0
  std::cerr << "[DEBUG: " << m_id << "] Analysing Diy file and establishing Connections finished without errors..." << std::endl;
  #endif
}

Diy::DiyTool::~DiyTool()
{
  free(m_benchmark);
  free(m_diy);
}

// Reader

llvm::Instruction* Diy::DiyTool::get_init_pc(int32_t file_id)
{
  if (file_id == -1) {
    // trace_main
    llvm::Instruction *pc = nullptr;
    for (auto &func : *mod) {
      if (func.isDeclaration() || func.getName() != "main")
        continue;
      pc = llvm::dyn_cast<llvm::Instruction>(func.getEntryBlock().begin());
    }
    return pc;
  } else {
    return init_pc[file_id];
  }
}

void Diy::DiyTool::set_init_pc(int32_t file_id, llvm::Instruction *pc)
{
  assert(file_id == (int32_t)init_pc.size());
  init_pc.push_back(pc);
}

uint32_t Diy::DiyTool::request_op(llvm::Instruction *inst)
{
  if (inst2op.count(inst)) {
    // std::cerr << "request op:" << std::endl;
    // inst->dump();
    // set the first instruction for that special opcode
    if (!op2first.count(inst2op[inst])) {
      op2first[inst2op[inst]] = inst;
    }
    return inst2op[inst];
  }
  return 0;
}

// Decoder

llvm::Instruction* Diy::DiyTool::get_first_instruction(const uint32_t op)
{
  assert(op > 0);
  return op2first[op];
}

unsigned int Diy::DiyTool::num_operands(const uint32_t op)
{
  assert(op > 0);
  uint32_t total = op2inst[op].size();
  unsigned int ret = 0;
  for (uint32_t i = 0; i < total; ++i) {
    llvm::Instruction *inst = op2inst[op][i];
    uint32_t size = inst->getNumOperands(), now = 0;
    for (uint32_t j = 0; j < size; ++j) {
      llvm::Instruction *prev_inst = llvm::dyn_cast<llvm::Instruction>(inst->getOperand(j));
      if (prev_inst && inst2op[prev_inst] != op) ++now;
    }
    ret += now;
  }
  // +1 means the result of this instruction should be store in some register
  return ret + 1;
}

bool Diy::DiyTool::op_is_reg(const uint32_t op, unsigned int idx)
{
  assert(op > 0);
  uint32_t total = op2inst[op].size();
  unsigned int ret = 0;
  for (uint32_t i = 0; i < total; ++i) {
    llvm::Instruction *inst = op2inst[op][i];
    uint32_t size = inst->getNumOperands(), now = 0;
    for (uint32_t j = 0; j < size; ++j) {
      llvm::Instruction *prev_inst = llvm::dyn_cast<llvm::Instruction>(inst->getOperand(j));
      if (prev_inst && inst2op[prev_inst] != op) ++now;
    }
    if (ret <= idx && idx < ret + now) {
      now = 0;
      for (uint32_t j = 0; j < size; ++j) {
        llvm::Instruction *prev_inst = llvm::dyn_cast<llvm::Instruction>(inst->getOperand(j));
        if (ret + now == idx) return prev_inst;
        ++now;
      }
    }
    ret += now;
  }
  // reach here, means that the last operands, means the register which is written by this special inst
  return true;
}

llvm::Instruction* Diy::DiyTool::get_op_reg(const uint32_t op, unsigned int idx)
{
  assert(op > 0);
  uint32_t total = op2inst[op].size();
  unsigned int ret = 0;
  for (uint32_t i = 0; i < total; ++i) {
    llvm::Instruction *inst = op2inst[op][i];
    uint32_t size = inst->getNumOperands(), now = 0;
    for (uint32_t j = 0; j < size; ++j) {
      llvm::Instruction *prev_inst = llvm::dyn_cast<llvm::Instruction>(inst->getOperand(j));
      if (prev_inst && inst2op[prev_inst] != op) ++now;
    }
    if (ret <= idx && idx < ret + now) {
      now = 0;
      for (uint32_t j = 0; j < size; ++j) {
        llvm::Instruction *prev_inst = llvm::dyn_cast<llvm::Instruction>(inst->getOperand(j));
        if (ret + now == idx) {
          uint32_t prev_op = request_op(prev_inst);
          if (prev_op) {
            return get_first_instruction(op);
          } else {
            return prev_inst;
          }
        }
        ++now;
      }
    }
    ret += now;
  }
  // reach here, means that the last operands, means the register which is written by this special inst
  return get_first_instruction(op);
}

// Performance Model

uint32_t Diy::DiyTool::get_latency(uint32_t op)
{
  assert(op > 0);
  op = category[op];
  return latency[op];
}