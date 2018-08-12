#include "MatcherString.hpp"

#include <exception>
#include <cstring>
#include <algorithm>

MatcherString::MatcherString(const MatcherOptions& options)
    : Matcher(options)
    , _position(0)
{
    _search = false;
    try {
        _position = std::stoi(_options.getOption("position"));
    } catch (std::exception& ex) {
        // no valid position: search for string in whole payload
        _search = true;
    }
    parseInputString(_string, _options.getOption("string"));
}

MatcherString::~MatcherString()
{}

void MatcherString::parseInputString(std::vector<char>& result, const std::string& input)
{
    size_t i;
    unsigned int tmp;
    
    for (i = 0; i < input.size(); i++) {
        if (input[i] == '\\') {
            i++;
            if (i == input.size())
                break;
            switch (input[i]) {
                case '\\':
                result.push_back('\\');
                break;
                
                case 'x':
                i++;
                if (i == input.size())
                    break;
                sscanf(&input[i], "%02x", &tmp);
                i++;
                if (i == input.size())
                    break;                
                result.push_back(static_cast<char>(tmp));
                break;
            }
        } else {
            result.push_back(input[i]);
        }
    }
}

bool MatcherString::match(const unsigned char* pt, const size_t size) const
{
    const std::vector<char> subject(pt, pt+size);
    bool found = false;
    size_t i;
        
    for (auto itSub = subject.begin() + _position; itSub != subject.end() - _position; itSub++) {
        found = true;
        for (i = 0; i < _string.size(); i++) {
            if (_string[i] != itSub[i]) {
                found = false;
                break;
            }
        }
        if (!_search || found)
            break;
    }
    
    return found;
}
