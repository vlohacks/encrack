#ifndef MATCHER_HPP
#define MATCHER_HPP

#include "MatcherOptions.hpp"

class Matcher
{
protected:	
	const MatcherOptions& _options;

public:
	Matcher(const MatcherOptions& options)
		: _options(options)
	{}
	virtual ~Matcher()
	{}
	virtual bool match(const unsigned char* pt, size_t size) const = 0;
};

#endif
