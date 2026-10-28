#include <sourcepp/vmfbsp/Brush.h>
#include <unordered_set>

Brush::Brush(const KeyValues& kv) {
    this->id = kv["id"].getValue<int>();

    const unsigned int side_num = kv.getChildCount("side");
    if (side_num > MAX_BRUSH_FACES) {
        throw std::overflow_error("Too many brush faces on brush id: " + std::to_string(this->id));
    }
    if (side_num == 0) {
        throw std::underflow_error("No defined brush faces on brush id: " + std::to_string(this->id));
    }

    for (auto side : kv.getChildren()) {
        if (sourcepp::string::iequals(side.getKey(), "side")) {
            this->sides.emplace_back(side);
        }
    }
}