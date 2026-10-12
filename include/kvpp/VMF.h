#pragma once

#include <string_view>
#include <vector>
#include <sourcepp/Math.h>
#include <map>
#include "KV1.h"

namespace kvpp {

    class VMF {
        std::string *backing;
        bool opened;
    public:

        class StringReference {
            bool valid = false;
            size_t start = 0;
            size_t end = 0;
            std::string *reference = nullptr;

        public:
            StringReference() = default;
            StringReference(size_t start, size_t end, std::string *ref) : start(start), end(end), reference(ref), valid(true) {}

            [[nodiscard]] std::string_view getStringView() const;

            [[nodiscard]] std::string getString() const;

            bool operator==(std::string_view str) const
            {
                return str == this->getStringView();
            }

            std::string_view operator()() const
            {
                return this->getStringView();
            }

        };

        struct VersionInfo {
            StringReference editorExtension;
            int editorVersion;
            int editorBuild;
            int mapVersion;
            int formatVersion;
            bool prefab;

            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct VisGroup {
            StringReference name;
            int visgroupID;
            sourcepp::math::Vec3i8 color;
            std::vector<VisGroup> branches;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct ViewSettings {
            bool snapToGrid;
            bool showGrid;
            bool showLogicalGrid;
            int gridSpacing;
            bool show3DGrid;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct UVAxis {
            float x;
            float y;
            float z;
            float degrees;
            float scaling;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Plane {
            sourcepp::math::Vec3f bottomLeft;
            sourcepp::math::Vec3f TopLeft;
            sourcepp::math::Vec3f TopRight;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct VertexColumn {
            sourcepp::math::Vec3f XYZ0;
            sourcepp::math::Vec3f XYZ1;
            sourcepp::math::Vec3f XYZ2;
            sourcepp::math::Vec3f XYZ3;
            sourcepp::math::Vec3f XYZ4;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct TagColumn {
            sourcepp::math::Vec2f XY0;
            sourcepp::math::Vec2f XY1;
            sourcepp::math::Vec2f XY2;
            sourcepp::math::Vec2f XY3;
            sourcepp::math::Vec2f XY4;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct DecimalColumn {
            float d0;
            float d1;
            float d2;
            float d3;
            float d4;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct DisplacementInfo {
            uint8_t power; //If we ever go 255 or below 0, then the laws of physics no longer apply, God help you.
            sourcepp::math::Vec3f startPosition;
            float elevation;
            bool isSubdivided;
            int flags;
            sourcepp::math::Vec3f uAxis;
            sourcepp::math::Vec3f vAxis;
            int mintess;
            int smooth;
            std::vector<VertexColumn> normals;
            std::vector<DecimalColumn> distances;
            std::vector<VertexColumn> offsets;
            std::vector<VertexColumn> offsetNormals;
            std::vector<DecimalColumn> alphas;
            std::vector<TagColumn> triangleTags;
            std::map<int, std::vector<int>> allowedTags;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Side {
            int id;
            Plane plane;
            StringReference material;
            UVAxis uAXis;
            UVAxis vAXis;
            float rotation;
            int lightmapScale;
            int smoothingGroups;
            int contents;
            int flags;
            DisplacementInfo displacementInfo;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Editor {
            sourcepp::math::Vec3i8 color;
            int visgroupID;
            int groupID;
            bool visgroupShown;
            bool visgroupAutoShown;
            StringReference comments;
            sourcepp::math::Vec2f logicalPos;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Solid {
            int id;
            std::vector<Side> sides;
            Editor editor;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Group {
            int id;
            Editor editor;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct BaseEntity {
            int id;
            StringReference className;
            std::map<std::string, StringReference> keyvalues;
            std::vector<Solid> solids;
            std::vector<Group> groups;
            std::vector<Solid> hidden;
        };

        struct World : public BaseEntity {
            StringReference skyName;
            int mapVersion;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Connection {
            StringReference output;
            StringReference entity;
            StringReference input;
            StringReference parameter;
            float delay;
            int fireCount;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct OverlayData {
            StringReference material;
            std::vector<int> sides;
            float startU;
            float endU;
            float startV;
            float endV;
            sourcepp::math::Vec3f basisOrigin;
            sourcepp::math::Vec3f basisU;
            sourcepp::math::Vec3f basisV;
            sourcepp::math::Vec3f basisNormal;
            sourcepp::math::Vec3f uv0;
            sourcepp::math::Vec3f uv1;
            sourcepp::math::Vec3f uv2;
            sourcepp::math::Vec3f uv3;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Entity : public BaseEntity {
            int spawnflags;
            sourcepp::math::Vec3f origin;
            std::vector<Connection> connections;
            std::vector<OverlayData> overlaytransition;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Camera
        {
            sourcepp::math::Vec3f position;
            sourcepp::math::Vec3f look;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };

        struct Cordon
        {
            sourcepp::math::Vec3f mins;
            sourcepp::math::Vec3f maxs;
            bool active;
            StringReference name;
            bool parse(const kvpp::KV1ElementReadable<std::basic_string_view<char>> &kv, std::string *backing);
        };


        VersionInfo versionInfo{};
        World world{};
        ViewSettings viewSettings;

        std::vector<Entity> entities{};
        std::vector<VisGroup> visgroups{};
        std::vector<Solid> hidden{};
        std::pair<int, std::vector<Camera>> cameras{};
        std::pair<bool, std::vector<Cordon>> cordons{};

        VMF();
        explicit VMF(const std::filesystem::path& path);
        explicit VMF(const kvpp::KV1<std::string_view>& kv);

        bool load(const kvpp::KV1<std::string_view>& kv);

    private:
        static StringReference newStringReference(std::string_view str, std::string* backing);
        static sourcepp::math::Vec3i8 parseColor(std::string_view str);
        static sourcepp::math::Vec3f parsePlane(std::string_view str);
    };

}