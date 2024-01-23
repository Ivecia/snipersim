#include "llvm_decoder.h"

namespace dl
{

LLVMDecoder::LLVMDecoder(dl_arch arch, dl_mode mode, dl_syntax syntax)
{
  this->m_arch = arch;
  this->m_mode = mode;
  this->m_syntax = syntax;
  this->m_isa = DL_ISA_LLVM;
}

LLVMDecoder::~LLVMDecoder()
{
}

void LLVMDecoder::decode(DecodedInst *inst)
{
  if (inst->get_already_decoded())
    return;
  ((LLVMDecodedInst*)inst)->set_llvm_instruction((llvm::Instruction*)inst->get_address());
  instid[((LLVMDecodedInst*)inst)->get_llvm_instruction()] = ++total_inst;
  inst->set_already_decoded(true);
}

void LLVMDecoder::decode(DecodedInst *inst, dl_isa isa)
{
  this->decode(inst);
}

// Change the ISA mode to new_mode
void LLVMDecoder::change_isa_mode(dl_isa new_isa)
{
  this->m_isa = new_isa;
}

// Get the instruction name from the numerical (enum) instruction Id
const char* LLVMDecoder::inst_name(unsigned int inst_id)
{
  return llvm::Instruction::getOpcodeName(inst_id);
}

// Get the register name from the numerical (enum) register Id
const char* LLVMDecoder::reg_name(unsigned int reg_id)
{
  return ("%" + std::to_string(reg_id)).c_str();
}

// X86: not used in LLVM
Decoder::decoder_reg LLVMDecoder::largest_enclosing_register(Decoder::decoder_reg r)
{
  return r;
}

// Check if this register is invalid
bool LLVMDecoder::invalid_register(decoder_reg r)
{
  return true;
}

// Check if this register holds the program counter
bool LLVMDecoder::reg_is_program_counter(decoder_reg r)
{
  return false;
}

// True if instruction belongs to instruction group/category (ignore)
bool LLVMDecoder::inst_in_group(const DecodedInst *inst, unsigned int group_id)
{
  return true;
}

// Get the number of operands of any type for the specified instruction
unsigned int LLVMDecoder::num_operands(const DecodedInst *inst)
{
  unsigned int ret = 0;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  // Normal instruction: i->getNumOperands() + 1
  // Store instruction: i->getNumOperands() (will not be a source)
  ret = i->getNumOperands() + (i->getOpcode() != llvm::Instruction::Store && i->getOpcode() != llvm::Instruction::Br);
  return ret;
}

// Get the number of memory operands of the specified instruction
unsigned int LLVMDecoder::num_memory_operands(const DecodedInst *inst)
{
  unsigned int ret = 0;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  switch (i->getOpcode()) {
    case llvm::Instruction::Load:
    case llvm::Instruction::Store:
      ret = 1;
      break;
  }
  return ret;
}

// Get the base register of the memory operand pointed by mem_idx
Decoder::decoder_reg LLVMDecoder::mem_base_reg(const DecodedInst *inst, unsigned int mem_idx)
{
  Decoder::decoder_reg ret = 0;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  switch (i->getOpcode()) {
    case llvm::Instruction::Load:
      ret = instid[llvm::dyn_cast<llvm::Instruction>(i->getOperand(0))];
      break;
    case llvm::Instruction::Store:
      ret = instid[llvm::dyn_cast<llvm::Instruction>(i->getOperand(1))];
      break;
  }
  return ret;
}

// Check if the base register of the memory operand pointed by mem_idx is also updated
bool LLVMDecoder::mem_base_upate(const DecodedInst *inst, unsigned int mem_idx)
{
  return false;
}

// Check if the memory operand pointed by mem_idx has an index register
bool LLVMDecoder::has_index_reg(const DecodedInst *inst, unsigned int mem_idx)
{
  return false;
}

// Get the index register of the memory operand pointed by mem_idx
Decoder::decoder_reg LLVMDecoder::mem_index_reg(const DecodedInst *inst, unsigned int mem_idx)
{
  return 0;
}

// Check if the operand mem_idx from instruction inst is read from memory
bool LLVMDecoder::op_read_mem(const DecodedInst *inst, unsigned int mem_idx)
{
  bool ret = false;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  switch (i->getOpcode()) {
    case llvm::Instruction::Load:
      ret = true;
      break;
  }
  return ret;
}

// Check if the operand mem_idx from instruction inst is written to memory
bool LLVMDecoder::op_write_mem(const DecodedInst *inst, unsigned int mem_idx)
{
  bool ret = false;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  switch (i->getOpcode()) {
    case llvm::Instruction::Store:
      ret = true;
      break;
  }
  return ret;
}

// Check if the operand idx from instruction inst reads from a register
bool LLVMDecoder::op_read_reg(const DecodedInst *inst, unsigned int idx)
{
  bool ret = false;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  if (idx < i->getNumOperands()) {
    ret = llvm::dyn_cast<llvm::Instruction>(i->getOperand(idx));
  }
  return ret;
}

// Check if the operand idx from instruction inst writes a register
bool LLVMDecoder::op_write_reg(const DecodedInst *inst, unsigned int idx)
{
  bool ret = false;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  if (idx == i->getNumOperands()) {
    ret = true;
  }
  return ret;
}

// Check if the operand idx from instruction inst is involved in an address generation operation
bool LLVMDecoder::is_addr_gen(const DecodedInst *inst, unsigned int idx)
{
  return false;
}

// Check if the operand idx from instruction inst is a register
bool LLVMDecoder::op_is_reg(const DecodedInst *inst, unsigned int idx)
{
  bool ret = false;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  if (idx < i->getNumOperands()) {
    ret = llvm::dyn_cast<llvm::Instruction>(i->getOperand(idx));
  } else if (idx == i->getNumOperands()) {
    ret = true;
  }
  return ret;
}

// Get the register used for operand idx from instruction inst.
Decoder::decoder_reg LLVMDecoder::get_op_reg(const DecodedInst *inst, unsigned int idx)
{
  Decoder::decoder_reg ret = 0;
  llvm::Instruction *i = ((LLVMDecodedInst*)inst)->get_llvm_instruction();
  if (idx < i->getNumOperands()) {
    ret = instid[llvm::dyn_cast<llvm::Instruction>(i->getOperand(idx))];
  } else if (idx == i->getNumOperands()) {
    ret = instid[i];
  }
  return ret;
}

// Get the size in bytes of the memory operand pointed by mem_idx
unsigned int LLVMDecoder::size_mem_op(const DecodedInst *inst, unsigned int mem_idx)
{
  // TODO
  return 4;
}

// Get the number of execution micro operations contained in instruction 'ins' 
unsigned int LLVMDecoder::get_exec_microops(const DecodedInst *ins, int numLoads, int numStores)
{
  // TODO
  return 1;
}

// Get the maximum size of the operands of instruction inst in bits
uint16_t LLVMDecoder::get_operand_size(const DecodedInst *ins)
{
  // TODO
  return 32;
}

// Check if the opcode is an instruction that performs a cache flush
bool LLVMDecoder::is_cache_flush_opcode(decoder_opcode opcd)
{
  return false;
}

// Check if the opcode is a division instruction
bool LLVMDecoder::is_div_opcode(decoder_opcode opcd)
{
  bool ret = false;
  switch (opcd) {
    case llvm::Instruction::UDiv:
    case llvm::Instruction::SDiv:
      ret = true;
      break;
  }
  return ret;
}

// Check if the opcode is a pause instruction
bool LLVMDecoder::is_pause_opcode(decoder_opcode opcd)
{
  return false;
}

// Check if the opcode is a branch instruction
bool LLVMDecoder::is_branch_opcode(decoder_opcode opcd)
{
  bool ret = false;
  switch (opcd) {
    case llvm::Instruction::Br:
      ret = true;
      break;
  }
  return ret;
}

// Check if the opcode is an add/sub instruction that operates in vector and FP registers
bool LLVMDecoder::is_fpvector_addsub_opcode(decoder_opcode opcd, const DecodedInst* ins) { return false; }

// Check if the opcode is a mul/div instruction that operates in vector and FP registers
bool LLVMDecoder::is_fpvector_muldiv_opcode(decoder_opcode opcd, const DecodedInst* ins) { return false; }

// Check if the opcode is an instruction that loads or store data on vector and FP registers
bool LLVMDecoder::is_fpvector_ldst_opcode(decoder_opcode opcd, const DecodedInst* ins) { return false; }

// Get the value of the last register in the enumeration
Decoder::decoder_reg LLVMDecoder::last_reg()
{
  return total_inst;
}

// Get the input register mapped. Some registers can be mapped onto the lower bits of others.
uint32_t LLVMDecoder::map_register(decoder_reg reg)
{
  return reg;
}

// Get the number of implicit registers that are read by the instruction
unsigned int LLVMDecoder::num_read_implicit_registers(const DecodedInst* inst) { return 0; }

// Get the idx implicit source register
Decoder::decoder_reg LLVMDecoder::get_read_implicit_reg(const DecodedInst* inst, unsigned int idx) { return 0; }

// Get the number of implicit registers that are written by the instruction
unsigned int LLVMDecoder::num_write_implicit_registers(const DecodedInst* inst) { return 0; }

// Get the idx implicit destiny register
Decoder::decoder_reg LLVMDecoder::get_write_implicit_reg(const DecodedInst* inst, unsigned int idx) { return 0; }



LLVMDecodedInst::LLVMDecodedInst(Decoder *d, const uint8_t *code, size_t size, uint64_t address)
{
  this->m_dec = d;
  this->m_code = code;
  this->m_size = size;
  this->m_address = address;
  this->m_already_decoded = false;
}

void LLVMDecodedInst::set_llvm_instruction(llvm::Instruction *inst)
{
  this->i = inst;
}

llvm::Instruction* LLVMDecodedInst::get_llvm_instruction()
{
  return this->i;
}

// Get the instruction numerical Id
unsigned int LLVMDecodedInst::inst_num_id() const
{
  return this->i->getOpcode();
}

// Get an string with the disassembled instruction
std::string LLVMDecodedInst::disassembly_to_str() const
{
  std::string ret;
  llvm::raw_string_ostream raw_stream(ret);
  this->i->print(raw_stream);
  return ret;
}

// Check if this instruction is a NOP
bool LLVMDecodedInst::is_nop() const { return false; }

// Check if this instruction is atomic
bool LLVMDecodedInst::is_atomic() const
{
  bool ret = false;
  switch (this->i->getOpcode()) {
    case llvm::Instruction::AtomicCmpXchg:
    case llvm::Instruction::AtomicRMW:
      ret = true;
      break;
  }
  return ret;
}

// Check if instruction is a prefetch
bool LLVMDecodedInst::is_prefetch() const { return false; }

// Check if this instruction is serializing (all previous instructions must have been executed)
bool LLVMDecodedInst::is_serializing() const { return false; }

// Check if this instruction is a conditional branch
bool LLVMDecodedInst::is_conditional_branch() const
{
  bool ret = false;
  switch (this->i->getOpcode()) {
    case llvm::Instruction::Br:
      ret = (this->i->getNumOperands() == 3);
      break;
  }
  return ret;
}

// Check if this instruction is an indirect branch
bool LLVMDecodedInst::is_indirect_branch() const
{
  bool ret = false;
  switch (this->i->getOpcode()) {
    case llvm::Instruction::IndirectBr:
      ret = true;
      break;
  }
  return ret;
}

// Check if this instruction is a fence/barrier-type
bool LLVMDecodedInst::is_barrier() const
{
  bool ret = false;
  switch (this->i->getOpcode()) {
    case llvm::Instruction::Fence:
      ret = true;
      break;
  }
  return ret;
}

// Check if in this instruction the result merges the source and destination.
bool LLVMDecodedInst::src_dst_merge() const { return false; }

// Intel: not used in LLVM
bool LLVMDecodedInst::is_X87() const { return false; }

// ARM: not used in LLVM
bool LLVMDecodedInst::has_modifiers() const { return false; }

// ARM: not used in LLVM
bool LLVMDecodedInst::is_mem_pair() const { return false; }

// ARM: not used in LLVM
bool LLVMDecodedInst::is_writeback() const { return false; }

}