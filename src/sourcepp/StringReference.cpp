#include <sourcepp/StringReference.h>

using namespace sourcepp::string;

void string_reference::str_ref_repr::decrease_ref() {
    --this->reference_count;
    if (this->reference_count == 0) {
        this->manager->RemoveRepresentation(this);
    }
}

template<string_alike V>
string_reference& string_reference::operator=(const V& other) requires std::convertible_to<V, std::string> {
    const auto& ref = this->our_representation->manager->request(other);
    this->operator=(ref);
    return *this;
}

std::size_t string_reference_manager::str_ref_hash::operator()(const string_reference::str_ref_repr* ref) const noexcept {
    // FNV-1a
    size_t hash = 14695981039346656037ull; // FNV offset basis
    constexpr size_t prime = 1099511628211ull;
    for (size_t i = ref->start; i < ref->len; ++i) {
        hash ^= static_cast<std::size_t>(ref->buffer->at(i));
        hash *= prime;
    }
    return hash;
}

const string_reference& string_reference_manager::request(const std::string& str) {
    auto out = new string_reference::str_ref_repr{};
    out->buffer = &str; // Temporarily set the passed string as the buffer. No writing is done.
    // If the hash turns out the same, this will fail the de-dupe check.
    out->start = 0;
    out->len = str.length();
    out->manager = this;
    const auto it = this->set.find(out);
    if (it == this->set.end()) {
        // New string, we can already point it to our buffer
        out->buffer = this->buffer_ptr.get();

        // 1. Try to find the string in buffer
        const size_t found_idx = this->buffer_ptr->find(str);
        if (found_idx != std::string::npos) {
            out->start = found_idx; // We found the substring, no need to append
        } else {
            out->start = this->buffer_ptr->length(); // We have to append, no substr has been found
            this->buffer_ptr->append(str);
        }

        // 2. Insert our reference representation into the set
        this->set.insert(out);
    } else {
        // String already exists, delete the previous object and grab the one from the set
        delete out;
        out = *it;
    }

    return string_reference{out};
}
