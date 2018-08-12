#include "MatcherFactory.hpp"

bool MatcherFactory::checkAvailability(const std::string& name)
{
    for (int i = 0; i < numMatchers; i++) {
        if (_availableMatchers[i].name == name)
            return true;
    }
    return false;
}

void MatcherFactory::printMatcherHelp()
{
    for (int i = 0; i < numMatchers; i++)
        printf("%-15s: %s\n", _availableMatchers[i].name, _availableMatchers[i].description);
}

void MatcherFactory::setDefaultValues(const std::string& name, MatcherOptions& options)
{
    if (name == "firstascii") {
        options.setDefaultValue("numBytes", "32");
    } else if (name == "libmagic") { 
        options.setDefaultValue("matchType", "text/plain");
    } else if (name == "string") { 
        options.setDefaultValue("string", "");
    }
}

Matcher* MatcherFactory::getMatcherByName(const std::string& name, const MatcherOptions& options) 
{
    Matcher* matcher = nullptr;
    if (name == "firstascii")   {
        matcher = new MatcherFirstAscii(options); 
    } else if (name == "libmagic") { 
        matcher = new MatcherLibMagic(options); 
    } else if (name == "string") {
        matcher = new MatcherString(options);
    }
    return matcher;
}
