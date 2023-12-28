#include "LLVMReader.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define VERBOSE 1

LLVM_Extension::LLVMReader::LLVMReader(const char *code, const char *memory, const char *branch, uint32_t id)
{
    m_id     = id;
    m_code   = strdup(code);
    m_memory = strdup(memory);
    m_branch = strdup(branch);
}

LLVM_Extension::LLVMReader::~LLVMReader()
{
    free(m_code);
    free(m_memory);
    free(m_branch);
}

bool LLVM_Extension::LLVMReader::initReader()
{
    #if VERBOSE > 0
    std::cerr << "[DEBUG:" << m_id << "] Initializing LLVMReader" << std::endl;
    #endif
    // Initialize LLVM
    llvm::LLVMContext context;
    llvm::SMDiagnostic error;

    // Load the LLVM IR File
    mod = llvm::parseIRFile(m_code, error, context);
    if (!mod) {
        error.print("[LLVMReader - initReader]", llvm::errs());
        return false;
    }

    #if VERBOSE > 0
    std::cerr << "[DEBUG:" << m_id << "] LLVMReader Initialized" << std::endl;
    #endif
    return true;
}