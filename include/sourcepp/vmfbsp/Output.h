#pragma once

#include <string>

/// Represents an I/O event between entities configured inside VMF/BSP files.
struct Output {
    std::string_view output_name;
    std::string target;
    std::string_view input_name;
    float delay = 0;
    int num_fires = -1;
};