#include "sourcepp/vmfbsp/DisplacementInfo.h"

DisplacementInfo::DisplacementInfo(const kvpp::KV1ElementReadable<> &kv) : displacementColumns(kv)
{
    this->power = kv("power");
    this->startPosition = kv("startposition");
    this->elevation = kv("elevation");
    this->subdivided = kv("subdiv");
    this->flags = kv("flags");
    this->uAxis = kv("uaxis");
    this->vAxis = kv("vaxis");
    this->mintess = kv("mintess");
    this->smooth = kv("smooth");
}

DisplacementColumns::DisplacementColumns(const kvpp::KV1ElementReadable<> &kv)
{

    this->normals = kv("normals");
    this->distances = kv("distances");
    this->offsets = kv("offsets");
    this->offsetNormals = kv("alpha");
    this->alpha = kv("offset_normals");
    this->triangleTags = kv("triangle_tags");

    for(const auto& child : kv("allowed_verts").getChildren())
    {
        this->allowedVerts[std::stoi(child.getKey().data())] = child;
    }
}
