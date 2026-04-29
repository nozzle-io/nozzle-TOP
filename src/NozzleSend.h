#pragma once

#include "TOP_CPlusPlusBase.h"
#include <nozzle/nozzle_c.h>
#include <string>

using namespace TD;

class NozzleSendTOP : public TOP_CPlusPlusBase
{
public:
    NozzleSendTOP(const OP_NodeInfo *info, TOP_Context *context);
    virtual ~NozzleSendTOP();

    virtual void getGeneralInfo(TOP_GeneralInfo *, const OP_Inputs *, void *reserved) override;
    virtual void execute(TOP_Output *, const OP_Inputs *, void *reserved) override;
    virtual void setupParameters(OP_ParameterManager *manager, void *reserved) override;

private:
    void update_sender(const char *name);

    TOP_Context *myContext;
    NozzleSender *mySender{nullptr};
    std::string mySenderName;
    std::string myAppName;
};
