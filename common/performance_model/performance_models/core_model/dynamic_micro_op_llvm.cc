#include "dynamic_micro_op_llvm.h"
#include "micro_op.h"

#include "llvm_meta.h"

const char* DynamicMicroOpLLVM::getType() const
{
   return "LLVM";
}

const char* DynamicMicroOpLLVM::PortTypeString(DynamicMicroOpLLVM::uop_port_t port)
{
   switch(port)
   {
      case UOP_NONE:       return "Port None";
      case UOP_INT:        return "Port Int";
      case UOP_FLOAT:      return "Port Float";
      case UOP_MEM:        return "Port Mem";
      case UOP_SPECIAL:    return "Port Special";
      case UOP_ALL:        return "Port ALL";
      default:
         LOG_PRINT_ERROR("Unknown port type %d", port);
   }
}

DynamicMicroOpLLVM::uop_port_t DynamicMicroOpLLVM::getPort(const MicroOp *uop)
{
   if (uop->getInstructionOpcode() > 100) {
      return DynamicMicroOpLLVM::UOP_SPECIAL;
   }
   switch (uop->getInstructionOpcode()) {
      case llvm_op_Add:
      case llvm_op_Sub:
      case llvm_op_Mul:
      case llvm_op_UDiv:
      case llvm_op_SDiv:
      case llvm_op_URem:
      case llvm_op_SRem:
      case llvm_op_Shl:
      case llvm_op_LShr:
      case llvm_op_AShr:
      case llvm_op_And:
      case llvm_op_Or:
      case llvm_op_Xor:
         return DynamicMicroOpLLVM::UOP_INT;
      case llvm_op_FNeg:
      case llvm_op_FAdd:
      case llvm_op_FSub:
      case llvm_op_FMul:
      case llvm_op_FDiv:
      case llvm_op_FRem:
         return DynamicMicroOpLLVM::UOP_FLOAT;
      case llvm_op_Load:
      case llvm_op_Store:
         return DynamicMicroOpLLVM::UOP_MEM;
      case llvm_op_Ret:
      case llvm_op_Br:
      case llvm_op_PHI:
      case llvm_op_Call:
      case llvm_op_Alloca:
      case llvm_op_GetElementPtr:
         return DynamicMicroOpLLVM::UOP_ALL;
   }
   return DynamicMicroOpLLVM::UOP_NONE;
}

DynamicMicroOpLLVM::uop_fu_t DynamicMicroOpLLVM::getFU(const MicroOp *uop)
{
   if (uop->getInstructionOpcode() > 100) {
      return UOP_FU_SPECIAL;
   }
   switch(uop->uop_type)
   {
      case MicroOp::UOP_LOAD:
      case MicroOp::UOP_STORE:
         switch (uop->getInstructionOpcode()) {
            case llvm_op_Load:
               return UOP_FU_LSU;
            default:
               return UOP_FU_NONE;
         }
      case MicroOp::UOP_EXECUTE:
         switch (uop->getInstructionOpcode()) {
            case llvm_op_Add:
            case llvm_op_Sub:
            case llvm_op_Shl:
            case llvm_op_LShr:
            case llvm_op_AShr:
            case llvm_op_And:
            case llvm_op_Or:
            case llvm_op_Xor:
               return UOP_FU_ALU;
            case llvm_op_Mul:
            case llvm_op_UDiv:
            case llvm_op_SDiv:
            case llvm_op_URem:
            case llvm_op_SRem:
               return UOP_FU_MDU;
            case llvm_op_FNeg:
            case llvm_op_FAdd:
            case llvm_op_FSub:
               return UOP_FU_FALU;
            case llvm_op_FMul:
            case llvm_op_FDiv:
            case llvm_op_FRem:
               return UOP_FU_FMDU;
            case llvm_op_Load:
            case llvm_op_Store:
               return UOP_FU_LSU;
            default:
               return UOP_FU_NONE;
         }
      default:
         return UOP_FU_NONE;
   }
}

DynamicMicroOpLLVM::DynamicMicroOpLLVM(const MicroOp *uop, const CoreModel *core_model, ComponentPeriod period)
   : DynamicMicroOp(uop, core_model, period)
   , uop_port(DynamicMicroOpLLVM::getPort(uop))
   , uop_fu(DynamicMicroOpLLVM::getFU(uop))
{
}
