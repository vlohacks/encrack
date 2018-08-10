#include "MatcherLibMagic.hpp"

#include <stdexcept>

MatcherLibMagic::MatcherLibMagic(const MatcherOptions& options)
    : Matcher(options)
{
    _matchType = options.getOption("matchType");
    _magicCookie = magic_open(MAGIC_MIME);
    
    if (_magicCookie == NULL)
        throw std::runtime_error("Error while initializing libmagic");
   
    if (magic_load(_magicCookie, NULL) != 0)
        throw std::runtime_error("Error while loading default magic database");

}

MatcherLibMagic::~MatcherLibMagic()
{ 
    if (_magicCookie != NULL) 
        magic_close(_magicCookie); 
}
    

bool MatcherLibMagic::match(const unsigned char * pt, size_t size) const
{
    const char * magicStr;

    magicStr = magic_buffer(_magicCookie, pt, size);
    if (magicStr != NULL) {
        if (std::string(magicStr).substr(0, _matchType.size()) == _matchType) {
            printf("%s\n", magicStr);
            return true;
        }
    }

    return false;
}
