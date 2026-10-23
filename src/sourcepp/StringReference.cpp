#include <sourcepp/StringReference.h>
#include <unordered_set>


std::string_view sourcepp::string::StringReference::getStringView() const
{
    if (!this->valid)
        return {};
    return {this->reference->data() + this->start, this->reference->data() + this->end};
}

std::string sourcepp::string::StringReference::getString() const {
    if (!this->valid)
        return {};
    return std::string(this->getStringView());
}

sourcepp::string::StringReference sourcepp::string::StringReference::newStringReference(std::string_view str, std::string* backing, std::unordered_set<StringReference, StringReferenceHash<StringReference>, StringReferenceCompare<StringReference>> set) {


    if(set.contains(str))
    {
        return *set.find(str);
    }

    size_t start = backing->length();
    backing->append(str);
    size_t end = backing->length();
    return {start,end,backing};
}

bool sourcepp::string::StringReference::isValid() const {
    return this->valid;
}
