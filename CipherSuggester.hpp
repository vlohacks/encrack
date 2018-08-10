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
    
    typedef struct {
        std::vector<std::string>* ciphers;
        const Subject* subject;
    } arg_t;

public: 
    static void suggest(const Subject& subject, Wordlist& wordlist);
};

#endif
