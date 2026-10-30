#include <sourcepp/StringReference.h>

using namespace stringpp;

string_reference::operator std::string() const {
    if (this->our_representation->len == 0 || this->our_representation->manager == nullptr) {
        return "";
    }
    return this->our_representation->manager->buffer->substr(this->our_representation->start, this->our_representation->len);
}

std::string::const_iterator string_reference::begin() const {
    return this->our_representation->manager->buffer->begin() + this->our_representation->start;
}

string_reference& string_reference::operator=(const std::string& other) {
    if (this->our_representation == nullptr) {
        throw string_reference_manager_exception();
    }
    const auto& ref = this->our_representation->manager->request(other);
    this->operator=(ref);
    return *this;
}

string_reference& string_reference::operator=(const std::string_view& other) {
    if (this->our_representation == nullptr) {
        throw string_reference_manager_exception();
    }
    const auto& ref = this->our_representation->manager->request(other);
    this->operator=(ref);
    return *this;
}

string_reference& string_reference::operator=(const char* other) {
    if (this->our_representation == nullptr) {
        throw string_reference_manager_exception();
    }
    const auto& ref = this->our_representation->manager->request(other);
    this->operator=(ref);
    return *this;
}

std::string string_reference::substr(const size_t start, size_t length) const {
    length = std::max<size_t>(length, this->our_representation->len);
    return this->our_representation->manager->buffer->substr(this->our_representation->start + start, length);
}

std::string string_reference::getString() const {
    if (!this->our_representation || this->our_representation->len == 0 || this->our_representation->manager == nullptr) {
        return "";
    }
    return this->our_representation->manager->buffer->substr(this->our_representation->start, this->our_representation->len);
}

std::size_t string_reference_manager::str_ref_hash::operator()(const string_reference::str_ref_repr* ref) const noexcept {
    // FNV-1a
    size_t hash = 14695981039346656037ull; // FNV offset basis
    constexpr size_t prime = 1099511628211ull;
    for (size_t i = ref->start; i < ref->len + ref->start; ++i) {
        hash ^= static_cast<std::size_t>(ref->manager->buffer->at(i));
        hash *= prime;
    }
    return hash;
}


std::size_t string_reference_manager::str_ref_hash::operator()(const std::string& ref) const noexcept {
    // FNV-1a
    size_t hash = 14695981039346656037ull; // FNV offset basis
    constexpr size_t prime = 1099511628211ull;
    for (char a : ref) {
        hash ^= static_cast<std::size_t>(a);
        hash *= prime;
    }
    return hash;
}

std::size_t string_reference_manager::str_ref_hash::operator()(const std::string_view ref) const noexcept {
    // FNV-1a
    size_t hash = 14695981039346656037ull; // FNV offset basis
    constexpr size_t prime = 1099511628211ull;
    for (char a : ref) {
        hash ^= static_cast<std::size_t>(a);
        hash *= prime;
    }
    return hash;
}

std::size_t string_reference_manager::str_ref_hash::operator()(const char* ref) const noexcept {
    // FNV-1a
    size_t hash = 14695981039346656037ull; // FNV offset basis
    constexpr size_t prime = 1099511628211ull;
    for (size_t i = 0; ref[i] != '\0'; ++i) {
        hash ^= static_cast<std::size_t>(ref[i]);
        hash *= prime;
    }
    return hash;
}
