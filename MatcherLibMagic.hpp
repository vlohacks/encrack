#ifndef MATCHERLIBMAGIC_HPP
#define MATCHERLIBMAGIC_HPP

#include "Matcher.hpp"
#include <string>
#include <magic.h>

class MatcherLibMagic : public Matcher
{
private:
	std::string _matchType;
	magic_t _magicCookie = NULL;

public:
	MatcherLibMagic(const MatcherOptions& options);
	~MatcherLibMagic();
	bool match(const unsigned char* pt, size_t size) const;
};

#endif
