#pragma once
#include <string>
#include <unordered_set>
#include <cstdint>
#include <memory>

namespace stringpp {

    class string_reference_manager;

    /// This is just a pointer wrapper
    class string_reference {

        /// A representation of this string_reference that lives inside the set
        class str_ref_repr {
        public:
            const std::string* buffer = nullptr; // The buffer we store our data in
            string_reference_manager* manager = nullptr; // The manager we belong to
            size_t start = 0; // The starting character's index belonging to our string
            std::uint16_t len = 0; // Length of the string

            /// Stores the amount of string_reference that reference us.
            size_t reference_count = 0;

            //bool operator==(const str_ref_repr& other) const {
            //    return  this->buffer == other.buffer &&
            //            this->start == other.start &&
            //            this->len == other.len;
            //}

            /// Decreases the reference count, and if it drops to 0 calls to the manager to delete this object.
            void decrease_ref();
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
            ++this->our_representation->reference_count;
        }

        string_reference(string_reference&& other) noexcept {
            this->our_representation = other.our_representation;
        }

        ~string_reference() {
            if (this->our_representation != nullptr) {
                this->our_representation->decrease_ref(); // This can possibly cause a chain reaction that will lead to str_ref_repr being deleted from memory.
            }
        }



        friend string_reference_manager;

        // BEGIN PUBLIC INTERFACE
    public:
        // Conversions
        operator std::string() const {
            if (this->our_representation->buffer == nullptr) {
                return "";
            }
            return this->our_representation->buffer->substr(this->our_representation->start, this->our_representation->len);
        }

        operator std::string_view() const
        {
            return {this->begin(), this->end()};
        }

        // Operators

        /// Assign other string_reference, may change the manager we belong to!
        string_reference& operator=(const string_reference& other) {
            this->our_representation = other.our_representation;
            ++this->our_representation->reference_count;
            return *this;
        }

        /// Assign a string (creates a new string_reference), assigned to the same manager.
        string_reference& operator=(const std::string& other);
        string_reference& operator=(const std::string_view& other);


        std::string::const_iterator begin() const {
            return this->our_representation->buffer->begin() + this->our_representation->start;
        }

        std::string::const_iterator end() const {
            return this->begin() + this->our_representation->len;
        }

        bool operator==(const string_reference& other) const {
            return this->our_representation == other.our_representation; // Pointer comparison!
        }

        friend std::ostream& operator<<(std::ostream& op, const string_reference& obj) {
            for (const char c : obj) {
                op << c;
            }
            return op;
        }

    };


    class string_reference_manager {

        /// Used for hashing
        struct str_ref_hash {
            using is_transparent = std::true_type;

            /// Hashing function, custom made to only read the buffer and not compare the actual object
            std::size_t operator()(const string_reference::str_ref_repr* ref) const noexcept;
            std::size_t operator()(const std::string& ref) const noexcept;
            std::size_t operator()(std::string_view ref) const noexcept;
        };

        struct str_ref_comp
        {
            using is_transparent = std::true_type;

            constexpr bool operator()(const std::string& ref, const string_reference::str_ref_repr* ref2) const
            {
                return std::string_view { ref2->buffer->begin() + ref2->start, ref2->buffer->begin() + ref2->start + ref2->len } == ref;
            };
            constexpr bool operator()(const std::string_view ref, const string_reference::str_ref_repr* ref2) const
            {
                return std::string_view { ref2->buffer->begin() + ref2->start, ref2->buffer->begin() + ref2->start + ref2->len } == ref;
            };
            constexpr bool operator()(const string_reference::str_ref_repr* ref, const string_reference::str_ref_repr* ref2) const
            {
                return std::string_view{ ref->buffer->begin() + ref->start, ref->buffer->begin() + ref->start + ref->len }
                == std::string_view { ref2->buffer->begin() + ref2->start, ref2->buffer->begin() + ref2->start + ref2->len };
            };
        };


        std::unordered_set<string_reference::str_ref_repr*, str_ref_hash, str_ref_comp> set;
        /// Manager has exclusive ownership of buffer. No operations from outside are allowed.
        std::unique_ptr<std::string> buffer_ptr;

        friend string_reference::str_ref_repr;
        void RemoveRepresentation(string_reference::str_ref_repr* repr) {
            const auto it = this->set.find(repr);
            this->set.erase(it);
            delete repr;
        }


        string_reference::str_ref_repr empty_repr;

    public:
        /// Create the manager.
        explicit string_reference_manager() {
            this->buffer_ptr = std::make_unique<std::string>();
            this->empty_repr.buffer = this->buffer_ptr.get();
            this->empty_repr.len = 0;
            this->empty_repr.start = 0;
            this->empty_repr.manager = this;
        }

        /// Create whilst passing a ready to be read buffer.
        explicit string_reference_manager(std::unique_ptr<std::string> buffer) {
            this->buffer_ptr = std::move(buffer);
            this->empty_repr.buffer = this->buffer_ptr.get();
            this->empty_repr.len = 0;
            this->empty_repr.start = 0;
            this->empty_repr.manager = this;
        }

        /// Request a string reference from us.
        [[nodiscard]] string_reference request(const std::string &str);
    };
}