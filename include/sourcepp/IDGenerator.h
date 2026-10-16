#include <vector>
#include <string>
#include <cstdint>

class IDGenerator
{
    std::vector<uint64_t> ids;
    bool allowDuplicates;

public:
    explicit IDGenerator(bool allowDupes = false): ids({}), allowDuplicates(allowDupes) {};
    explicit IDGenerator(std::vector<uint64_t> ids, bool allowDupes = false): ids(std::move(ids)), allowDuplicates(allowDupes) {};

    uint64_t getNextID();

    uint64_t getNextID(uint64_t preferred);

    bool registerID(uint64_t id);

    bool unregisterID(uint64_t id);

    bool hasID(uint64_t id);

    IDGenerator& operator=(uint64_t reg)
    {
        this->registerID(reg);
        return *this;
    }

    explicit operator uint64_t()
    {
        return this->getNextID();
    }

    uint64_t operator<<(IDGenerator& id)
    {
        return id.getNextID();
    }

    uint64_t operator++()
    {
        return this->getNextID();
    }

    uint64_t operator++(int pref)
    {
        return this->getNextID(pref);
    }

    bool operator==(uint64_t id)
    {
        return this->hasID(id);
    }

};
