#pragma once

#include <string>
#include "VMFBSP.h"

/// Represents an I/O event between entities configured inside VMF/BSP files.
class Output {
public:
    std::string_view output_name;
    std::string target;
    std::string_view input_name;
    std::string_view parameter;
    float delay = 0;
    int num_fires = -1;

    Output() = default;
    explicit Output(const std::string_view& output_name_, const std::string& target_, const std::string_view& input_name_, const std::string_view& parameter_ = "", const float delay_ = 0, const int num_fires_ = -1):
        output_name(output_name_),
        target(target_),
        input_name(input_name_),
        parameter(parameter_),
        delay(delay_),
        num_fires(num_fires_) {};

    /// VMF format construction
    explicit Output(const KeyValues& kv) {
        this->output_name = kv.getKey();

        auto splits = sourcepp::string::split(kv.getValue(), '');
        if (splits.size() < 5) {
            splits = sourcepp::string::split(kv.getValue(), ','); // Older Source branches used commas for delimiters
        }

        if (splits.size() < 5) {
            throw std::format_error("Cannot parse output: '" + std::string(kv.getValue()) + "'");
        }

        this->target = splits[0];
        this->input_name = splits[1];
        this->parameter = splits[2];
        sourcepp::string::toFloat(splits[3], this->delay);
        sourcepp::string::toInt(splits[4], this->num_fires);
    }
};