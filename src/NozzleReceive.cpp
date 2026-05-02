#include "NozzleReceive.h"
#include "pixel_format.h"

#include <cstring>
#include <cassert>

extern "C"
{

DLLEXPORT
void
FillTOPPluginInfo(TOP_PluginInfo *info)
{
    info->apiVersion = TOPCPlusPlusAPIVersion;
    info->executeMode = TOP_ExecuteMode::CPUMem;

    OP_CustomOPInfo &customInfo = info->customOPInfo;
    customInfo.opType->setString("Nozzlereceive");
    customInfo.opLabel->setString("Nozzle Receive");
    customInfo.authorName->setString("2bbb");
    customInfo.authorEmail->setString("noreply@2bbb.gg");
    customInfo.minInputs = 0;
    customInfo.maxInputs = 0;
    customInfo.cookOnStart = true;
}

DLLEXPORT
TOP_CPlusPlusBase *
CreateTOPInstance(const OP_NodeInfo *info, TOP_Context *context)
{
    return new NozzleReceiveTOP(info, context);
}

DLLEXPORT
void
DestroyTOPInstance(TOP_CPlusPlusBase *instance, TOP_Context *context)
{
    delete static_cast<NozzleReceiveTOP *>(instance);
}

} // extern "C"

static const char *SenderNamePar = "Sendername";
static const char *SenderNameLabel = "Sender Name";
static const char *AppNamePar = "Appname";
static const char *AppNameLabel = "Application Name";

NozzleReceiveTOP::NozzleReceiveTOP(const OP_NodeInfo *info, TOP_Context *context)
    : myContext{context}
{
}

NozzleReceiveTOP::~NozzleReceiveTOP()
{
    if (myReceiver) {
        nozzle_receiver_destroy(myReceiver);
        myReceiver = nullptr;
    }
}

void
NozzleReceiveTOP::getGeneralInfo(TOP_GeneralInfo *ginfo, const OP_Inputs *, void *)
{
    ginfo->cookEveryFrame = true;
    ginfo->cookEveryFrameIfAsked = true;
}

void
NozzleReceiveTOP::execute(TOP_Output *output, const OP_Inputs *inputs, void *)
{
    const char *sender_name = inputs->getParString(SenderNamePar);
    update_receiver(sender_name);

    if (!myReceiver) {
        return;
    }

    NozzleAcquireDesc acquire_desc{};
    acquire_desc.timeout_ms = 0;

    NozzleFrame *frame = nullptr;
    NozzleErrorCode err = nozzle_receiver_acquire_frame(myReceiver, &acquire_desc, &frame);

    if (err != NOZZLE_OK || !frame) {
        return;
    }

    NozzleFrameInfo frame_info{};
    err = nozzle_frame_get_info(frame, &frame_info);

    if (err != NOZZLE_OK) {
        nozzle_frame_release(frame);
        return;
    }

    NozzleMappedPixels pixels;
    err = nozzle_frame_lock_pixels(frame, &pixels);

    if (err != NOZZLE_OK) {
        nozzle_frame_release(frame);
        return;
    }

    TD::OP_PixelFormat td_fmt = nozzle_top::nozzle_to_td_format(pixels.format);
    uint32_t bpp = nozzle_top::bytes_per_pixel_td(td_fmt);
    uint32_t src_row_bytes = pixels.width * bpp;
    uint64_t byte_size = static_cast<uint64_t>(src_row_bytes) * pixels.height;

    // Create output buffer and copy pixel data
    OP_SmartRef<TOP_Buffer> out_buf = myContext->createOutputBuffer(byte_size, TOP_BufferFlags::None, nullptr);

    if (out_buf) {
        if (pixels.row_bytes == src_row_bytes) {
            std::memcpy(out_buf->data, pixels.data, byte_size);
        } else {
            uint8_t *dst = static_cast<uint8_t *>(out_buf->data);
            const uint8_t *src = static_cast<const uint8_t *>(pixels.data);
            uint32_t copy_bytes = (src_row_bytes < pixels.row_bytes) ? src_row_bytes : pixels.row_bytes;
            for (uint32_t y = 0; y < pixels.height; ++y) {
                std::memcpy(dst, src, copy_bytes);
                dst += src_row_bytes;
                src += pixels.row_bytes;
            }
        }

        TOP_UploadInfo info;
        info.textureDesc.width = pixels.width;
        info.textureDesc.height = pixels.height;
        info.textureDesc.pixelFormat = td_fmt;
        info.textureDesc.texDim = TD::OP_TexDim::e2D;
        info.colorBufferIndex = 0;

        output->uploadBuffer(&out_buf, info, nullptr);
    }

    nozzle_frame_unlock_pixels(frame);
    nozzle_frame_release(frame);
}

void
NozzleReceiveTOP::setupParameters(OP_ParameterManager *manager, void *)
{
    {
        OP_StringParameter p;
        p.name = SenderNamePar;
        p.label = SenderNameLabel;
        p.page = "Nozzle";
        p.defaultValue = "nozzle";
        assert(manager->appendString(p) == OP_ParAppendResult::Success);
    }
    {
        OP_StringParameter p;
        p.name = AppNamePar;
        p.label = AppNameLabel;
        p.page = "Nozzle";
        p.defaultValue = "TouchDesigner";
        assert(manager->appendString(p) == OP_ParAppendResult::Success);
    }
}

void
NozzleReceiveTOP::update_receiver(const char *name)
{
    if (myReceiver && mySenderName == name) {
        return;
    }

    if (myReceiver) {
        nozzle_receiver_destroy(myReceiver);
        myReceiver = nullptr;
    }

    mySenderName = name ? name : "";

    NozzleReceiverDesc desc{};
    desc.name = mySenderName.c_str();
    desc.application_name = "TouchDesigner";
    desc.receive_mode = NOZZLE_RECEIVE_LATEST_ONLY;

    NozzleErrorCode err = nozzle_receiver_create(&desc, &myReceiver);
    if (err != NOZZLE_OK) {
        FILE *f = fopen("/tmp/nozzle_td_debug.log", "a");
        if (f) { fprintf(f, "[nozzle] receiver create failed for '%s': error %d\n", mySenderName.c_str(), static_cast<int>(err)); fclose(f); }
        myReceiver = nullptr;
    } else {
        FILE *f = fopen("/tmp/nozzle_td_debug.log", "a");
        if (f) { fprintf(f, "[nozzle] receiver connected to '%s'\n", mySenderName.c_str()); fclose(f); }
    }
}
