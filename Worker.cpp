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
	unsigned char * pt;
	unsigned char key[EVP_MAX_KEY_LENGTH];
	unsigned char iv[EVP_MAX_IV_LENGTH];
	int outlen;
	const EVP_CIPHER* cipher = NULL;
	const EVP_MD *digest = NULL;
	EVP_CIPHER_CTX* ctx;
	size_t pt_length;

	digest = EVP_sha256();

	char * passwd;

	Matcher* matcher = MatcherFactory::getMatcherByName(_matcherName, _matcherOptions);

	pt = new unsigned char[_subject.getCipherTextLength()];
	ctx = EVP_CIPHER_CTX_new();

	printf("[%02d]: starting...\n", _threadId);

  	for (auto cipherIt = _cipherList.getList().begin(); cipherIt != _cipherList.getList().end(); cipherIt++) {
		cipher = EVP_get_cipherbyname(*cipherIt);
		if (cipher == NULL) {
			fprintf(stderr, "[%02d]: warning: invalid cipher: '%s'\n", _threadId, *cipherIt);
			continue;
		}
		//iv_size = EVP_CIPHER_iv_length(cipher);
		//k_size = EVP_CIPHER_key_length(cipher);

		for (auto passwdIt = _passwordList.getList().begin() + _threadId; passwdIt < _passwordList.getList().end(); passwdIt+= _stride) {
			passwd = *passwdIt;

			/*
			if (!PKCS5_PBKDF2_HMAC(passwd, strlen(passwd), salt, sizeof(salt), iter, digest, k_size+iv_size, tmpkeyiv)) {
				printf("oh no pbkdf2\n");
				exit(1);
			}
       			memcpy(key, tmpkeyiv, k_size);
			memcpy(iv, tmpkeyiv+k_size, iv_size);*/

			if (!EVP_BytesToKey(cipher, digest, _subject.getSalt(), (unsigned char *)passwd, strlen(passwd), 1, key, iv)) {
               			printf("EVP_BytesToKey failed\n");
	       			exit(1);
			}

			EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv);

			if(!EVP_DecryptUpdate(ctx, pt, &outlen, _subject.getCipherText(), _subject.getCipherTextLength())) 
				continue;

			pt_length = outlen;
					
			if(!EVP_DecryptFinal_ex(ctx, pt+outlen, &outlen)) 
				continue;

			pt_length += outlen;

			if (matcher->match(pt, pt_length)) {
				pkcsUnpad(pt, _subject.getCipherTextLength());
				printf("[%02d]: found candidate!\npassword : %s\ncipher   : %s\nplaintext: %s\n", _threadId, passwd, *cipherIt, pt);
			}
		}

	}

	EVP_CIPHER_CTX_free(ctx);
	printf("[%02d]: finishing...\n", _threadId);
	delete[] pt;
	delete matcher;
}