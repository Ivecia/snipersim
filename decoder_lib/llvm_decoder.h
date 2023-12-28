#ifndef _LLVM_DECODER_H_
#define _LLVM_DECODER_H_

#include "decoder.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <map>

namespace dl
{

class LLVMDecoder : public Decoder
{
  public:
    LLVMDecoder(dl_arch arch, dl_mode mode, dl_syntax syntax);
    virtual ~LLVMDecoder();

    virtual void decode(DecodedInst *inst) override;
    void decode(DecodedInst *inst, dl_isa isa) override;

    // Change the ISA mode to new_mode
    virtual void change_isa_mode(dl_isa new_isa) override;
    // Get the instruction name from the numerical (enum) instruction Id
    virtual const char* inst_name(unsigned int inst_id) override;
    // Get the register name from the numerical (enum) register Id
    virtual const char* reg_name(unsigned int reg_id) override;
    // Get the largest enclosing register; applies to x86 only; ARM just returns r
    virtual decoder_reg largest_enclosing_register(decoder_reg r) override;
    // Check if this register is invalid
    virtual bool invalid_register(decoder_reg r) override;
    // Check if this register holds the program counter
    virtual bool reg_is_program_counter(decoder_reg r) override;
    // True if instruction belongs to instruction group/category
    virtual bool inst_in_group(const DecodedInst *inst, unsigned int group_id) override;
    // Get the number of operands of any type for the specified instruction
    virtual unsigned int num_operands(const DecodedInst *inst) override;
    // Get the number of memory operands of the specified instruction
    virtual unsigned int num_memory_operands(const DecodedInst * inst) override;
    // Get the base register of the memory operand pointed by mem_idx
    virtual decoder_reg mem_base_reg(const DecodedInst *inst, unsigned int mem_idx) override;
    // Check if the base register of the memory operand pointed by mem_idx is also updated
    virtual bool mem_base_upate(const DecodedInst *inst, unsigned int mem_idx) override;
    // Check if the memory operand pointed by mem_idx has an index register
    virtual bool has_index_reg(const DecodedInst *inst, unsigned int mem_idx) override;
    // Get the index register of the memory operand pointed by mem_idx
    virtual decoder_reg mem_index_reg(const DecodedInst *inst, unsigned int mem_idx) override;
    // Check if the operand mem_idx from instruction inst is read from memory
    virtual bool op_read_mem(const DecodedInst *inst, unsigned int mem_idx) override;
    // Check if the operand mem_idx from instruction inst is written to memory
    virtual bool op_write_mem(const DecodedInst *inst, unsigned int mem_idx) override;
    // Check if the operand idx from instruction inst reads from a register
    virtual bool op_read_reg(const DecodedInst *inst, unsigned int idx) override;
    // Check if the operand idx from instruction inst writes a register
    virtual bool op_write_reg(const DecodedInst *inst, unsigned int idx) override;
    // Check if the operand idx from instruction inst is involved in an address generation operation
    virtual bool is_addr_gen(const DecodedInst *inst, unsigned int idx) override;
    // Check if the operand idx from instruction inst is a register
    virtual bool op_is_reg(const DecodedInst *inst, unsigned int idx) override;
    // Get the register used for operand idx from instruction inst.
    virtual decoder_reg get_op_reg(const DecodedInst *inst, unsigned int idx) override;
    // Get the size in bytes of the memory operand pointed by mem_idx
    virtual unsigned int size_mem_op(const DecodedInst *inst, unsigned int mem_idx) override;
    // Get the number of execution micro operations contained in instruction 'ins' 
    virtual unsigned int get_exec_microops(const DecodedInst *ins, int numLoads, int numStores) override;
    // Get the maximum size of the operands of instruction inst in bits
    virtual uint16_t get_operand_size(const DecodedInst *ins) override;
    // Check if the opcode is an instruction that performs a cache flush
    virtual bool is_cache_flush_opcode(decoder_opcode opcd) override;
    // Check if the opcode is a division instruction
    virtual bool is_div_opcode(decoder_opcode opcd) override;
    // Check if the opcode is a pause instruction
    virtual bool is_pause_opcode(decoder_opcode opcd) override;
    // Check if the opcode is a branch instruction
    virtual bool is_branch_opcode(decoder_opcode opcd) override;
    // Check if the opcode is an add/sub instruction that operates in vector and FP registers
    virtual bool is_fpvector_addsub_opcode(decoder_opcode opcd, const DecodedInst* ins) override;
    // Check if the opcode is a mul/div instruction that operates in vector and FP registers
    virtual bool is_fpvector_muldiv_opcode(decoder_opcode opcd, const DecodedInst* ins) override;
    // Check if the opcode is an instruction that loads or store data on vector and FP registers
    virtual bool is_fpvector_ldst_opcode(decoder_opcode opcd, const DecodedInst* ins) override;
    // Get the value of the last register in the enumeration
    virtual decoder_reg last_reg() override;
    // Get the input register mapped. Some registers can be mapped onto the lower bits of others.
    virtual uint32_t map_register(decoder_reg reg) override;
    // Get the number of implicit registers that are read by the instruction
    virtual unsigned int num_read_implicit_registers(const DecodedInst* inst) override;
    // Get the idx implicit source register
    virtual decoder_reg get_read_implicit_reg(const DecodedInst* inst, unsigned int idx) override;
    // Get the number of implicit registers that are written by the instruction
    virtual unsigned int num_write_implicit_registers(const DecodedInst* inst) override;
    // Get the idx implicit destiny register
    virtual decoder_reg get_write_implicit_reg(const DecodedInst* inst, unsigned int idx) override;

  private:
    unsigned int total_inst = 0;
    std::map<llvm::Instruction*, Decoder::decoder_reg> instid;

};

class LLVMDecodedInst : public DecodedInst
{
  public:
    LLVMDecodedInst(Decoder *d, const uint8_t *code, size_t size, uint64_t address);
    // virtual ~LLVMDecodedInst();    // dtor

    void set_llvm_instruction(llvm::Instruction inst);
    llvm::Instruction* get_llvm_instruction();

    // Get the instruction numerical Id
    virtual unsigned int inst_num_id() const override;
    // Get an string with the disassembled instruction
    virtual std::string disassembly_to_str() const override;
    // Check if this instruction is a NOP
    virtual bool is_nop() const override;
    // Check if this instruction is atomic
    virtual bool is_atomic() const override;
    // Check if instruction is a prefetch
    virtual bool is_prefetch() const override;
    // Check if this instruction is serializing (all previous instructions must have been executed)
    virtual bool is_serializing() const override;
    // Check if this instruction is a conditional branch
    virtual bool is_conditional_branch() const override;
    // Check if this instruction is an indirect branch
    virtual bool is_indirect_branch() const override;
    // Check if this instruction is a fence/barrier-type
    virtual bool is_barrier() const override;
    // Check if in this instruction the result merges the source and destination.
    virtual bool src_dst_merge() const override;
    // Intel: not used in LLVM
    virtual bool is_X87() const override;
    // ARM: not used in LLVM
    virtual bool has_modifiers() const override;
    // ARM: not used in LLVM
    virtual bool is_mem_pair() const override;
    // ARM: not used in LLVM
    virtual bool is_writeback() const override;

  private:
    llvm::Instruction *i;
};
}

#endif