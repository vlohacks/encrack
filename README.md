# encrack
tool for cracking openssl enc style encrypted files. Since there is no hash of the password or something like that stored in the file, successful decryption must be determined from plaintext directly.

## Features
### Matching modules
The program allows to add easily different matching modules. Currently 2 modules are supported which most likely cover most usecases:
* firstascii: checks the first numBytes being ascii 7 bit
* libmagic:	uses libmagic to match data against mime type matchType.
	(note: matchType can be a substring. text/plain also includes text/plain; charset: utf-16)
* string: searches for a specific 'string' either on a specific 'position' or in the complete decrypted payload
### Cipher suggester
If no cipher is known the program autogenerates a suitable list of ciphers. The suggester matches ciphers whose block size matches into the cipher text.

## Prerequirements
* openssl library (-lcrypto, debian: `apt-get install libssl-dev`)
* libmagic (-lmagic, debian `apt-get install libmagic-dev`)
* c++11 compiler (for threads)

## Compilation
`make` should do it

## Usage Examples
### Crack textfile
* Crack secret.txt.enc 
* Using passwords from wordlist.txt
* Match for first 32 ascii characters (default value)
* Auto-use suggested ciphers
```
encrack -i ./secret.txt.enc -w ./wordlist.txt -m firstascii 
```
### Crack binary data
* Crack secretpic.jpg.enc 
* Using passwords from wordlist.txt
* Match for image/jpeg using libmagic
```
encrack -i ./secretpic.jpg.enc -w ./wordlist.txt -m libmagic -o matchType=image/jpeg 
```
### Crack textfile using specific ciphers
* Crack secret.txt.enc 
* Using passwords from wordlist.txt
* Match for first 32 ascii characters (default value)
* use cipher list from ciphers.txt
```
encrack -i ./secret.txt.enc -w ./wordlist.txt -m firstascii -c ./ciphers.txt
```
### Crack file, search for a specific string with
* crack 2nd_pm.s3m.enc (which is a encrypted S3M music module)
* Using passwords from wordlist.txt
* match for the SCRM magic at position 44 which is specific for this file format
* use cipher list from ciphers_cbc.txt
* limit decryption length to 48 bytes to save a lot of time since the header starts at byte 44 and is 4 bytes long (encrack takes care for itself rounding up to block size)
* use 32 threads since we pretend to have such a supercool threadripper machine ;-)
```
./encrack -i ./2nd_pm.s3m.enc -w ./wordlist.txt -m string -o string='SCRM' -o position=44 -c ./ciphers_cbc.txt -l48 -t32
```


### Suggest ciphers
* Output suggested ciphers for secret.txt.enc and exit
```
encrack -i ./secret.txt.enc -s
```

## example output
```
$ ./encrack -m firstascii -i ./test.txt.enc -w ./testpwd.list -c ./ciphers_aes_cbc.txt  -t 1
* Loaded 9 password(s)
* Loaded 3 cipher(s)
* Loaded 240 bytes ciphertext
* running 1 thread(s)...
[MATCHER] Warning: setting default value of 'numBytes' to value '32'
[MATCHER] use -o numBytes=<value> to set user defined value
[00]: starting...
[00]: found candidate!
password : waynesburg
cipher   : aes-256-cbc
plaintext: Yo supergeheimer text!
Dieser Text ist so geheim, dass es geheimer nicht geht. Supergeheim. 
Aber so dermaßen geheim das glaubst du echt nicht! Abartig! 
Das ist alles so dermaßen krass geheim, da dreht es dir den Helm um!
Wirklich!


[00]: finishing...
* finished, all threads ended
```

## Todos
* Moar matching methods
* limit ciphertext to specific blocks/size
* newer PBKDF2 keyexpansion as option which seems to be default in newer openssl
* maybe other HMAC algorithms (currently default SHA256 only is supported)
* method to quit after first match
* output stats (progress)
* more output options for candidates (hexdump, into file)

## Benchmark
Rocking rockyou.txt on my poor little Core-i5 (i need a threadripper... ...why? ...why not?)
```
$ cat /proc/cpuinfo |grep model|sort -u
model		: 58
model name	: Intel(R) Core(TM) i5-3330 CPU @ 3.00GHz
$ time ./encrack -m firstascii -o numBytes=32 -i ./test.txt.enc -w ~/Downloads/rockyou.txt -c ./ciphers_cbc.txt 
* added matcher option numBytes: 32
* Loaded 14344390 password(s)
* Loaded 18 cipher(s)
* Loaded 240 bytes ciphertext
* running 4 thread(s)...
[00]: starting...
[02]: starting...
[01]: starting...
[03]: starting...
[00]: found candidate!
password : waynesburg
cipher   : aes-256-cbc
plaintext: Yo supergeheimer text!
Dieser Text ist so geheim, dass es geheimer nicht geht. Supergeheim. 
Aber so dermaßen geheim das glaubst du echt nicht! Abartig! 
Das ist alles so dermaßen krass geheim, da dreht es dir den Helm um!
Wirklich!


[02]: finishing...
[00]: finishing...
[01]: finishing...
[03]: finishing...
* finished, all threads ended

real	6m56,269s
user	27m34,244s
sys		0m0,234s
```
