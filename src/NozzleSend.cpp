#include "NozzleSend.h"
#include "pixel_format.h"

#include <cstring>
#include <cassert>
#include <cstdio>

extern "C"
{

DLLEXPORT
void
FillTOPPluginInfo(TOP_PluginInfo *info)
{
    info->apiVersion = TOPCPlusPlusAPIVersion;
    info->executeMode = TOP_ExecuteMode::CPUMem;

    OP_CustomOPInfo &customInfo = info->customOPInfo;
    customInfo.opType->setString("Nozzlesend");
    customInfo.opLabel->setString("Nozzle Send");
    customInfo.authorName->setString("2bbb");
    customInfo.authorEmail->setString("noreply@2bbb.gg");
    customInfo.minInputs = 1;
    customInfo.maxInputs = 1;
}

DLLEXPORT
TOP_CPlusPlusBase *
CreateTOPInstance(const OP_NodeInfo *info, TOP_Context *context)
{
    return new NozzleSendTOP(info, context);
}

DLLEXPORT
void
DestroyTOPInstance(TOP_CPlusPlusBase *instance, TOP_Context *context)
{
    delete static_cast<NozzleSendTOP *>(instance);
}

} // extern "C"

static const char *SenderNamePar = "Sendername";
static const char *SenderNameLabel = "Sender Name";
static const char *AppNamePar = "Appname";
static const char *AppNameLabel = "Application Name";

NozzleSendTOP::NozzleSendTOP(const OP_NodeInfo *info, TOP_Context *context)
    : myContext{context}
{
}

NozzleSendTOP::~NozzleSendTOP()
{
    if (mySender) {
        nozzle_sender_destroy(mySender);
        mySender = nullptr;
    }
}

void
NozzleSendTOP::getGeneralInfo(TOP_GeneralInfo *ginfo, const OP_Inputs *, void *)
{
    ginfo->cookEveryFrameIfAsked = false;
    ginfo->inputSizeIndex = 0;
}

void
NozzleSendTOP::execute(TOP_Output *output, const OP_Inputs *inputs, void *)
{
    const OP_TOPInput *top = inputs->getInputTOP(0);
    if (!top) {
        return;
    }

    const char *sender_name = inputs->getParString(SenderNamePar);
    const char *app_name = inputs->getParString(AppNamePar);
    update_sender(sender_name);

    if (!mySender) {
        return;
    }

    int32_t width = static_cast<int32_t>(top->textureDesc.width);
    int32_t height = static_cast<int32_t>(top->textureDesc.height);
    OP_PixelFormat pixel_format = top->textureDesc.pixelFormat;

    OP_TOPInputDownloadOptions opts;
    opts.pixelFormat = pixel_format;

    OP_SmartRef<OP_TOPDownloadResult> down_res = top->downloadTexture(opts, nullptr);
    if (!down_res) {
        return;
    }

    void *src_data = down_res->getData();
    if (!src_data) {
        return;
    }

    uint64_t byte_size = down_res->size;

    NozzleTextureFormat nz_fmt = nozzle_top::td_to_nozzle_format(pixel_format);
    if (nz_fmt != NOZZLE_FORMAT_UNKNOWN) {
        NozzleFrame *frame = nullptr;
        NozzleErrorCode err = nozzle_sender_acquire_writable_frame(
            mySender, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
            nz_fmt, &frame);

#if NOZZLE_TOP_DEBUG
        {
            static int count = 0;
            if (++count <= 10) {
                FILE *f = fopen("/tmp/nozzle_send_td_debug.log", "a");
                if (f) { fprintf(f, "[%d] acquire: err=%d frame=%p w=%d h=%d fmt=%d\n", count, (int)err, frame, width, height, (int)nz_fmt); fclose(f); }
            }
        }
#endif

        if (err == NOZZLE_OK && frame) {
            NozzleMappedPixels pixels;
            err = nozzle_frame_lock_writable_pixels(frame, &pixels);
            if (err == NOZZLE_OK) {
                uint32_t src_row_bytes = width * nozzle_top::bytes_per_pixel_td(pixel_format);
                uint32_t dst_row_bytes = pixels.row_bytes;
                uint32_t copy_bytes = (src_row_bytes < dst_row_bytes) ? src_row_bytes : dst_row_bytes;

                if (src_row_bytes == dst_row_bytes) {
                    std::memcpy(pixels.data, src_data, byte_size);
                } else {
                    uint8_t *dst = static_cast<uint8_t *>(pixels.data);
                    const uint8_t *src = static_cast<const uint8_t *>(src_data);
                    for (int32_t y = 0; y < height; ++y) {
                        std::memcpy(dst, src, copy_bytes);
                        dst += dst_row_bytes;
                        src += src_row_bytes;
                    }
                }

                nozzle_frame_unlock_writable_pixels(frame);
            }
            NozzleErrorCode commit_err = nozzle_sender_commit_frame(mySender, frame);

#if NOZZLE_TOP_DEBUG
            {
                static int commit_count = 0;
                if (++commit_count <= 10) {
                    FILE *f = fopen("/tmp/nozzle_send_td_debug.log", "a");
                    if (f) { fprintf(f, "[%d] commit: err=%d\n", commit_count, (int)commit_err); fclose(f); }
                }
            }
#endif
        }
    }

    TOP_UploadInfo info;
    info.textureDesc = down_res->textureDesc;
    info.colorBufferIndex = 0;

    OP_SmartRef<TOP_Buffer> out_buf = myContext->createOutputBuffer(byte_size, TOP_BufferFlags::None, nullptr);
    if (out_buf) {
        std::memcpy(out_buf->data, src_data, byte_size);
        output->uploadBuffer(&out_buf, info, nullptr);
    }
}

void
NozzleSendTOP::setupParameters(OP_ParameterManager *manager, void *)
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
NozzleSendTOP::update_sender(const char *name)
{
    if (mySender && mySenderName == name) {
        return;
    }

    if (mySender) {
        nozzle_sender_destroy(mySender);
        mySender = nullptr;
    }

    mySenderName = name ? name : "";

    NozzleSenderDesc desc{};
    desc.name = mySenderName.c_str();
    desc.application_name = "TouchDesigner";
    desc.ring_buffer_size = 3;

    NozzleErrorCode err = nozzle_sender_create(&desc, &mySender);
    if (err != NOZZLE_OK) {
        mySender = nullptr;
    }
}
