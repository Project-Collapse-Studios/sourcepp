#pragma once
#include <string>
#include <unordered_set>


namespace sourcepp::string {
    class string_reference_manager;

    /// This is just a pointer wrapper
    class string_reference {

        /// A representation of this string_reference that lives inside the set
        class str_ref_repr {
        public:
            std::string* buffer = nullptr; // The buffer we store our data in
            string_reference_manager* manager = nullptr; // The manager we belong to
            size_t start = 0; // The starting character's index belonging to our string
            size_t len = 0; // End char index + 1

            /// Stores the amount of string_reference that reference us.
            size_t reference_count = 0;

            bool operator==(const str_ref_repr& other) const {
                return  this->buffer == other.buffer &&
                        this->start == other.start &&
                        this->len == other.len;
            }

            /// Decreases the reference count, and if it drops to 0 calls to the manager to delete this object.
            void decrease_ref();
        };

        str_ref_repr* our_representation = nullptr;

        explicit string_reference(str_ref_repr* reference) {
            this->our_representation = reference;
            ++this->our_representation->reference_count; // We start to reference it
        };
        ~string_reference() {
            this->our_representation->decrease_ref(); // This can possibly cause a chain reaction that will lead to str_ref_repr being deleted from memory.
        };



        friend string_reference_manager;

        // BEGIN PUBLIC INTERFACE
    public:
        operator std::string() const {
            return this->our_representation->buffer->substr(this->our_representation->start, this->our_representation->len);
        }

        // TODO: Add operator overloads for operator=() and others.
    };


    class string_reference_manager {

        /// Used for hashing
        struct str_ref_hash {
            using is_transparent = std::true_type;

            /// Hashing function, custom made to only read the buffer and not compare the actual object
            std::size_t operator()(const string_reference::str_ref_repr* ref) const noexcept;
        };

        std::unordered_set<string_reference::str_ref_repr*, str_ref_hash> set;
        /// Manager has exclusive ownership of buffer. No operations from outside are allowed.
        std::unique_ptr<std::string> buffer_ptr;

        friend string_reference::str_ref_repr;
        void RemoveRepresentation(string_reference::str_ref_repr* repr) {
            const auto it = this->set.find(repr);
            this->set.erase(it);
            delete repr;
        }

    public:
        /// Create the manager.
        explicit string_reference_manager() {
            this->buffer_ptr = std::make_unique<std::string>();
        }

        /// Create whilst passing a ready to be read buffer.
        explicit string_reference_manager(std::unique_ptr<std::string> buffer) {
            this->buffer_ptr = std::move(buffer);
        }

        /// Request a string reference from us.
        [[nodiscard]] string_reference request(std::string& str);
    };
}