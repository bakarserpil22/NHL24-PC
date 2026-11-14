#pragma once
#include <stdint.h>
namespace FrostBridge { namespace HLE { class SyscallManager { public: static void HandleSyscall(class CpuContext* ctx); }; } }