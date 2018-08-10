#ifndef CIPHERSUGGESTER_HPP
#define CIPEHRSUGGESTER_HPP

#include "Subject.hpp"
#include "Wordlist.hpp"

#include <openssl/objects.h>
#include <openssl/evp.h>

class CipherSuggester 
{
private:
    static void doallCallback(const OBJ_NAME* name, void* arg);

public: 
    static void suggest(const Subject& subject, Wordlist& wordlist);
};

#endif
