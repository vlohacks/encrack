#include "Worker.hpp"
#include "MatcherFactory.hpp"
#include <cstring>

void Worker::run()
{
    _thread = std::thread(&Worker::runner, this);
}

void Worker::pkcsUnpad(unsigned char* pt, size_t size)
{
    size--;
    size_t num = pt[size];
    while(num--)
        pt[size--] = '\0';
}


void Worker::runner()
{
    //size_t k_size, iv_size;
    size_t decryptLength;
    size_t blockSizeDifference;
    unsigned char * pt;
    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    
    const EVP_CIPHER* cipher = NULL;
    const EVP_MD* digest = NULL;
    EVP_CIPHER_CTX* ctx;
    
    int outlen;
    size_t pt_length;

    Matcher* matcher = MatcherFactory::getMatcherByName(_matcherName, _matcherOptions);

    pt = new unsigned char[_subject.getCipherTextLength()];
    digest = EVP_sha256();
    ctx = EVP_CIPHER_CTX_new();

    printf("[%02d]: Starting...\n", _threadId);

    for (auto cipherIt = _cipherList.getList(0).begin(); cipherIt != _cipherList.getList(0).end(); cipherIt++) {
        cipher = EVP_get_cipherbyname(cipherIt->c_str());
        
        if (cipher == NULL) {
            fprintf(stderr, "[%02d]: Warning: Invalid cipher: '%s'\n", _threadId, cipherIt->c_str());
            _stats += _passwordList.getSize(_threadId);
            continue;
        }
        
        decryptLength = _subject.getCipherTextLimit();
        if ((blockSizeDifference = decryptLength % EVP_CIPHER_block_size(cipher)) != 0)
            decryptLength += (EVP_CIPHER_block_size(cipher) - blockSizeDifference);
            
        //iv_size = EVP_CIPHER_iv_length(cipher);
        //k_size = EVP_CIPHER_key_length(cipher);

        for (auto passwdIt = _passwordList.getList(_threadId).begin(); passwdIt != _passwordList.getList(_threadId).end(); passwdIt ++) {
            _stats++;
            /*
            if (!PKCS5_PBKDF2_HMAC(passwd, strlen(passwd), salt, sizeof(salt), iter, digest, k_size+iv_size, tmpkeyiv)) {
                printf("oh no pbkdf2\n");
                exit(1);
            }
                memcpy(key, tmpkeyiv, k_size);
            memcpy(iv, tmpkeyiv+k_size, iv_size);*/

            if (!EVP_BytesToKey(cipher, digest, _subject.getSalt(), reinterpret_cast<const unsigned char*>(passwdIt->c_str()), strlen(passwdIt->c_str()), 1, key, iv)) {
                printf("EVP_BytesToKey failed\n");
                exit(1);
            }

            EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv);

            if(!EVP_DecryptUpdate(ctx, pt, &outlen, _subject.getCipherText(), decryptLength))
                continue;

            pt_length = outlen;
            
            // do final round only if full ciphertext has been decrypted, since it tries to validate padding, it will fail on incomplete decryption.
            if (decryptLength == _subject.getCipherTextLength()) {
                if(!EVP_DecryptFinal_ex(ctx, pt+outlen, &outlen))
                    continue;
                    
                pt_length += outlen;
            } else {
                // the API does not report the full decrypt length since it awaits the final round.
                // so we just set it hardcore to the decrypt length
                pt_length = decryptLength;
            }
            
            if (matcher->match(pt, pt_length)) {
                // unpad only if full ciphertext has been decrypted
                if (decryptLength == _subject.getCipherTextLength())
                    pkcsUnpad(pt, _subject.getCipherTextLength());
                printf("[%02d]: Found candidate!\nPassword : %s\nCipher   : %s\nPlaintext: %s\n", _threadId, passwdIt->c_str(), cipherIt->c_str(), pt);
            }
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    printf("[%02d]: Finishing...\n", _threadId);
    delete[] pt;
    delete matcher;
}
