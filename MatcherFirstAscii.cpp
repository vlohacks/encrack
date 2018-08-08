#include "MatcherFirstAscii.hpp"

MatcherFirstAscii::MatcherFirstAscii(const MatcherOptions& options)
	: Matcher(options)
{
	_numBytes = std::stoi(_options.getOption("numBytes"));
}

MatcherFirstAscii::~MatcherFirstAscii()
{}

bool MatcherFirstAscii::match(const unsigned char * pt, size_t size) const
{
	size_t i;

	for (i = 0; i < _numBytes; i++) {
		if (pt[i] > 127)
			return false;
	}

	return true;
}
