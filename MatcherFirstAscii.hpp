#ifndef MATCHERFIRSTASCII_HPP
#define MATCHERFIRSTASCII_HPP

#include "Matcher.hpp"

class MatcherFirstAscii : public Matcher
{
private:
	size_t _numBytes;
	void initDefaults();

public:
	MatcherFirstAscii(const MatcherOptions& options);
	~MatcherFirstAscii();

	bool match(const unsigned char* pt, size_t size) const;

};

#endif
