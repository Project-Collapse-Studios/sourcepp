#pragma once

#include "kvpp/kvpp.h"

namespace vmfpp {

	class StringReference {
        bool valid = false;
        size_t start = 0;
        size_t end = 0;
        std::string *reference = nullptr;

    public:
        StringReference() : valid(false) {};
        StringReference(size_t start, size_t end, std::string *ref) : start(start), end(end), reference(ref), valid(true) {}

        [[nodiscard]] std::string_view getStringView() const;

        [[nodiscard]] std::string getString() const;

        bool operator==(std::string_view str) const
        {
            if(!this->valid)
                return false;
            return str == this->getStringView();
        }

        bool operator==(StringReference ref)
        {
            if(!this->valid || !ref.valid)
                return false;
            return ref.end == this->end && ref.start == this->start && ref.reference == this->reference;
        }

        std::string_view operator()() const
        {
            if(!this->valid)
                return {};
            return this->getStringView();
        }

        std::string_view operator()(size_t s) const
        {
            if(!this->valid)
                return {};
            return this->getStringView().substr(s);
        }

        std::string_view operator()(size_t s, size_t n) const
        {
            if(!this->valid)
                return {};
            return this->getStringView().substr(s, n);
        }

        char operator[](size_t p) const
        {
            if(!this->valid)
                return {};
            return this->reference->at(p);
        }

        static StringReference newStringReference(std::string_view str, std::string* backing)
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

    };

    class VMF {
        std::string *backing{};
        bool opened;
    public:

        struct VersionInfo {
            StringReference editorExtension;
            int editorVersion;
            int editorBuild;
            int mapVersion;
            int formatVersion;
            bool prefab;

            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
        };

        struct VisGroup {
            StringReference name;
            int visgroupID;
            mathpp::Vec3i8 color;
            std::vector<VisGroup> branches;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
        };

        struct ViewSettings {
            bool snapToGrid;
            bool showGrid;
            bool showLogicalGrid;
            int gridSpacing;
            bool show3DGrid;
            bool parse(const kvpp::KV1ElementReadable<>& kv);
        };

        struct UVAxis {
            float x;
            float y;
            float z;
            float degrees;
            float scaling;
            bool parse(const kvpp::KV1ElementReadable<>& kv);
        };

        struct Plane {
            mathpp::Vec3f bottomLeft;
            mathpp::Vec3f TopLeft;
            mathpp::Vec3f TopRight;
            bool parse(const kvpp::KV1ElementReadable<>& kv);
        };

        struct VertexColumn {
            mathpp::Vec3f XYZ0;
            mathpp::Vec3f XYZ1;
            mathpp::Vec3f XYZ2;
            mathpp::Vec3f XYZ3;
            mathpp::Vec3f XYZ4;
            bool parse(const kvpp::KV1ElementReadable<>& kv);
        };

        struct TagColumn {
            mathpp::Vec2f XY0;
            mathpp::Vec2f XY1;
            mathpp::Vec2f XY2;
            mathpp::Vec2f XY3;
            mathpp::Vec2f XY4;
            bool parse(const kvpp::KV1ElementReadable<>& kv);
        };

        struct DecimalColumn {
            float d0;
            float d1;
            float d2;
            float d3;
            float d4;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing);
        };

        struct DisplacementInfo {
            uint8_t power; //If we ever go 255 or below 0, then the laws of physics no longer apply, God help you.
            mathpp::Vec3f startPosition;
            float elevation;
            bool isSubdivided;
            int flags;
            mathpp::Vec3f uAxis;
            mathpp::Vec3f vAxis;
            int mintess;
            int smooth;
            std::vector<VertexColumn> normals;
            std::vector<DecimalColumn> distances;
            std::vector<VertexColumn> offsets;
            std::vector<VertexColumn> offsetNormals;
            std::vector<DecimalColumn> alphas;
            std::vector<TagColumn> triangleTags;
            std::map<int, std::vector<int>> allowedTags;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
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
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
        };

        struct Editor {
            mathpp::Vec3i8 color;
            int visgroupID;
            int groupID;
            bool visgroupShown;
            bool visgroupAutoShown;
            StringReference comments;
            mathpp::Vec2f logicalPos;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
        };

        struct Solid {
            int id;
            std::vector<Side> sides;
            Editor editor;
            void parse(const kvpp::KV1ElementReadable<>& kv, std::string* backing_);
        };

        struct Group {
            int id;
            Editor editor;
            void parse(const kvpp::KV1ElementReadable<>& kv, std::string* backing_);
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
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
        };

        struct Connection {
            StringReference output;
            StringReference entity;
            StringReference input;
            StringReference parameter;
            float delay;
            int fireCount;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing_);
        };

        struct OverlayData {
            StringReference material;
            std::vector<int> sides;
            float startU;
            float endU;
            float startV;
            float endV;
            mathpp::Vec3f basisOrigin;
            mathpp::Vec3f basisU;
            mathpp::Vec3f basisV;
            mathpp::Vec3f basisNormal;
            mathpp::Vec3f uv0;
            mathpp::Vec3f uv1;
            mathpp::Vec3f uv2;
            mathpp::Vec3f uv3;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing);
        };

        struct Entity : public BaseEntity {
            int spawnflags;
            mathpp::Vec3f origin;
            std::vector<Connection> connections;
            std::vector<OverlayData> overlaytransition;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing);
        };

        struct Camera
        {
            mathpp::Vec3f position;
            mathpp::Vec3f look;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing);
        };

        struct Cordon
        {
            mathpp::Vec3f mins;
            mathpp::Vec3f maxs;
            bool active;
            StringReference name;
            bool parse(const kvpp::KV1ElementReadable<> &kv, std::string *backing);
        };


        VersionInfo versionInfo{};
        World world{};
        ViewSettings viewSettings{};

        std::vector<Entity> entities{};
        std::vector<VisGroup> visgroups{};
        std::vector<Solid> hidden{};
        std::pair<int, std::vector<Camera>> cameras{};
        std::pair<bool, std::vector<Cordon>> cordons{};

        VMF();
        explicit VMF(const std::filesystem::path& path);
        explicit VMF(const kvpp::KV1<>& kv);

        bool load(const kvpp::KV1<>& kv);

    private:
        static mathpp::Vec3i8 parseColor(std::string_view str);
        static mathpp::Vec3f parsePlane(std::string_view str);
    };

}
