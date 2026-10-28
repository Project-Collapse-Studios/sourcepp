#include <sourcepp/vmfbsp/BrushFace.h>

BrushFace::BrushFace(const KeyValues& kv) : Material(kv) {
    this->id = kv["id"].getValue<int>();

    // Plane
    mathpp::Point p1, p2, p3;
    using namespace sourcepp::string;
    const auto splits = split(kv["plane"].getValue(), ' ');
    if (splits.size() < 9) {
        // If more, it will get ignored
        throw std::exception("Cannot create BrushFace with fewer than 3 points!");
    }
    toFloatOmitInvalid(splits[0], p1[0]);
    toFloatOmitInvalid(splits[1], p1[1]);
    toFloatOmitInvalid(splits[2], p1[2]);
    toFloatOmitInvalid(splits[3], p2[0]);
    toFloatOmitInvalid(splits[4], p2[1]);
    toFloatOmitInvalid(splits[5], p2[2]);
    toFloatOmitInvalid(splits[6], p3[0]);
    toFloatOmitInvalid(splits[7], p3[1]);
    toFloatOmitInvalid(splits[8], p3[2]);
    this->Plane_From3Points(p1, p2, p3);
}
