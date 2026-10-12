
#include <kvpp/VMF.h>

kvpp::VMF::StringReference kvpp::VMF::newStringReference(std::string_view str, std::string* backing)
{

    auto substr = backing->find(str);
    if(substr != std::string::npos)
    {
        return {substr, substr + str.length(),backing};
    }

    size_t start = backing->length();
    backing->append(str);
    size_t end = backing->length();
    return {start,end,backing};
}

sourcepp::math::Vec3i8 kvpp::VMF::parseColor(std::string_view str) {

    auto splitStr = sourcepp::string::split(str, ' ');
    if(splitStr.size() != 3)
        return {};
    return {std::stoi(splitStr[0]), std::stoi(splitStr[1]), std::stoi(splitStr[2])};
}

sourcepp::math::Vec3f kvpp::VMF::parsePlane(std::string_view str) {

    auto splitStr = sourcepp::string::split(str, ' ');
    if(splitStr.size() != 3)
        return {};
    return {std::stof(splitStr[0]), std::stof(splitStr[1]), std::stof(splitStr[2])};
}

kvpp::VMF::VMF() : opened(false)
{

}

kvpp::VMF::VMF(const std::filesystem::path& path)
{
    auto file = sourcepp::fs::readFileText(path);
    file.insert(0, "map {");
    file.append("}");
    kvpp::KV1<std::string_view> keyv = kvpp::KV1(file);
    this->opened = this->load(keyv);
}

kvpp::VMF::VMF(const kvpp::KV1<std::string_view>& kv)
{
    this->opened = this->load(kv);
}

bool kvpp::VMF::load(const kvpp::KV1<std::string_view> &kv) {

    if(!kv.operator bool())
        return false;

    auto map = kv["map"];

    if(!map.hasChild("versioninfo")) // Essentially the header, without it, file's invalid.
        return false;

    this->backing = new std::string();
    backing->reserve(map.getChildCount() * 128); //Bold assumption, but it's fine.

    for(const auto &mapChild : map.getChildren())
    {
        if(mapChild.getKey() == "versioninfo")
            if(!this->versionInfo.parse(mapChild, this->backing))
                return false;

        if(mapChild.getKey() == "visgroups")
        {
            auto groups = map["visgroups"];
            for(int i = 0; i < groups.getChildCount("visgroup"); i++) {
                const auto &vis = groups("visgroup", i);
                auto& visgroup = this->visgroups.emplace_back();
                if(!visgroup.parse(vis, this->backing))
                    return false;
            }
        }

        if(mapChild.getKey() == "viewsettings")
            if(!this->viewSettings.parse(mapChild, backing))
                return false;

        if(mapChild.getKey() == "world")
        {
            if(!world.parse(mapChild, backing))
                return false;
        }

        if(mapChild.getKey() == "entity")
        {
            auto& entity = this->entities.emplace_back();
            if(!entity.parse(mapChild, backing))
                return false;
        }

    }
    return true;
}

bool
kvpp::VMF::VersionInfo::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing)
{

    if(kv.hasChild("editorextension"))
        this->editorExtension = VMF::newStringReference(kv["editorextension"].getValue(), backing);
    this->editorVersion = kv["editorversion"].getValue<int>();
    this->editorBuild = kv["editorbuild"].getValue<int>();
    this->mapVersion = kv["mapversion"].getValue<int>();
    this->formatVersion = kv["formatversion"].getValue<int>();
    this->prefab = kv["prefab"].getValue<bool>();
    return true;
}

bool
kvpp::VMF::VisGroup::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    this->name = newStringReference(kv["name"].getValue(), backing);
    this->visgroupID = kv["visgroupid"].getValue<int>();
    this->color = parseColor(kv["color"].getValue());
    for(int i = 0; i < kv.getChildCount("visgroup"); i++)
    {
        const auto& vis = kv("visgroup",i);
        auto& branch = this->branches.emplace_back();
        if(!branch.parse(vis,backing))
           return false;
    }
    return true;
}

bool
kvpp::VMF::ViewSettings::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {

    this->snapToGrid = kv("bSnapToGrid").getValue<bool>();
    this->showGrid = kv("bShowGrid").getValue<bool>();
    this->showLogicalGrid = kv("bShowLogicalGrid").getValue<bool>();
    this->gridSpacing = kv("nGridSpacing").getValue<int>();
    this->show3DGrid = kv("bShow3DGrid").getValue<bool>();
    return true;
}

bool kvpp::VMF::UVAxis::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    auto value = kv.getValue();
    for(int i = 0; i < value.length(); i++)
    {
        if(value[i] == '[')
        {
            int start = i;

            while(value[i] != ']')
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
            std::from_chars(scale.begin(), scale.end(), flScale);
            this->scaling = flScale;
        }

    }
    return true;
}

bool kvpp::VMF::Plane::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {

    auto split = sourcepp::string::split(kv.getValue(), '(');
    bool offset = false;
    if(split[0].empty())
        offset = true;
    auto split1 = sourcepp::string::split(split[0 + offset],')')[0];
    auto split2 = sourcepp::string::split(split[1 + offset],')')[0];
    auto split3 = sourcepp::string::split(split[2 + offset],')')[0];

    this->bottomLeft = parsePlane(split1);
    this->TopLeft = parsePlane(split2);
    this->TopRight = parsePlane(split3);

    return true;
}

bool
kvpp::VMF::VertexColumn::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    return false;
}

bool
kvpp::VMF::TagColumn::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    return false;
}

bool kvpp::VMF::DecimalColumn::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv,
                                     std::string *backing) {
    return false;
}

bool kvpp::VMF::DisplacementInfo::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv,
                                        std::string *backing) {

    this->power = kv("power").getValue<int>();

    auto startpos = kv("startposition").getValue();
    auto split = sourcepp::string::split(startpos.substr(1, startpos.length() - 1), ' ');
    this->startPosition = {std::stof(split[0]), std::stof(split[1]), std::stof(split[2])};

    this->elevation = kv("elevation").getValue<float>();
    this->isSubdivided = kv("subdiv").getValue<bool>();
    this->flags = kv("flags").getValue<int>();
    this->isSubdivided = kv("subdiv").getValue<bool>();

    for(int i = 0; i < kv("normals").getChildCount(); i++)
    {
        auto& normal = this->normals.emplace_back();
        normal.parse(kv("normals")[i], backing);
    }

    for(int i = 0; i < kv("distances").getChildCount(); i++)
    {
        auto& distance = this->distances.emplace_back();
        distance.parse(kv("distances")[i], backing);
    }

    for(int i = 0; i < kv("offsets").getChildCount(); i++)
    {
        auto& offset = this->offsets.emplace_back();
        offset.parse(kv("offsets")[i], backing);
    }

    for(int i = 0; i < kv("offset_normals").getChildCount(); i++)
    {
        auto& offset = this->offsetNormals.emplace_back();
        offset.parse(kv("offset_normals")[i], backing);
    }

    for(int i = 0; i < kv("alphas").getChildCount(); i++)
    {
        auto& alpha = this->alphas.emplace_back();
        alpha.parse(kv("alphas")[i], backing);
    }

    for(int i = 0; i < kv("triangle_tags").getChildCount(); i++)
    {
        auto& tag = this->triangleTags.emplace_back();
        tag.parse(kv("triangle_tags")[i], backing);
    }

    for(int i = 0; i < kv("allowed_verts").getChildCount(); i++)
    {
        auto &tags = this->allowedTags[std::stoi(std::string(kv("allowed_verts")[i].getKey()))];
        auto splitTags = sourcepp::string::split(kv("allowed_verts").getValue(), ' ');
        for(const auto& tag : splitTags)
            tags.push_back(std::stoi(tag));
    }

    return true;
}

bool kvpp::VMF::Side::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {

    this->id = kv("id").getValue<int>();
    this->plane.parse(kv("plane"), backing);
    this->material = newStringReference(kv("material").getValue(),backing);
    this->uAXis.parse(kv("uaxis"),backing);
    this->vAXis.parse(kv("vaxis"),backing);
    this->rotation = kv("rotation").getValue<float>();
    this->lightmapScale = kv("lightmapscale").getValue<int>();
    this->smoothingGroups = kv("smoothing_groups").getValue<int>();
    this->contents = kv("contents").getValue<int>();
    this->flags = kv("flags").getValue<int>();
    if(kv.hasChild("dispinfo"))
        this->displacementInfo.parse(kv("dispinfo"), backing);

    return true;
}

bool kvpp::VMF::Editor::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    this->color = parseColor(kv("color").getValue());

    if(kv.hasChild("visgroupid"))
        this->visgroupID = kv("visgroupid").getValue<int>();
    if(kv.hasChild("groupid"))
        this->groupID = kv("groupid").getValue<int>();

    this->visgroupShown = kv("visgroupshown").getValue<bool>();
    this->visgroupAutoShown = kv("visgroupautoshown").getValue<bool>();

    if(kv.hasChild("comments"))
        this->comments = newStringReference(kv("comments").getValue(), backing);

    if(kv.hasChild("logicalpos"))
    {
        auto logicposString = kv("logicalpos").getValue();
        auto split = sourcepp::string::split(logicposString.substr(1, logicposString.length() - 1), ' ');
        this->logicalPos = {std::stof(split[0]), std::stof(split[1])};
    }

    return true;
}

bool kvpp::VMF::Solid::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {

    this->id = kv("id").getValue<int>();
    for(int i = 0; i < kv.getChildCount("side"); i++)
    {
        auto& side = this->sides.emplace_back();
        side.parse(kv("side",i),backing);
    }
    this->editor.parse(kv("editor"),backing);
    return true;
}

bool kvpp::VMF::Group::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    this->id = kv("id").getValue<int>();
    this->editor.parse(kv("editor"),backing);
    return true;
}

bool kvpp::VMF::World::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    this->id = kv("id").getValue<int>();
    this->mapVersion = kv("mapversion").getValue<int>(); //mapversion is marked as sz (string) on the wiki, this is probably a mistake.
    this->className = newStringReference(kv("classname").getValue(), backing);
    this->skyName = newStringReference(kv("skyname").getValue(), backing);

    for(int i = 0; i < kv.getChildCount("solid"); i++)
    {
        if(kv("solid").getChildCount() < 1) {

            this->keyvalues["solid"] = newStringReference(kv("solid").getValue(), backing);
            continue;
        }

        auto& solid = this->solids.emplace_back();
        if(!solid.parse( kv("solid",i), backing ))
            return false;
    }


    for(int i = 0; i < kv.getChildCount("group"); i++)
    {
        auto& group = this->groups.emplace_back();
        if(!group.parse( kv("group",i), backing ))
            return false;
    }

    static constexpr std::array<std::string_view, 8> nonKV {"id","classname", "mapversion", "skyname", "solid", "hidden", "origin", "group"};

    for(const auto& keyvalue : kv)
    {
        if(std::find(nonKV.begin(), nonKV.end(),keyvalue.getKey()) != nonKV.end())
            continue;

        this->keyvalues[std::string(keyvalue.getKey())] = newStringReference(keyvalue.getValue(),backing);
    }

    return true;
}

bool
kvpp::VMF::Connection::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {

    this->output = newStringReference(kv.getKey(),backing);

    auto sp = kv.getValue();

    auto splitCon = sourcepp::string::split(kv.getValue(),0x1b);
    if(splitCon.size() == 1) //We didn't find the 0x1b delim, must be non-VScript
        splitCon = sourcepp::string::split(kv.getValue(),',');

    if(splitCon.size() <= 1) //Still didn't find anything, must be corrupt. :/.
        return false;


    if(!splitCon[0].empty())
        this->entity = newStringReference(splitCon[0],backing);
    if(!splitCon[1].empty())
        this->input = newStringReference(splitCon[1],backing);
    if(!splitCon[2].empty())
        this->parameter = newStringReference(splitCon[2],backing);
    if(!splitCon[3].empty())
        std::from_chars(splitCon[3].begin().base(), splitCon[3].end().base(), this->delay);
    if(!splitCon[4].empty())
        std::from_chars(splitCon[4].begin().base(), splitCon[4].end().base(), this->fireCount);


    return true;
}

bool
kvpp::VMF::OverlayData::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    return false;
}

bool kvpp::VMF::Entity::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    this->id = kv("id").getValue<int>();
    this->className = newStringReference(kv("classname").getValue(), backing);
    this->spawnflags = kv("classname").getValue<int>();

    if(kv.hasChild("origin"))
    {
        auto originStr = kv("origin").getValue();
        auto xyz = sourcepp::string::split(originStr,' ');
        std::from_chars(xyz[0].data(), xyz[0].data() + xyz[0].length(), this->origin[0]);
        std::from_chars(xyz[1].data(), xyz[1].data() + xyz[1].length(), this->origin[1]);
        std::from_chars(xyz[2].data(), xyz[2].data() + xyz[2].length(), this->origin[2]);
    }

    for(int i = 0; i < kv.getChildCount("solid"); i++)
    {
        if(kv("solid").getChildCount() < 1) {

            this->keyvalues["solid"] = newStringReference(kv("solid").getValue(), backing);
            break;
        }

        auto& solid = this->solids.emplace_back();
        if(!solid.parse( kv("solid",i), backing ))
            return false;
    }

    for(int i = 0; i < kv.getChildCount("group"); i++)
    {
        if(kv("group").getChildCount() < 1) {

            this->keyvalues["group"] = newStringReference(kv("group").getValue(), backing);
            continue;
        }

        auto& group = this->groups.emplace_back();
        if(!group.parse( kv("group",i), backing ))
            return false;
    }

    for(int i = 0; i < kv.getChildCount("hidden"); i++)
    {

        if(kv("hidden").getChildCount() < 1) {
            this->keyvalues["hidden"] = newStringReference(kv("hidden").getValue(), backing);
            continue;
        }

        for (int j = 0; j < kv("hidden").getChildCount("solid"); j++)
            if(!this->hidden.emplace_back().parse(kv("hidden")("solid", j), backing))
                return false;

    }

    for(int i = 0; i < kv.getChildCount("connections"); i++)
    {
        if(kv("connections").getChildCount() < 1) {
            this->keyvalues["connections"] = newStringReference(kv("connections").getValue(), backing);
            continue;
        }

        const auto& conn = kv("connections")[i];

        if(!this->connections.emplace_back().parse(conn, backing))
            return false;
    }

    static constexpr std::array<std::string_view, 8> nonKV {"id","classname", "spawnflags", "connections", "solid", "hidden", "origin", "editor"};

    for(const auto& keyvalue : kv)
    {
        if(std::find(nonKV.begin(), nonKV.end(),keyvalue.getKey()) != nonKV.end())
            continue;

        this->keyvalues[std::string(keyvalue.getKey())] = newStringReference(keyvalue.getValue(),backing);
    }

    return true;
}

bool kvpp::VMF::Camera::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    return false;
}

bool kvpp::VMF::Cordon::parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing) {
    return false;
}

std::string_view kvpp::VMF::StringReference::getStringView() const {
    if(!valid)
        return "";
    return {this->reference->data() + this->start, this->reference->data() + this->end};
}

std::string kvpp::VMF::StringReference::getString() const {
    if(!valid)
        return "";
    return std::string(this->getStringView());
}