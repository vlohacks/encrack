#ifndef WORDLIST_HPP
#define WORDLIST_HPP

#include <vector>

class Wordlist
{
private:
	char* _data;
	std::vector<char*> _list;

public:
	Wordlist();
	~Wordlist();
	
	void loadFile(const char * filename);
	const char * getEntry(const size_t index) const	{ return _list[index]; }
	const size_t getSize() const					{ return _list.size(); }
	const std::vector<char*>& getList() const 		{ return _list; }
};

#endif
