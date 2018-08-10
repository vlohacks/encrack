#ifndef WORDLIST_HPP
#define WORDLIST_HPP

#include <vector>
#include <string>

class Wordlist
{
private:
    std::vector<std::vector<std::string> > _list;
    const size_t _numPools;
    
public:
    Wordlist(const size_t numPools);
    ~Wordlist();
    
    void loadFile(const char * filename);
    const std::vector<std::string>& getList(const size_t pool) const    { return _list[pool]; }
    const size_t getSize(const size_t pool) const                       { return _list[pool].size(); }
    const size_t getSize();
    const size_t getNumPools() const                                    { return _numPools; }
    void addWord(const std::string& item);
};

#endif
