#pragma once
#include <cstdint>
#include <vector>
/// This is a class that has an endless supply of an object stored in heap.
/// It's main usage is to optimize heap allocations, by using the same trick used by the STD:
/// Allocating more and more (2^x) elements each time the internal buffer empties.
template<typename T>
class EndlessWell {

    using size_t = std::size_t;

    uint8_t current_factor = 1;
    size_t array_size = 1;
    T* arr = nullptr;
    std::vector<T*> blocks;

    static constexpr auto MAX_FACTOR = 32;

    void create_more() {

        ++current_factor;

        if (current_factor >= MAX_FACTOR) {
            current_factor = MAX_FACTOR; // Limit the maximum we can allocate
        }

        size_t amount = 2;
        for (uint8_t i = 0; i < this->current_factor; ++i) {
            amount *= 2;
        }

        this->arr = new T[amount];
        this->blocks.push_back(this->arr);
        this->array_size = amount;
    }

public:
    EndlessWell() {
        this->arr = new T[1];
    }

    ~EndlessWell() {
        // Delete the elements
        for (auto& elem : blocks) {
            delete [] elem;
        }
    }

    /// Retrieve the element.
    /// Please note that the EndlessWell claims ownership of the objects it has created.
    /// If you destruct the EndlessWell, all the objects created by it will be deleted.
    T* get_element() {
        /// We ran out of elements
        if (array_size == 0) {
            this->create_more();
        }

        T* elem = &this->arr[array_size - 1];
        --array_size;

        return elem;
    }
};