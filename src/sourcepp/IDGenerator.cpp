#include <sourcepp/IDGenerator.h>

uint64_t IDGenerator::getNextID()
{
    for(uint64_t i = 0; i < std::numeric_limits<uint64_t>::max(); i++)
    {
        if(std::find(this->ids.begin(), this->ids.end(), i) == this->ids.end())
        {
            this->ids.push_back(i);
            return i;
        }
    }

    throw "Maximum numbers of IDs reached : How did we get here?";
}

uint64_t IDGenerator::getNextID(uint64_t preferred)
{
    if(this->allowDuplicates || std::find(this->ids.begin(), this->ids.end(), preferred) != this->ids.end()) {
        this->ids.push_back(preferred);
        return preferred;
    }

    return this->getNextID();
}

bool IDGenerator::registerID(uint64_t id)
{
    if(this->allowDuplicates || std::find(this->ids.begin(), this->ids.end(), id) != this->ids.end())
        return false;

    this->ids.push_back(id);
    return true;
}

bool IDGenerator::unregisterID(uint64_t id)
{
    auto registered = std::find(this->ids.begin(), this->ids.end(), id);

    if(registered == this->ids.end())
        return false;

    this->ids.erase(registered);
    return true;
}

bool IDGenerator::hasID(uint64_t id)
{
    return std::find(this->ids.begin(), this->ids.end(), id) != this->ids.end();
}
