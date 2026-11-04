#include "sourcepp/vmfbsp/DisplacementInfo.h"



DisplacementInfo::DisplacementInfo(const KeyValues& kv)
{
    this->power = kv("power");
    this->start_position = kv("startposition");
    this->subdiv = kv("subdiv");
    this->elevation = kv("elevation");
    this->flags = kv("flags");

    // Parse vertex data

    size_t num_rows = pow(2, this->power) + 1;

    // "Matrices" of size num_rows, num_rows
    this->normals.reserve(num_rows);
    this->distances.reserve(num_rows);
    this->offsets.reserve(num_rows);
    this->offset_normals.reserve(num_rows);
    this->alphas.reserve(num_rows);


    // DEFINE HELPER LAMBDAS
    auto convertVecRow = [&num_rows](const KeyValues& data, std::vector<std::vector<mathpp::Vec3f>>& out) -> void {
        for (size_t i = 0; i < num_rows; ++i) {
            const auto& row = data[i].getValue();
            auto splits = sourcepp::string::split(row, ' ');

            // Convert to vectors
            if (splits.size() < num_rows * 3) {
                throw std::underflow_error{"Bad row (too few elements) in disp_info!"};
            }

            std::vector<mathpp::Vec3f> arr;
            arr.reserve(num_rows);
            for (size_t j = 0; j < num_rows * 3; j += 3) {
                mathpp::Vec3f vec;

                sourcepp::string::toArithmetic<float>(splits[j], vec[0]);
                sourcepp::string::toArithmetic<float>(splits[j + 1], vec[1]);
                sourcepp::string::toArithmetic<float>(splits[j + 2], vec[2]);

                arr.push_back(vec);
            }

            out.push_back(arr);
        }
    };

    auto convertFloatRow = [&num_rows](const KeyValues& data, std::vector<std::vector<float>>& out) -> void {
        for (size_t i = 0; i < num_rows; ++i) {
            const auto& row = data[i].getValue();
            auto splits = sourcepp::string::split(row, ' ');

            if (splits.size() < num_rows) {
                throw std::underflow_error{"Bad row (too few elements) in disp_info!"};
            }

            std::vector<float> arr;
            arr.reserve(num_rows);
            for (size_t j = 0; j < num_rows; ++j) {
                float val;
                sourcepp::string::toArithmetic<float>(splits[j], val);
                arr.push_back(val);
            }

            out.push_back(arr);
        }
    };
    // END


    convertVecRow(kv("normals"), this->normals);
    convertFloatRow(kv("distances"), this->distances);
    convertVecRow(kv("offsets"), this->offsets);
    convertVecRow(kv("offset_normals"), this->offset_normals);
    convertFloatRow(kv("alphas"), this->alphas);



    // Triangle tags

    const KeyValues& data = kv("triangle_tags");

    --num_rows;

    for (size_t i = 0; i < num_rows; ++i) {
        const auto& row = data[i].getValue();
        auto splits = sourcepp::string::split(row, ' ');

        if (splits.size() < num_rows * 2) {
            throw std::underflow_error{"Bad row (too few elements) in disp_info!"};
        }

        for (size_t j = 0; j < num_rows * 2; j += 2) {
            uint8_t a, b;
            sourcepp::string::toArithmetic<uint8_t>(splits[j], a);
            sourcepp::string::toArithmetic<uint8_t>(splits[j + 1], b);
            std::array< TriangleTags, 2 > arr{static_cast<TriangleTags>(a), static_cast<TriangleTags>(b)};
            this->triangle_tags.push_back(arr);
        }
    }



    // Allowed verts

    const KeyValues& allowed_verts_data = kv("allowed_verts")[0];
    size_t allowed_verts_num = 0;
    sourcepp::string::toArithmetic<size_t>(allowed_verts_data.getKey(), allowed_verts_num);
    const auto splits = sourcepp::string::split(allowed_verts_data.getValue(), ' ');
    if (splits.size() < allowed_verts_num) {
        throw std::underflow_error{"Bad allowed verts row, amount doesn't match key!"};
    }

    for (size_t i = 0; i < allowed_verts_num; ++i) {
        size_t vert_id = 0; // Is it actually an id?
        sourcepp::string::toArithmetic<size_t>(splits[i], vert_id);
        this->allowed_verts.push_back(vert_id);
    }
}
