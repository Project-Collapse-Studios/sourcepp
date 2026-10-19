
#include <map>
#include <string_view>
#include <vector>

#include "sourcepp/Math.h"
#include "kvpp/KV1.h"
#include "vmfpp/VMF.h"


std::string_view vmfpp::StringReference::getStringView() const {
    if (!valid)
        return "";
    return {this->reference->data() + this->start, this->reference->data() + this->end};
}

std::string vmfpp::StringReference::getString() const {
    if (!valid)
        return "";
    return std::string(this->getStringView());
}

bool vmfpp::VMF::VersionInfo::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_)
{
    if (kv.hasChild("editorextension"))
        this->editorExtension = vmfpp::StringReference::newStringReference(kv["editorextension"].getValue(), backing_);
    this->editorVersion = kv["editorversion"].getValue<int>();
    this->editorBuild = kv["editorbuild"].getValue<int>();
    this->mapVersion = kv["mapversion"].getValue<int>();
    this->formatVersion = kv["formatversion"].getValue<int>();
    this->prefab = kv["prefab"].getValue<bool>();
    return true;
}

bool vmfpp::VMF::VisGroup::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_) {
    this->name = vmfpp::StringReference::newStringReference(kv["name"].getValue(), backing_);
    this->visgroupID = kv["visgroupid"].getValue<int>();
    this->color = parseColor(kv["color"].getValue());
    for (int i = 0; i < kv.getChildCount("visgroup"); i++)
    {
        const auto& vis = kv("visgroup",i);
        if (auto& branch = this->branches.emplace_back(); !branch.parse(vis,backing_))
            return false;
    }
    return true;
}

bool vmfpp::VMF::ViewSettings::parse(const kvpp::KV1ElementReadable<>& kv) {

    this->snapToGrid = kv("bSnapToGrid").getValue<bool>();
    this->showGrid = kv("bShowGrid").getValue<bool>();
    this->showLogicalGrid = kv("bShowLogicalGrid").getValue<bool>();
    this->gridSpacing = kv("nGridSpacing").getValue<int>();
    this->show3DGrid = kv("bShow3DGrid").getValue<bool>();
    return true;
}

bool vmfpp::VMF::UVAxis::parse(const kvpp::KV1ElementReadable<>& kv) {
    const auto value = kv.getValue();
    for (int i = 0; i < value.length(); i++)
    {
        if (value[i] == '[')
        {
            const int start = i;

            while (value[i] != ']')
                i++;

            auto vals = sourcepp::string::split(std::string_view {value.data() + start + 1, value.data() + i}, ' ');
            i++;

            this->x = std::stof(vals[0]);
            this->y = std::stof(vals[1]);
            this->z = std::stof(vals[2]);
            this->degrees = std::stof(vals[3]);
            i++;

            auto scale = value.substr(i);
            auto stScale = std::string(scale);
            float flScale = 0;
            std::from_chars(scale.data(), scale.data() + scale.size(), flScale);
            this->scaling = flScale;
        }

    }
    return true;
}

bool vmfpp::VMF::Plane::parse(const kvpp::KV1ElementReadable<>& kv) {

    const auto split = sourcepp::string::split(kv.getValue(), '(');
    bool offset = false;
    if (split[0].empty())
        offset = true;
    const auto split1 = sourcepp::string::split(split[0 + offset],')')[0];
    const auto split2 = sourcepp::string::split(split[1 + offset],')')[0];
    const auto split3 = sourcepp::string::split(split[2 + offset],')')[0];

    this->bottomLeft = parsePlane(split1);
    this->TopLeft = parsePlane(split2);
    this->TopRight = parsePlane(split3);

    return true;
}

bool vmfpp::VMF::VertexColumn::parse(const kvpp::KV1ElementReadable<> &kv) {
    return false;
}

bool vmfpp::VMF::TagColumn::parse(const kvpp::KV1ElementReadable<> &kv) {
    return false;
}

bool vmfpp::VMF::DecimalColumn::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing) {
    return false;
}

bool vmfpp::VMF::DisplacementInfo::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_) {

    this->power = kv("power").getValue<int>();

    const auto startpos = kv("startposition").getValue();
    const auto split = sourcepp::string::split(startpos.substr(1, startpos.length() - 1), ' ');
    this->startPosition = {std::stof(split[0]), std::stof(split[1]), std::stof(split[2])};

    this->elevation = kv("elevation").getValue<float>();
    this->isSubdivided = kv("subdiv").getValue<bool>();
    this->flags = kv("flags").getValue<int>();
    this->isSubdivided = kv("subdiv").getValue<bool>();

    for (int i = 0; i < kv("normals").getChildCount(); i++)
    {
        auto& normal = this->normals.emplace_back();
        normal.parse(kv("normals")[i]);
    }

    for (int i = 0; i < kv("distances").getChildCount(); i++)
    {
        auto& distance = this->distances.emplace_back();
        distance.parse(kv("distances")[i], backing_);
    }

    for (int i = 0; i < kv("offsets").getChildCount(); i++)
    {
        auto& offset = this->offsets.emplace_back();
        offset.parse(kv("offsets")[i]);
    }

    for (int i = 0; i < kv("offset_normals").getChildCount(); i++)
    {
        auto& offset = this->offsetNormals.emplace_back();
        offset.parse(kv("offset_normals")[i]);
    }

    for (int i = 0; i < kv("alphas").getChildCount(); i++)
    {
        auto& alpha = this->alphas.emplace_back();
        alpha.parse(kv("alphas")[i], backing_);
    }

    for (int i = 0; i < kv("triangle_tags").getChildCount(); i++)
    {
        auto& tag = this->triangleTags.emplace_back();
        tag.parse(kv("triangle_tags")[i]);
    }

    for (int i = 0; i < kv("allowed_verts").getChildCount(); i++)
    {
        auto &tags = this->allowedTags[std::stoi(std::string(kv("allowed_verts")[i].getKey()))];
        auto splitTags = sourcepp::string::split(kv("allowed_verts").getValue(), ' ');
        for(const auto& tag : splitTags)
            tags.push_back(std::stoi(tag));
    }

    return true;
}

bool vmfpp::VMF::Side::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_) {

    this->id = kv("id").getValue<int>();
    this->plane.parse(kv("plane"));
    this->material = vmfpp::StringReference::newStringReference(kv("material").getValue(),backing_);
    this->uAXis.parse(kv("uaxis"));
    this->vAXis.parse(kv("vaxis"));
    this->rotation = kv("rotation").getValue<float>();
    this->lightmapScale = kv("lightmapscale").getValue<int>();
    this->smoothingGroups = kv("smoothing_groups").getValue<int>();
    this->contents = kv("contents").getValue<int>();
    this->flags = kv("flags").getValue<int>();
    if (kv.hasChild("dispinfo"))
        this->displacementInfo.parse(kv("dispinfo"), backing_);

    return true;
}

bool vmfpp::VMF::Editor::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_) {
    this->color = parseColor(kv("color").getValue());

    if (kv.hasChild("visgroupid"))
        this->visgroupID = kv("visgroupid").getValue<int>();
    if (kv.hasChild("groupid"))
        this->groupID = kv("groupid").getValue<int>();

    this->visgroupShown = kv("visgroupshown").getValue<bool>();
    this->visgroupAutoShown = kv("visgroupautoshown").getValue<bool>();

    if (kv.hasChild("comments"))
        this->comments = vmfpp::StringReference::newStringReference(kv("comments").getValue(), backing_);

    if (kv.hasChild("logicalpos"))
    {
        const auto logicposString = kv("logicalpos").getValue();
        const auto split = sourcepp::string::split(logicposString.substr(1, logicposString.length() - 1), ' ');
        this->logicalPos = {std::stof(split[0]), std::stof(split[1])};
    }

    return true;
}

void vmfpp::VMF::Solid::parse(const kvpp::KV1ElementReadable<>& kv, std::string* backing_) {

    this->id = kv("id").getValue<int>();
    for (int i = 0; i < kv.getChildCount("side"); i++)
    {
        auto& side = this->sides.emplace_back();
        side.parse(kv("side",i),backing_);
    }
    this->editor.parse(kv("editor"),backing_);
}

void vmfpp::VMF::Group::parse(const kvpp::KV1ElementReadable<>& kv, std::string* backing_) {
    this->id = kv("id").getValue<int>();
    this->editor.parse(kv("editor"),backing_);
}

bool vmfpp::VMF::World::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_) {
    this->id = kv("id").getValue<int>();
    this->mapVersion = kv("mapversion").getValue<int>(); //mapversion is marked as sz (string) on the wiki, this is probably a mistake.
    this->className = vmfpp::StringReference::newStringReference(kv("classname").getValue(), backing_);
    this->skyName = vmfpp::StringReference::newStringReference(kv("skyname").getValue(), backing_);

    for (int i = 0; i < kv.getChildCount("solid"); i++)
    {
        if (kv("solid").getChildCount() < 1) {

            this->keyvalues["solid"] = vmfpp::StringReference::newStringReference(kv("solid").getValue(), backing_);
            continue;
        }

        auto& solid = this->solids.emplace_back();
        solid.parse( kv("solid",i), backing_ );
    }


    for (int i = 0; i < kv.getChildCount("group"); i++)
    {
        auto& group = this->groups.emplace_back();
        group.parse( kv("group",i), backing_ );
    }

    static constexpr std::array<std::string_view, 8> nonKV {"id","classname", "mapversion", "skyname", "solid", "hidden", "origin", "group"};

    for (const auto& keyvalue : kv)
    {
        if (std::ranges::find(nonKV,keyvalue.getKey()) != nonKV.end())
            continue;

        this->keyvalues[std::string(keyvalue.getKey())] = vmfpp::StringReference::newStringReference(keyvalue.getValue(),backing_);
    }

    return true;
}

bool vmfpp::VMF::Connection::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_) {

    this->output = vmfpp::StringReference::newStringReference(kv.getKey(),backing_);

    auto sp = kv.getValue();

    auto splitCon = sourcepp::string::split(kv.getValue(),0x1b);
    if (splitCon.size() == 1) //We didn't find the 0x1b delim, must be non-VScript
        splitCon = sourcepp::string::split(kv.getValue(),',');

    if (splitCon.size() <= 1) //Still didn't find anything, must be corrupt. :/.
        return false;


    if (!splitCon[0].empty())
         this->entity = vmfpp::StringReference::newStringReference(splitCon[0],backing_);
    if (!splitCon[1].empty())
         this->input = vmfpp::StringReference::newStringReference(splitCon[1],backing_);
    if (!splitCon[2].empty())
         this->parameter = vmfpp::StringReference::newStringReference(splitCon[2],backing_);
    if (!splitCon[3].empty())
        std::from_chars(splitCon[3].data(), splitCon[3].data() + splitCon[3].size(), this->delay);
    if (!splitCon[4].empty())
        std::from_chars(splitCon[4].data(), splitCon[4].data() + splitCon[3].size(), this->delay);


    return true;
}

bool vmfpp::VMF::OverlayData::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing) {
    return false;
}

bool vmfpp::VMF::Entity::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing) {
    this->id = kv("id").getValue<int>();
    this->className = vmfpp::StringReference::newStringReference(kv("classname").getValue(), backing);
    this->spawnflags = kv("classname").getValue<int>();

    if (kv.hasChild("origin"))
    {
        const auto originStr = kv("origin").getValue();
        const auto xyz = sourcepp::string::split(originStr,' ');
        std::from_chars(xyz[0].data(), xyz[0].data() + xyz[0].length(), this->origin[0]);
        std::from_chars(xyz[1].data(), xyz[1].data() + xyz[1].length(), this->origin[1]);
        std::from_chars(xyz[2].data(), xyz[2].data() + xyz[2].length(), this->origin[2]);
    }

    for (int i = 0; i < kv.getChildCount("solid"); i++)
    {
        if (kv("solid").getChildCount() < 1) {

            this->keyvalues["solid"] = vmfpp::StringReference::newStringReference(kv("solid").getValue(), backing);
            break;
        }

        auto& solid = this->solids.emplace_back();
        solid.parse( kv("solid",i), backing );
    }

    for (int i = 0; i < kv.getChildCount("group"); i++)
    {
        if (kv("group").getChildCount() < 1) {

            this->keyvalues["group"] = vmfpp::StringReference::newStringReference(kv("group").getValue(), backing);
            continue;
        }

        auto& group = this->groups.emplace_back();
        group.parse( kv("group",i), backing );
    }

    for (int i = 0; i < kv.getChildCount("hidden"); i++)
    {

        if (kv("hidden").getChildCount() < 1) {
            this->keyvalues["hidden"] = vmfpp::StringReference::newStringReference(kv("hidden").getValue(), backing);
            continue;
        }

        for (int j = 0; j < kv("hidden").getChildCount("solid"); j++)
            this->hidden.emplace_back().parse(kv("hidden")("solid", j), backing);

    }

    for (int i = 0; i < kv.getChildCount("connections"); i++)
    {
        if (kv("connections").getChildCount() < 1) {
            this->keyvalues["connections"] = vmfpp::StringReference::newStringReference(kv("connections").getValue(), backing);
            continue;
        }

        if (const auto& conn = kv("connections")[i]; !this->connections.emplace_back().parse(conn, backing))
            return false;
    }

    static constexpr std::array<std::string_view, 8> nonKV {"id","classname", "spawnflags", "connections", "solid", "hidden", "origin", "editor"};

    for (const auto& keyvalue : kv)
    {
        if(std::ranges::find(nonKV,keyvalue.getKey()) != nonKV.end())
            continue;

        this->keyvalues[std::string(keyvalue.getKey())] = vmfpp::StringReference::newStringReference(keyvalue.getValue(),backing);
    }

    return true;
}

bool vmfpp::VMF::Camera::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing) {
    return false;
}

bool vmfpp::VMF::Cordon::parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing) {
    return false;
}

vmfpp::VMF::VMF() : opened(false) {};

vmfpp::VMF::VMF(const std::filesystem::path& path)
{
    auto file = sourcepp::fs::readFileText(path);
    file.insert(0, "map {");
    file.append("}");
    const auto keyv = kvpp::KV1(file);
    this->opened = this->load(keyv);
}

vmfpp::VMF::VMF(const kvpp::KV1<>& kv)
{
    this->opened = this->load(kv);
}

bool vmfpp::VMF::load(const kvpp::KV1<> &kv) {

    if (!kv.operator bool())
        return false;

    const auto map = kv["map"];
    if (!map.hasChild("versioninfo")) // Essentially the header, without it, file's invalid.
        return false;

    this->backing = new std::string();
    backing->reserve(map.getChildCount() * 128); //Bold assumption, but it's fine.

    for(const auto &mapChild : map.getChildren())
    {
        if (mapChild.getKey() == "versioninfo")
            this->versionInfo.parse(mapChild, this->backing);

        if(mapChild.getKey() == "visgroups")
        {
            auto groups = map["visgroups"];
            for (int i = 0; i < groups.getChildCount("visgroup"); i++) {
                const auto &vis = groups("visgroup", i);
                if (auto& visgroup = this->visgroups.emplace_back(); !visgroup.parse(vis, this->backing))
                    return false;
            }
        }

        if(mapChild.getKey() == "viewsettings")
            this->viewSettings.parse(mapChild);

        if(mapChild.getKey() == "world")
        {
            world.parse(mapChild, backing);
        }

        if(mapChild.getKey() == "entity")
        {
            if (auto& entity = this->entities.emplace_back(); !entity.parse(mapChild, backing))
                return false;
        }

    }
    return true;
}

mathpp::Vec3i8 vmfpp::VMF::parseColor(const std::string_view str) {

    const auto splitStr = sourcepp::string::split(str, ' ');
    if (splitStr.size() != 3)
        return {};
    return {std::stoi(splitStr[0]), std::stoi(splitStr[1]), std::stoi(splitStr[2])};
}

mathpp::Vec3f vmfpp::VMF::parsePlane(const std::string_view str) {

    const auto splitStr = sourcepp::string::split(str, ' ');
    if (splitStr.size() != 3)
        return {};
    return {std::stof(splitStr[0]), std::stof(splitStr[1]), std::stof(splitStr[2])};
}
