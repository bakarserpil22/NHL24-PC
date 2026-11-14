#include "syscalls.h"
#include "kernel/process.h"
#include "kernel/memory.h"
#include <iostream>

namespace FrostBridge {
namespace HLE {

void SyscallManager::HandleSyscall(CpuContext* ctx) {
    uint32_t syscall_num = ctx->rax;
    
    switch(syscall_num) {
        case 1: // sys_exit
            Process::Exit(ctx->rdi);
            break;
        case 3: // sys_read
            ctx->rax = VFS::Read(ctx->rdi, (void*)ctx->rsi, ctx->rdx);
            break;
        case 4: // sys_write
            ctx->rax = VFS::Write(ctx->rdi, (const void*)ctx->rsi, ctx->rdx);
            break;
        case 477: // sys_mmap (Modified for Windows VRAM backing)
            ctx->rax = Memory::Mmap(ctx->rdi, ctx->rsi, ctx->rdx, ctx->r10, ctx->r8, ctx->r9);
            break;
        case 532: // sys_app_state_change
            // Dummy implementation to bypass game state checks
            ctx->rax = 0; 
            break;
        case 586: // sys_get_cpu_usage
            ctx->rax = 0; // Fake low CPU usage to bypass dynamic resolution drops
            break;
        default:
            std::cerr << "[SYSCALL] Unimplemented syscall: " << syscall_num << std::endl;
            ctx->rax = -1; // ENOSYS
            break;
    }
}

} // namespace HLE
} // namespace FrostBridge
