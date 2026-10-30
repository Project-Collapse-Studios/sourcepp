#include "sourcepp/vmfbsp/DisplacementInfo.h"

DisplacementInfo::DisplacementInfo(const KeyValues &kv) : displacementColumns(kv)
{
    this->power = kv("power");
    if(this->power > 4)
        throw std::overflow_error("Power too high.");
    this->startPosition = kv("startposition");
    this->subdivided = kv("subdiv");
    this->elevation = kv("elevation");
    this->flags = kv("flags");
    this->uAxis = kv("uaxis");
    this->vAxis = kv("vaxis");
    this->mintess = kv("mintess");
    this->smooth = kv("smooth");
}

DisplacementColumns::DisplacementColumns(const KeyValues &kv)
{
}
