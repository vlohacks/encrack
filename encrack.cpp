#include <iostream>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <string>
#include <exception>

#include "MatcherFactory.hpp"
#include "MatcherOptions.hpp"
#include "Subject.hpp"
#include "Wordlist.hpp"
#include "Worker.hpp"


void printhex(unsigned char * p, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
		printf("%02X", *p++);

	printf("\n");
}

void usage(char * progname)
{
	printf("Usage: %s -w <word_list> -i <input_file> -c <ciphers_list> -m <matcher_name> [-t <num_threads>] [-o <param=value> -o <param=value> ...] \n", progname);
	printf("    -w <word_list>:     a wordlist containing passwordz\n");
	printf("    -i <input_file>:    the encrypted file\n");
	printf("    -c <ciphers_list>:  list with ciphers to probe for\n");
	printf("    -t <num_threads>:   change number of concurrent threads\n");
	printf("                        (optional, default: 4)\n");
	printf("    -m <matcher_name>:  use the specified matcher module\n");
	printf("    -o <param=value>:   set the matcher parameter param to value value\n");	
	printf("                        (can be used multiple times for multiple values\n");

	exit(1);
}

int main(int argc, char** argv)
{
	int i;

	char * passwords_file = NULL;
	char * ciphers_file = NULL;
	char * input_file = NULL;
	
	Wordlist passwordList;
	Wordlist cipherList;
	Subject subject;
	std::vector<Worker*> workers;

	std::string matcherName;
	MatcherOptions matcherOptions;

	int num_threads = 4;

	while ((i = getopt(argc, argv, "i:w:t:c:m:o:h")) != -1) {
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
			case 'm':
				matcherName = std::string(optarg);
				break;
			case 'o':
				{
					char * opt = optarg;
					char * p = strstr(opt, "=");
					*p = 0;
					char * key = opt;
					char * val = p+1;
					printf("* added matcher option: %s=%s\n", key, val);
					matcherOptions.addOption(key, val);
				}
				break;
			case 'h':

			default:
				usage(argv[0]);
				break;

		}
	}
	
	if (matcherName == "h") {
		MatcherFactory::printMatcherHelp();
		return 1;
	}
	
	if (!MatcherFactory::checkAvailability(matcherName)) {
		fprintf(stderr, "Error: invalid matcher option\nplease specify a valid matcher with the -m option\n");
		fprintf(stderr, "use -mh to get a list of available matchers\n");
		return 1;
	} else {
		MatcherFactory::setDefaultValues(matcherName, matcherOptions);
	}

	try {
		passwordList.loadFile(passwords_file);
		printf("* Loaded %lu password(s)\n", passwordList.getSize());
	} catch (std::exception& e) {
		fprintf(stderr, "! ERROR: Could not load password list: %s\n", passwords_file);
		return 1;
	}

	try {
		cipherList.loadFile(ciphers_file);
		printf("* Loaded %lu cipher(s)\n", cipherList.getSize());
	} catch (std::exception& e) {
		fprintf(stderr, "! ERROR: Could not load ciphers list: %s\n", ciphers_file);
		return 1;
	}

	try {
		subject.loadFile(input_file);
		printf("* Loaded %lu bytes ciphertext\n", subject.getCipherTextLength());
	} catch (std::exception& e) {
		fprintf(stderr, "! ERROR: Could not load input file: %s\n", input_file);
		return 1;
	}
	
	printf("* running %d thread(s)...\n", num_threads);
	for (i = 0; i < num_threads; i++) {
		Worker* worker = new Worker(subject, passwordList, cipherList, i, num_threads, matcherName, matcherOptions);
		workers.push_back(worker);
		worker->run();
	}

	for (i = 0; i < num_threads; i++)
		workers[i]->getThread().join();

	printf("* finished, all threads ended\n");
}


