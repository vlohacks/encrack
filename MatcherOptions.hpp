#ifndef MATCHEROPTIONS_HPP
#define MATCHEROPTIONS_HPP

#include <map>
#include <string>

class MatcherOptions
{
private:    
    std::map<std::string, std::string> _options;

public:
    void addOption(const std::string& key, const std::string& value);
    const std::string& getOption(const std::string& key) const;
    void setDefaultValue(const std::string& key, const std::string& value);
};

#endif

