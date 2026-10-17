#pragma once
#include <string>


namespace sourcepp::string {

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

        explicit operator std::string() const {
            return this->getString();
        }

        explicit operator std::string_view() const {
            return this->getStringView();
        }

        bool operator==(std::string_view str) const {
            if (!this->valid)
                return false;
            return str == this->getStringView();
        }

        bool operator==(StringReference ref) {
            if (!this->valid || !ref.valid)
                return false;
            return ref.end == this->end && ref.start == this->start && ref.reference == this->reference;
        }

        std::string_view operator()() const {
            if (!this->valid)
                return {};
            return this->getStringView();
        }

        std::string_view operator()(size_t s) const {
            if (!this->valid)
                return {};
            return this->getStringView().substr(s);
        }

        std::string_view operator()(size_t s, size_t n) const {
            if (!this->valid)
                return {};
            return this->getStringView().substr(s, n);
        }

        char operator[](size_t p) const {
            if (!this->valid)
                return {};
            return this->reference->at(p);
        }

        static StringReference newStringReference(std::string_view str, std::string *backing);

    };
};