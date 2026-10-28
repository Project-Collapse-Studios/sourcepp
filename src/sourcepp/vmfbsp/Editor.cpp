#include <sourcepp/vmfbsp/Editor.h>

EditorInfo::EditorInfo(const KeyValues& kv)
{
    this->color = kv("color").getValue<mathpp::Vec3i8>();
    this->visgroupid = kv.getChildrenValues<unsigned int>("visgroupid");
    this->group = kv("group").getValue<int>();
    this->visgroupshown = kv("visgroupshown").getValue<bool>();
    this->visgroupshown = kv("visgroupautoshown").getValue<bool>();
    this->comments = kv("comments").getValue();
    this->logicalpos = kv("logicalpos").getValue<mathpp::Vec2f>();
}
