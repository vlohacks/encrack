#include "MatcherOptions.hpp"
#include <stdexcept>

void MatcherOptions::addOption(const std::string& key, const std::string& value)
{
    _options[key] = value;
}

const std::string& MatcherOptions::getOption(const std::string& key) const
{
    auto it = _options.find(key);
    if (it != _options.end())
        return it->second;
    else 
        throw std::runtime_error("tried to access empty option");
}

void MatcherOptions::setDefaultValue(const std::string& key, const std::string& value)
{
    auto it = _options.find(key);
    if (it == _options.end()) {
        fprintf(stderr, "[MATCHER] Warning: setting default value of '%s' to value '%s'\n", key.c_str(), value.c_str());
        fprintf(stderr, "[MATCHER] use -o %s=<value> to set user defined value\n", key.c_str());
        addOption(key, value);
    }
}
