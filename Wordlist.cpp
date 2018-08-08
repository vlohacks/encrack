#include "Wordlist.hpp"

#include <cstdio>
#include <cstring>
#include <stdexcept>

Wordlist::Wordlist()
    : _data(nullptr)
{}

Wordlist::~Wordlist()
{
    if (_data != nullptr)
        delete[] _data;
}

void Wordlist::loadFile(const char* filename)
{
    char* p;
    size_t length;

    FILE* f = fopen(filename, "rb");

    if (_data != nullptr)
        throw std::runtime_error("Wordlist already loaded");

    if (f == NULL)
        throw std::runtime_error("could not open file");

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    _data = new char[length+1];
    _data[length] = 0;
    fread(_data, length, 1, f);

    p = strtok(_data, "\r\n");
    while(p != NULL) {
        if (strlen(p) > 0)
            _list.push_back(p);
        p = strtok(NULL, "\r\n");
    }
    fclose(f);
}

