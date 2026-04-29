#pragma once

#include <CPlusPlus_Common.h>
#include <bbb/nozzle/nozzle_c.h>
#include <cstdint>

namespace nozzle_top {

inline NozzleTextureFormat td_to_nozzle_format(TD::OP_PixelFormat fmt)
{
    using PF = TD::OP_PixelFormat;
    switch (fmt) {
        case PF::BGRA8Fixed:   return NOZZLE_FORMAT_BGRA8_UNORM;
        case PF::RGBA8Fixed:   return NOZZLE_FORMAT_RGBA8_UNORM;
        case PF::RGBA16Fixed:  return NOZZLE_FORMAT_RGBA16_UNORM;
        case PF::RGBA16Float:  return NOZZLE_FORMAT_RGBA16_FLOAT;
        case PF::RGBA32Float:  return NOZZLE_FORMAT_RGBA32_FLOAT;
        case PF::Mono8Fixed:   return NOZZLE_FORMAT_R8_UNORM;
        case PF::Mono16Fixed:  return NOZZLE_FORMAT_R16_UNORM;
        case PF::Mono16Float:  return NOZZLE_FORMAT_R16_FLOAT;
        case PF::Mono32Float:  return NOZZLE_FORMAT_R32_FLOAT;
        case PF::RG8Fixed:     return NOZZLE_FORMAT_RG8_UNORM;
        case PF::RG16Fixed:    return NOZZLE_FORMAT_RG16_UNORM;
        case PF::RG16Float:    return NOZZLE_FORMAT_RG16_FLOAT;
        case PF::RG32Float:    return NOZZLE_FORMAT_RG32_FLOAT;
        default:               return NOZZLE_FORMAT_UNKNOWN;
    }
}

inline TD::OP_PixelFormat nozzle_to_td_format(NozzleTextureFormat fmt)
{
    using PF = TD::OP_PixelFormat;
    switch (fmt) {
        case NOZZLE_FORMAT_BGRA8_UNORM:  return PF::BGRA8Fixed;
        case NOZZLE_FORMAT_RGBA8_UNORM:  return PF::RGBA8Fixed;
        case NOZZLE_FORMAT_RGBA16_UNORM: return PF::RGBA16Fixed;
        case NOZZLE_FORMAT_RGBA16_FLOAT: return PF::RGBA16Float;
        case NOZZLE_FORMAT_RGBA32_FLOAT: return PF::RGBA32Float;
        case NOZZLE_FORMAT_R8_UNORM:     return PF::Mono8Fixed;
        case NOZZLE_FORMAT_R16_UNORM:    return PF::Mono16Fixed;
        case NOZZLE_FORMAT_R16_FLOAT:    return PF::Mono16Float;
        case NOZZLE_FORMAT_R32_FLOAT:    return PF::Mono32Float;
        case NOZZLE_FORMAT_RG8_UNORM:    return PF::RG8Fixed;
        case NOZZLE_FORMAT_RG16_UNORM:   return PF::RG16Fixed;
        case NOZZLE_FORMAT_RG16_FLOAT:   return PF::RG16Float;
        case NOZZLE_FORMAT_RG32_FLOAT:   return PF::RG32Float;
        default:                         return PF::BGRA8Fixed;
    }
}

inline uint32_t bytes_per_pixel_td(TD::OP_PixelFormat fmt)
{
    using PF = TD::OP_PixelFormat;
    switch (fmt) {
        case PF::Mono8Fixed:   return 1;
        case PF::Mono16Fixed:
        case PF::Mono16Float:  return 2;
        case PF::Mono32Float:  return 4;
        case PF::RG8Fixed:     return 2;
        case PF::RG16Fixed:
        case PF::RG16Float:    return 4;
        case PF::RG32Float:    return 8;
        case PF::BGRA8Fixed:
        case PF::RGBA8Fixed:   return 4;
        case PF::RGBA16Fixed:
        case PF::RGBA16Float:  return 8;
        case PF::RGBA32Float:  return 16;
        default:               return 4;
    }
}

} // namespace nozzle_top
