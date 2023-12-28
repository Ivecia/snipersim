#ifndef __LLVMREADER_H_
#define __LLVMREADER_H_

namespace LLVM_Extension
{
    class LLVMReader
    {
    private:
        uint32_t m_id;      // Thread id
        char *m_code;       // Source code for LLVM
        char *m_memory;     // Memory trace
        char *m_branch;     // Branch trace
        // LLVM
        std::unique_ptr<llvm::Module> mod;

    public:
        LLVMReader(const char *code, const char *memory, const char *branch, uint32_t id);
        ~LLVMReader();
        bool initReader();
        bool Read();
    };
}

#endif