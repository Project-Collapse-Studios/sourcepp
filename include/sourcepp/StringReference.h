#pragma once
#include <string>
#include <unordered_set>


namespace sourcepp::string {

    template<typename T>
    struct StringReferenceCompare
    {
        using is_transparent = std::true_type;

        bool operator()(const std::string_view& view, const T& ref ) const { return view == ref; };
        bool operator()(const T& ref, const std::string& view ) const { return view == ref; };
        bool operator()(const T& ref, const char* view ) const { return view == ref; };
        bool operator()(const T& ref, const T& ref2 ) const { return ref == ref2; };
    };

    template<typename T>
    struct StringReferenceHash
    {
        using is_transparent = std::true_type;

        std::size_t operator()(const T& ref) const noexcept
        {
            return std::hash<std::string_view>{}(ref.getStringView());
        }

        std::size_t operator()(const char* ref) const noexcept
        {
            return std::hash<const char*>{}(ref);
        }

        std::size_t operator()(const std::string & ref) const noexcept
        {
            return std::hash<std::string>{}(ref);
        }

        std::size_t operator()(const std::string_view & ref) const noexcept
        {
            return std::hash<std::string_view>{}(ref);
        }
    };
    
    class StringReference {
        bool valid = false;
        size_t start = 0;
        size_t end = 0;
        std::string *reference = nullptr;

    public:
        StringReference() : valid(false), reference(nullptr), start(-1), end(-1) {};

        StringReference(size_t start, size_t end, std::string *ref) : start(start), end(end), reference(ref), valid(true) {}

        [[nodiscard]] std::string_view getStringView() const;

        [[nodiscard]] std::string getString() const;

        [[nodiscard]] bool isValid() const;

        explicit operator std::string() const {
            return this->getString();
        }

        explicit operator std::string_view() const {
            return this->getStringView();
        }

        bool operator==(std::string_view str) const {
            if (!this->isValid())
                return false;
            return str == this->getStringView();
        }

        bool operator==(const StringReference& ref) const {
            if (!this->isValid() || !ref.isValid())
                return false;
            return ref.end == this->end && ref.start == this->start && ref.reference == this->reference;
        }

        bool operator==(const char* str) const
        {
            return this->getStringView() == str;
        }

        std::string_view operator()() const {
            if (!this->isValid())
                return {};
            return this->getStringView();
        }

        std::string_view operator()(size_t s) const {
            if (!this->isValid())
                return {};
            return this->getStringView().substr(s);
        }

        std::string_view operator()(size_t s, size_t n) const {
            if (!this->isValid())
                return {};
            return this->getStringView().substr(s, n);
        }

        char operator[](size_t p) const {
            if (!this->isValid())
                return {};
            return this->reference->at(p);
        }

        explicit operator bool() const
        {
           return this->isValid();
        }

        static StringReference newStringReference(std::string_view str, std::string *backing, std::unordered_set<StringReference, StringReferenceHash<StringReference>, StringReferenceCompare<StringReference>> set = std::unordered_set<StringReference, StringReferenceHash<StringReference>, StringReferenceCompare<StringReference>>{});

    };
};