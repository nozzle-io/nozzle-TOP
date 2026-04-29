#pragma once

#include "TOP_CPlusPlusBase.h"
#include <bbb/nozzle/nozzle_c.h>
#include <string>

using namespace TD;

class NozzleReceiveTOP : public TOP_CPlusPlusBase
{
public:
    NozzleReceiveTOP(const OP_NodeInfo *info, TOP_Context *context);
    virtual ~NozzleReceiveTOP();

    virtual void getGeneralInfo(TOP_GeneralInfo *, const OP_Inputs *, void *reserved) override;
    virtual void execute(TOP_Output *, const OP_Inputs *, void *reserved) override;
    virtual void setupParameters(OP_ParameterManager *manager, void *reserved) override;

private:
    void update_receiver(const char *name);

    TOP_Context *myContext;
    NozzleReceiver *myReceiver{nullptr};
    std::string mySenderName;
    std::string myAppName;
};
