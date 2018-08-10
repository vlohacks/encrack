#include "CipherSuggester.hpp"

#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

void CipherSuggester::doallCallback(const OBJ_NAME* name, void* arg)
{
    const EVP_CIPHER* cipher;
    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();;
    
    arg_t* args = reinterpret_cast<arg_t*>(arg);

    if (!islower(static_cast<unsigned char>(*name->name)))
        return;
        
    cipher = EVP_get_cipherbyname(name->name);

    // filter ciphers which are not supported by openssl enc
    if (cipher == nullptr || ((EVP_CIPHER_flags(cipher) & EVP_CIPH_FLAG_AEAD_CIPHER) != 0) || ((EVP_CIPHER_mode(cipher) == EVP_CIPH_XTS_MODE)))
        return;
        
    // filter ciphers where the ciphertext length is not a multiple of the block size
    if ((args->subject->getCipherTextLength() % EVP_CIPHER_block_size(cipher)) != 0)
        return;
        
    // filter ciphers for which no decryption can be initialized
    if (!EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv))
        return;
    
    // if cipher passed all tests, add it to the wordlist
    args->ciphers->push_back(name->name);
}

void CipherSuggester::suggest(const Subject& subject, Wordlist& wordlist)
{
    std::vector<std::string> ciphers;
    arg_t args = { .ciphers = &ciphers, .subject = &subject };
    
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS, NULL);
    OBJ_NAME_do_all(OBJ_NAME_TYPE_CIPHER_METH, CipherSuggester::doallCallback, reinterpret_cast<void*>(&args));
    
    // sort suitable ciphers by their blocksize, descending
    std::sort(ciphers.begin(), ciphers.end(),
        [](const std::string& a, const std::string& b) -> bool { 
            const EVP_CIPHER* cipherA = EVP_get_cipherbyname(a.c_str());
            const EVP_CIPHER* cipherB = EVP_get_cipherbyname(b.c_str());
            if (EVP_CIPHER_block_size(cipherA) == EVP_CIPHER_block_size(cipherB))
                return a[0] < b[0];
            else
                return EVP_CIPHER_block_size(cipherA) > EVP_CIPHER_block_size(cipherB);
        }
    );
    
    for (const auto& it : ciphers)
        wordlist.addWord(it);
}
