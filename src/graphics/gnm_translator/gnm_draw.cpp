#include "gnm_translator.h"

namespace FrostBridge {
namespace GNM {

void SceGnmDrawIndexAuto(uint32_t indexCount, bool useOpaque) {
    // Translates PS4 GNM draw calls to Vulkan vkCmdDrawIndexed
    auto cmdbuf = VulkanBackend::GetCurrentCommandBuffer();
    
    if (useOpaque) {
        VulkanBackend::BindOpaquePipeline(cmdbuf);
    } else {
        VulkanBackend::BindTransparentPipeline(cmdbuf);
    }
    
    // EA specific index buffer offset patch
    uint32_t ea_offset = Memory::Read32(0x1400AABBCC);
    vkCmdDrawIndexed(cmdbuf, indexCount, 1, ea_offset, 0, 0);
}

}
}
