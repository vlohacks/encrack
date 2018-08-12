#ifndef MATCHERSTRING_HPP
#define MATCHERSTRING_HPP

#include "Matcher.hpp"
#include <string>
#include <vector>

class MatcherString : public Matcher
{
private:
    size_t _position;
    bool _search;
    std::vector<char> _string;
    void parseInputString(std::vector<char>& result, const std::string& input);
    
public:
    MatcherString(const MatcherOptions& options);
    ~MatcherString();

    bool match(const unsigned char* pt, const size_t size) const;
};

#endif
