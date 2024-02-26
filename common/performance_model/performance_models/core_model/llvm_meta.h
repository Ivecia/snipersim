#ifndef __LLVM_META_H
#define __LLVM_META_H

// https://llvm.org/docs/LangRef.html#instruction-reference
enum llvm_op
{
    llvm_op_Illegal = 0,            // illegal instruction
    // Terminate
    llvm_op_Ret = 1,                // return control flow (and optionally a value) from a function back to the caller
    llvm_op_Br = 2,                 // cause control flow to transfer to a different basic block in the current function
    llvm_op_Switch = 3,             // (not considered)
    llvm_op_IndirectBr = 4,         // (not considered)
    llvm_op_Invoke = 5,             // (not considered)
    llvm_op_Resume = 6,             // (not considered)
    llvm_op_Unreachable = 7,        // (not considered)
    llvm_op_CleanupRet = 8,         // (not considered)
    llvm_op_CatchRet = 9,           // (not considered)
    llvm_op_CatchSwitch = 10,       // (not considered)
    llvm_op_CallBr = 11,            // (not considered)
    // Unary
    llvm_op_FNeg = 12,              // returns the negation of its operand (floating-point or vector)
    // Binary
    llvm_op_Add = 13,               // returns the sum of its two operands (integer or vector)
    llvm_op_FAdd = 14,              // returns the sum of its two operands (floating-point or vector)
    llvm_op_Sub = 15,               // returns the difference of its two operands (integer or vector)
    llvm_op_FSub = 16,              // returns the difference of its two operands (floating-point or vector)
    llvm_op_Mul = 17,               // returns the product of its two operands (integer or vector)
    llvm_op_FMul = 18,              // returns the product of its two operands (floating-point or vector)
    llvm_op_UDiv = 19,              // returns the quotient of its two operands (unsigned integer or vector)
    llvm_op_SDiv = 20,              // returns the quotient of its two operands (signed integer or vector)
    llvm_op_FDiv = 21,              // returns the quotient of its two operands (floating-point or vector)
    llvm_op_URem = 22,              // returns the remainder from the unsigned division of its two operands (integer or vector)
    llvm_op_SRem = 23,              // returns the remainder from the signed division of its two operands (integer or vector)
    llvm_op_FRem = 24,              // returns the remainder from the division of its two operands (floating-point or vector)
    // Logical
    llvm_op_Shl = 25,               // returns the first operand shifted to the left a specified number of bits
    llvm_op_LShr = 26,              // returns the first operand shifted to the right a specified number of bits with zero fill
    llvm_op_AShr = 27,              // returns the first operand shifted to the right a specified number of bits with sign extension
    llvm_op_And = 28,               // returns the bitwise logical and of its two operands
    llvm_op_Or = 29,                // returns the bitwise logical inclusive or of its two operands
    llvm_op_Xor = 30,               // returns the bitwise logical exclusive or of its two operands
    // Memory
    llvm_op_Alloca = 31,            // allocates memory on the stack frame of the currently executing function
    llvm_op_Load = 32,              // read from memory
    llvm_op_Store = 33,             // write to memory
    llvm_op_GetElementPtr = 34,     // get the address of a subelement of an aggregate data structure
    llvm_op_Fence = 35,             // (not considered)
    llvm_op_AtomicCmpXchg = 36,     // (not considered)
    llvm_op_AtomicRMW = 37,         // (not considered)
    // Conversion
    llvm_op_Trunc = 38,             // (not considered)
    llvm_op_ZExt = 39,              // (not considered)
    llvm_op_SExt = 40,              // (not considered)
    llvm_op_FPToUI = 41,            // (not considered)
    llvm_op_FPToSI = 42,            // (not considered)
    llvm_op_UIToFP = 43,            // (not considered)
    llvm_op_SIToFP = 44,            // (not considered)
    llvm_op_FPTrunc = 45,           // (not considered)
    llvm_op_FPExt = 46,             // (not considered)
    llvm_op_PtrToInt = 47,          // (not considered)
    llvm_op_IntToPtr = 48,          // (not considered)
    llvm_op_BitCast = 49,           // (not considered)
    llvm_op_AddrSpaceCast = 50,     // (not considered)
    // Funclet Pad
    llvm_op_CleanupPad = 51,        // (not considered)
    llvm_op_CatchPad = 52,          // (not considered)
    // Other
    llvm_op_ICmp = 53,              // (not considered)
    llvm_op_FCmp = 54,              // (not considered)
    llvm_op_PHI = 55,               // implement the φ node in the SSA graph representing the function
    llvm_op_Call = 56,              // represents a simple function call
    llvm_op_Select = 57,            // (not considered)
    llvm_op_UserOp1 = 58,           // (not considered)
    llvm_op_UserOp2 = 59,           // (not considered)
    llvm_op_VAArg = 60,             // (not considered)
    // Vector
    llvm_op_ExtractElement = 61,    // (not considered)
    llvm_op_InsertElement = 62,     // (not considered)
    llvm_op_ShuffleVector = 63,     // (not considered)
    // Aggregate
    llvm_op_ExtractValue = 64,      // (not considered)
    llvm_op_InsertValue = 65,       // (not considered)
    llvm_op_LandingPad = 66,        // (not considered)
    // Other (continue)
    llvm_op_Freeze = 67,            // (not considered)
};

#endif