#include "CipherSuggester.hpp"

#include <cstring>
#include <vector>
#include <string>

void CipherSuggester::doallCallback(const OBJ_NAME* name, void* arg)
{
    if (islower(static_cast<unsigned char>(*name->name)))
        static_cast<std::vector<std::string>*>(arg)->push_back(name->name);
}

void CipherSuggester::suggest(const Subject& subject, Wordlist& wordlist)
{
    const EVP_CIPHER* cipher;
    std::vector<std::string> allCiphers;
    
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL);
    OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH, CipherSuggester::doallCallback, &allCiphers); 

    /*
    for(auto it = ciphers->begin(); it != ciphers->end(); it++) {
        cipher = EVP_get_cipherbyname(it->c_str());
        // remove unsuitable ciphers
        if ((EVP_CIPHER_flags(cipher) & EVP_CIPH_FLAG_AEAD_CIPHER) != 0 || (EVP_CIPHER_mode(cipher) == EVP_CIPH_XTS_MODE))
            ciphers.remove
    }
    */

    for(auto it : allCiphers) {
        printf("%s\n", it.c_str());
    }
}
