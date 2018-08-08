#ifndef MATCHERFACTORY_HPP
#define MATCHERFACTORY_HPP

#include <string>
#include <array>

#include "MatcherOptions.hpp"
#include "MatcherFirstAscii.hpp"
#include "MatcherLibMagic.hpp"

class MatcherFactory 
{
private:
    
    typedef struct {
        const char* name;
        const char* description;
    } matcher_description_t;
    
    static constexpr int numMatchers = 2;
    static constexpr matcher_description_t _availableMatchers[numMatchers] = {
        { "firstascii", "Matches the first numBytes for being 7 bit clean" },
        { "libmagic", "Uses libmagic to match the cleartext against given mimetype 'matchType'" }
    };
    
public:
    static bool checkAvailability(const std::string& name);
    static void printMatcherHelp();
    static void setDefaultValues(const std::string& name, MatcherOptions& options);
    static Matcher* getMatcherByName(const std::string& name, const MatcherOptions& options);
};

#endif
