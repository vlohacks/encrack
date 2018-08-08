#include <iostream>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <thread>

void printhex(unsigned char * p, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
		printf("%02X", *p++);

	printf("\n");
}

void worker(const int threadnum, const std::vector<char*>& cipher_list, const std::vector<char*>& password_list, const size_t offset, const size_t increment, const unsigned char* salt, const unsigned char* ct, const size_t ct_length);

char * load_wordlist(std::vector<char*>& list, char* filename)
{
	char* data;
	char* p;
	size_t length;
	
	FILE* f = fopen(filename, "rb");

	if (f == NULL)
		return NULL;
	
	fseek(f, 0, SEEK_END);	
	length = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = new char[length+1];
	data[length] = 0;
	fread(data, length, 1, f);

	p = strtok(data, "\r\n");
	while(p != NULL) {
		if (strlen(p) > 0)
			list.push_back(p);
		p = strtok(NULL, "\r\n");
	}
	fclose(f);
	return data;
}

unsigned char * load_ciphertext(char * filename, unsigned char * salt, size_t * ct_length)
{
	const char magic[] = "Salted__";
	char tmp[strlen(magic)];
	unsigned char * ct;

	FILE * f = fopen(filename, "rb");

	if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	*ct_length = ftell(f) - sizeof(tmp) - PKCS5_SALT_LEN;
	fseek(f, 0, SEEK_SET);

	fread(tmp, strlen(magic), 1, f);
	if (memcmp(tmp, magic, sizeof(tmp))) {
		fprintf(stderr, "Error: bad magic: %s\n", tmp);
		fclose(f);
		return NULL;
	}
	fread(salt, PKCS5_SALT_LEN, 1, f);

	ct = new unsigned char[*ct_length];
	fread(ct, *ct_length, 1, f);
	fclose(f);

	return ct;
}

void usage(char * progname)
{
	printf("Usage: %s -w <word_list> -i <input_file> -c <ciphers_list> [-t <num_threads>]\n", progname);
	printf("	-w <word_list>:		a wordlist containing passwordz\n");
	printf("	-i <input_file>:	the encrypted file\n");
	printf("	-c <ciphers_list>:	list with ciphers to probe for\n");
	printf("	-t <num_threads>:	change number of concurrent threads\n");
	printf("				(optional, default: 4)\n");
	exit(1);
}

int main(int argc, char** argv)
{
	unsigned char salt[PKCS5_SALT_LEN];
	size_t ct_length;
	unsigned char * ct;
	char * password_list_blob;
	char * cipher_list_blob;
	int i;

	char * passwords_file = NULL;
	char * ciphers_file = NULL;
	char * input_file = NULL;
	
	std::vector<char*> password_list;
	std::vector<char*> cipher_list;
	std::vector<std::thread> workers;

	int num_threads = 4;

	while ((i = getopt(argc, argv, "i:w:t:c:h")) != -1) {
		switch(i) {
			case 'i':
				input_file = optarg;
				break;
			case 'w':
				passwords_file = optarg;
				break;
			case 't':
				num_threads = std::atoi(optarg);
				break;
			case 'c':
				ciphers_file = optarg;
				break;
			case 'h':
			default:
				usage(argv[0]);
				break;

		}
	}

	password_list_blob = load_wordlist(password_list, passwords_file);
	if (password_list_blob == NULL) {
		fprintf(stderr, "! ERROR: Could not load password list: %s\n", passwords_file);
		return 1;
	} else {
		printf("* Loaded %lu password(s)\n", password_list.size());
	}

	cipher_list_blob = load_wordlist(cipher_list, ciphers_file);

	if (cipher_list_blob == NULL) {
		fprintf(stderr, "! ERROR: Could not load ciphers list: %s\n", ciphers_file);
		return 1;
	} else {
		printf("* Loaded %lu cipher(s)\n", cipher_list.size());
	}

	ct = load_ciphertext(input_file, salt, &ct_length);
	if (ct == NULL) {
		fprintf(stderr, "! ERROR: Could not load input file: %s\n", input_file);
		return 1;
	} else {
		printf("* Loaded %lu bytes ciphertext\n", ct_length);
	}

	printf("* running %d thread(s)...\n", num_threads);
	for (i = 0; i < num_threads; i++)
		workers.push_back(std::thread(worker, i, cipher_list, password_list, i, num_threads, salt, ct, ct_length));

	for (i = 0; i < num_threads; i++)
		workers[i].join();

	printf("* finished, all threads ended\n");

	delete[] ct;
	delete[] password_list_blob;
	delete[] cipher_list_blob;
}

int validate(unsigned char * pt, size_t max) 
{
	size_t i;
	for (i = 0; i < max; i++) {
		if (pt[i] > 127)
			return 0;
	}
	return 1;
}

void pkcs_unpad(unsigned char * pt, size_t size)
{
	size--;
	size_t num = pt[size];
	while(num--)
		pt[size--] = 0;
}

void worker(const int threadnum, const std::vector<char*>& cipher_list, const std::vector<char*>& password_list, const size_t offset, const size_t increment, const unsigned char* salt, const unsigned char* ct, const size_t ct_length)
{
	//size_t k_size, iv_size;
	unsigned char * pt;
	unsigned char key[EVP_MAX_KEY_LENGTH];
	unsigned char iv[EVP_MAX_IV_LENGTH];
	int outlen;
	const EVP_CIPHER* cipher = NULL;
	const EVP_MD *digest = NULL;
	EVP_CIPHER_CTX* ctx;
	size_t i, j;

	digest = EVP_sha256();

	char * passwd;
	char * ciphername;

	pt = new unsigned char[ct_length];
	ctx = EVP_CIPHER_CTX_new();

	printf("[%02d]: starting...\n", threadnum);

  	for (j = 0; j < cipher_list.size(); j++) {
		ciphername = cipher_list[j];
		cipher = EVP_get_cipherbyname(ciphername);
		if (cipher == NULL) {
			fprintf(stderr, "[%02d]: warning: invalid cipher: '%s'\n", threadnum, ciphername);
			continue;
		}
		//iv_size = EVP_CIPHER_iv_length(cipher);
		//k_size = EVP_CIPHER_key_length(cipher);

		for (i = offset; i < password_list.size(); i += increment) {
			passwd = password_list[i];

			/*
			if (!PKCS5_PBKDF2_HMAC(passwd, strlen(passwd), salt, sizeof(salt), iter, digest, k_size+iv_size, tmpkeyiv)) {
				printf("oh no pbkdf2\n");
				exit(1);
			}
       			memcpy(key, tmpkeyiv, k_size);
			memcpy(iv, tmpkeyiv+k_size, iv_size);*/

			if (!EVP_BytesToKey(cipher, digest, salt, (unsigned char *)passwd, strlen(passwd), 1, key, iv)) {
               			printf("EVP_BytesToKey failed\n");
	       			exit(1);
			}

	        	EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv);

			if(!EVP_DecryptUpdate(ctx, pt, &outlen, ct, ct_length)) 
				continue;
		
			if(!EVP_DecryptFinal_ex(ctx, pt+outlen, &outlen)) 
				continue;


			if (validate(pt, 32)) {
				pkcs_unpad(pt, ct_length);
				printf("[%02d]: found candidate!\npassword : %s\ncipher   : %s\nplaintext: %s\n", threadnum, passwd, ciphername, pt);
			}
		}

	}

	EVP_CIPHER_CTX_free(ctx);
	printf("[%02d]: finishing...\n", threadnum);
	delete[] pt;
}



