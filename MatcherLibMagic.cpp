#include "MatcherLibMagic.hpp"

MatcherLibMagic::MatcherLibMagic(const MatcherOptions& options)
                : Matcher(options)
{
    _matchType = options.getOption("matchType");
    _magicCookie = magic_open(MAGIC_MIME);
    if (_magicCookie == NULL) {
        printf("scheisse1");
                exit(1);
    }
    if (magic_load(_magicCookie, NULL) != 0) {
        printf("scheisse2");
        exit(1);
    }               // TODO handle errors
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
