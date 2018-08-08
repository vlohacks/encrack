#ifndef SUBJECT_HPP
#define SUBJECT_HPP

#include <vector>
#include <openssl/evp.h>

class Subject
{
private:
    unsigned char*  _cipherText;
    unsigned char   _salt[PKCS5_SALT_LEN];
    size_t          _cipherTextLength;

public:
    Subject();
    ~Subject();
    
    void loadFile(const char* filename);
    const unsigned char* getCipherText() const  { return _cipherText; }
    const unsigned char* getSalt() const        { return _salt; }
    const size_t getSaltLength() const          { return PKCS5_SALT_LEN; }
    const size_t getCipherTextLength() const    { return _cipherTextLength; }
};

#endif
