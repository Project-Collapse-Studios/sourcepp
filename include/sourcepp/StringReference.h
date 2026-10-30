#pragma once
#include <string>
#include <unordered_set>
#include <cstdint>
#include <memory>
#include <sourcepp/String.h>

namespace stringpp {
    class string_reference_manager;

    class string_reference_manager_exception : public std::exception
    {
        [[nodiscard]] const char * what() const noexcept override
        {
            return "Cannot create a new string reference without a valid manager!";
        }
    };
    /// This is just a pointer wrapper
    class string_reference {

        /// A representation of this string_reference that lives inside the set
        class str_ref_repr {
        public:
            string_reference_manager* manager = nullptr; // The manager we belong to
            size_t start = 0; // The starting character's index belonging to our string
            std::uint16_t len = 0; // Length of the string

            /// Stores the amount of string_reference that reference us.
            size_t reference_count = 0;
        };

        str_ref_repr* our_representation = nullptr;

        explicit string_reference(str_ref_repr* reference) {
            this->our_representation = reference;
            ++this->our_representation->reference_count; // We start to reference it
        };
    public:
        string_reference() = default; // Allow for the default constructor, but this won't create a valid string_reference!
        string_reference(const string_reference& other) { // Copy constructor
            this->our_representation = other.our_representation;
            if (this->our_representation != nullptr) {
                ++this->our_representation->reference_count;
            }
        }

        string_reference(string_reference&& other) noexcept {
            this->our_representation = other.our_representation;
        }

        ~string_reference() {
            if (this->our_representation != nullptr) {
                --this->our_representation->reference_count;
            }
        }



        friend string_reference_manager;

        // BEGIN PUBLIC INTERFACE
    public:
        // Conversions
        operator std::string() const;

        operator std::string_view() const
        {
            return {this->begin(), this->end()};
        }

        // Operators

        std::string getString() const;

        /// Assign other string_reference, may change the manager we belong to!
        string_reference& operator=(const string_reference& other) {
            this->our_representation = other.our_representation;
            ++this->our_representation->reference_count;
            return *this;
        }

        /// Assign a string (creates a new string_reference), assigned to the same manager.
        string_reference& operator=(const std::string& other);
        string_reference& operator=(const std::string_view& other);


        std::string::const_iterator begin() const;

        std::string::const_iterator end() const {
            return this->begin() + this->our_representation->len;
        }

        /// Compare two string references, compares only by pointer!
        /// Use str_compare to actually compare the underlaying strings
        bool operator==(const string_reference& other) const {
            return this->our_representation == other.our_representation;
        }

        bool str_compare(const string_reference& other) const {
            return std::string_view{ this->begin(), this->end()}
            == std::string_view { other.begin(), other.end() };
        }

        bool str_compare(const std::string_view other) const {
            return std::string_view{ this->begin(), this->end()}
            == other;
        }

        bool str_compare(const char* other) const {
            return std::string_view{ this->begin(), this->end()}
            == std::string_view { other };
        }

        bool str_compare_caseless(const string_reference& other) const {
            return sourcepp::string::iequals(std::string_view{ this->begin(), this->end()},
            std::string_view { other.begin(), other.end() });
        }

        bool str_compare_caseless(const std::string_view other) const {
            return sourcepp::string::iequals(std::string_view{ this->begin(), this->end()},
            other);
        }

        bool str_compare_caseless(const char* other) const {
            return sourcepp::string::iequals(std::string_view{ this->begin(), this->end()}, other);
        }

        bool starts_with(const std::string_view sv) const noexcept {
            if (sv.length() > this->our_representation->len) { return false; }
            size_t indx = 0;
            for (const auto c : *this) {
                if (c != sv[indx++]) {
                    return false;
                }
            }
            return true;
        }

        bool starts_with(const char c) const noexcept {
            if (this->our_representation->len == 0) {return false;}
            return *this->begin() == c;
        }

        bool starts_with(const char* chararr) const {
            const auto begin = this->begin();
            for (size_t i = 0; chararr[i] != '\0' && i < this->our_representation->len; ++i) {
                if (*(begin + i) != chararr[i]) {return false;}
            }
            return true;
        }

        std::string substr(size_t start = 0, size_t length = std::string::npos) const;

        std::uint16_t length() const {
            return this->our_representation->len;
        }

        bool empty() const {
            return this->our_representation->len == 0;
        }

        friend std::ostream& operator<<(std::ostream& op, const string_reference& obj) {
            for (const char c : obj) {
                op << c;
            }
            return op;
        }

    };

    template<typename V>
    concept StringAlike =   std::same_as<V, std::string> ||
                            std::same_as<V, std::string_view> ||
                            std::same_as<V, string_reference> ||
                           (std::is_array_v<V> && std::same_as<std::remove_extent_t<V>, char>) ||
                            std::same_as<V, char*>;


    class string_reference_manager {

        /// Used for hashing
        struct str_ref_hash {
            using is_transparent = std::true_type;

            /// Hashing function, custom made to only read the buffer and not compare the actual object
            std::size_t operator()(const string_reference::str_ref_repr* ref) const noexcept;
            std::size_t operator()(const std::string& ref) const noexcept;
            std::size_t operator()(std::string_view ref) const noexcept;
            std::size_t operator()(const char* ref) const noexcept;
        };

        struct str_ref_comp
        {
            using is_transparent = std::true_type;

            constexpr bool operator()(const std::string& ref, const string_reference::str_ref_repr* ref2) const
            {
                return std::string_view { ref2->manager->buffer->begin() + ref2->start, ref2->manager->buffer->begin() + ref2->start + ref2->len } == ref;
            };
            constexpr bool operator()(const std::string_view ref, const string_reference::str_ref_repr* ref2) const
            {
                return std::string_view { ref2->manager->buffer->begin() + ref2->start, ref2->manager->buffer->begin() + ref2->start + ref2->len } == ref;
            };
            constexpr bool operator()(const string_reference::str_ref_repr* ref, const string_reference::str_ref_repr* ref2) const
            {
                return std::string_view{ ref->manager->buffer->begin() + ref->start, ref->manager->buffer->begin() + ref->start + ref->len }
                == std::string_view { ref2->manager->buffer->begin() + ref2->start, ref2->manager->buffer->begin() + ref2->start + ref2->len };
            };
            constexpr bool operator()(const char* ref, const string_reference::str_ref_repr* ref2) const {
                return std::string_view{ref} == std::string_view { ref2->manager->buffer->begin() + ref2->start, ref2->manager->buffer->begin() + ref2->start + ref2->len };
            }
        };

        friend string_reference;
        friend string_reference::str_ref_repr;

        std::unordered_set<string_reference::str_ref_repr*, str_ref_hash, str_ref_comp> set;
        /// Manager has exclusive ownership of buffer. No operations from outside are allowed.
        std::unique_ptr<std::string> buffer;

        string_reference::str_ref_repr empty_repr;
        bool caseless = false;

    public:
        /// Create the manager.
        explicit string_reference_manager(bool caseless_ = false) {
            this->buffer = std::make_unique<std::string>();
            this->empty_repr.len = 0;
            this->empty_repr.start = 0;
            this->empty_repr.manager = this;
            this->caseless = caseless_;
        }

        /// Create whilst passing a ready to be read buffer.
        explicit string_reference_manager(std::unique_ptr<std::string> buffer, bool caseless_ = false) {
            this->buffer = std::move(buffer);
            this->empty_repr.len = 0;
            this->empty_repr.start = 0;
            this->empty_repr.manager = this;
            this->caseless = caseless_;
        }

        ~string_reference_manager() {
            for (auto el : this->set) {
                delete el;
            }
        }

        /// Request a string reference from us.
        template<StringAlike V>
        [[nodiscard]] string_reference request(const V& str) {
            if constexpr (std::is_array_v<V>) {
                if (str[0] == '\0') {
                    return string_reference{&this->empty_repr};
                }
            } else {
                if (str.empty()) {
                    return string_reference{&this->empty_repr};
                }
            }
            auto out = new string_reference::str_ref_repr{};
            out->start = 0;
            out->manager = this;

            if constexpr (std::is_array_v<V>) {
                for (size_t i = 0; str[i] != '\0'; ++i) {
                    ++out->len;
                }
            } else {
                out->len = str.length();
            }

            // Our hash and compare set supports all types of V
            std::unordered_set<string_reference::str_ref_repr*, str_ref_hash, str_ref_comp>::const_iterator it;
            if (!this->caseless) {
                it = this->set.find(str);
            } else {
                // Is there a better way to do this?
                std::string A;
                A.reserve(out->len);
                if constexpr (std::is_array_v<V>) {
                    for (size_t i = 0; str[i] != '\0'; ++i) {
                        A += std::tolower(str[i]);
                    }
                } else {
                    for (const auto ch : str) {
                        A += std::tolower(ch);
                    }
                }
                it = this->set.find(A);
            }

            if (it == this->set.end()) {
                out->start = this->buffer->length();
                this->buffer->append(str); // This should automatically handle conversion to a supported type.
                this->set.insert(out);
            } else {
                // String already exists, delete the previous object and grab the one from the set
                delete out;
                out = *it;
            }

            return string_reference{out};
        }

        /// Clears unused references from memory.
        /// This is expensive!
        void ClearUnusedReferences() {
            for (auto it = this->set.begin(); it != this->set.end(); ++it) {
                if ((*it)->reference_count == 0) {
                    this->buffer->erase((*it)->start, (*it)->len);
                    this->set.erase(it);
                }
            }
        }
    };
}