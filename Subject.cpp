#include "Subject.hpp"

#include <cstdio>
#include <cstring>

Subject::Subject()
    : _cipherText(nullptr)
    , _decryptLimit(0)
{}

Subject::~Subject()
{
    if (_cipherText != nullptr)
        delete[] _cipherText;
}

void Subject::loadFile(const char* filename)
{
    const char magic[] = "Salted__";
    char tmp[strlen(magic)];

    FILE * f = fopen(filename, "rb");

    if (_cipherText != nullptr)
        return;

    if (f == NULL)
        return;

    fseek(f, 0, SEEK_END);
    _cipherTextLength = ftell(f) - sizeof(tmp) - PKCS5_SALT_LEN;
    fseek(f, 0, SEEK_SET);

    fread(tmp, strlen(magic), 1, f);
    if (memcmp(tmp, magic, sizeof(tmp))) {
        fprintf(stderr, "Error: bad magic: %s\n", tmp);
        fclose(f);
        return;
    }
    fread(_salt, PKCS5_SALT_LEN, 1, f);

    _cipherText = new unsigned char[_cipherTextLength];
    fread(_cipherText, _cipherTextLength, 1, f);
    fclose(f);
}


