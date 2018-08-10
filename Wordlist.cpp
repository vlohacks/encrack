#include "Wordlist.hpp"

#include <cstdio>
#include <cstring>
#include <stdexcept>

Wordlist::Wordlist(const size_t numPools)
    : _list(std::vector<std::vector<std::string>>(numPools))
    , _numPools(numPools)
{}

Wordlist::~Wordlist()
{}

void Wordlist::loadFile(const char* filename)
{
    size_t pool = 0;
    size_t length;
    char * tmp;
    char * p;

    FILE* f = fopen(filename, "rb");

    if (f == NULL)
        throw std::runtime_error("could not open file");
    
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    tmp = new char[length + 1];
    tmp[length] = 0;
    fread(tmp, length, 1, f);
    
    p = strtok(tmp, "\r\n");

    while (p != nullptr) {
        _list[pool].push_back(std::string(p));
        pool = (pool + 1) % _numPools;
        p = strtok(nullptr, "\r\n");
    }
    
    delete[] tmp;
    fclose(f);
}

const size_t Wordlist::getSize()
{
    size_t sum = 0;
    
    for (const auto& pool : _list)
        sum += pool.size();
    
    return sum;
}

/* add word to wordlist and make sure to evenly distribute
 * new added words to the pools.
 * it currently does not prefer the smallest pool but this
 * should be ok since the public  interface does not allow 
 * to have a distance > 1 between pool sizes
 */
void Wordlist::addWord(const std::string& item)
{
    size_t lastSize = 0;
    
    // add item to pool which is smaller than it's previous
    for (auto& pool : _list) {
        if (pool.size() < lastSize) {
            pool.push_back(item);
            return;
        }
        lastSize = pool.size();
    }
    
    // if all pools are equal size, add to first pool
    _list.begin()->push_back(item);
}
