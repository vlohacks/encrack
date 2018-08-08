# encrack
tool for cracking openssl enc style encrypted files. Since there is no hash of the password or something like that stored in the file, successful decryption must be determined from plaintext directly. Currently 2 matching modules are implemented
* firstascii:	checks the first numBytes being ascii 7 bit
* libmagic:		uses libmagic to match data against mime type matchType.
	(note: matchType can be a substring. text/plain also includes text/plain; charset: utf-16)

## prerequirements
* openssl library (-lcrypto, debian: `apt-get install libssl-dev`)
* libmagic (-lmagic, debian `apt-get install libmagic-dev`)
* c++11 compiler (for threads)

## compilation
`make` should do it

## usage example
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
