### encrack
tool for cracking openssl enc style encrypted files. Since there is no hash of the password or something like that stored in the file, successful decryption must be determined from plaintext directly. Currently it matches the first 32 bytes for being 7 bit clean ascii. More matching methods coming soon

## prerequirements
* openssl library (-lcrypto, debian: `apt-get install libssl-dev`)
* c++11 compiler (for threads)

## compilation
g++ -o encrack encrack.cpp -lpthread -lcrypto

## usage example
```
$ ./encrack -w ./testpwd.list -i ./test.txt.enc -c ./ciphers_cbc.txt  
* Loaded 9 password(s)
* Loaded 18 cipher(s)
* Loaded 240 bytes ciphertext
* running 4 thread(s)...
[00]: starting...
[01]: starting...
[02]: starting...
[03]: starting...
[01]: found candidate!
password : pfaelzersaumagen
cipher   : aes-256-cbc
plaintext: Yo supergeheimer text!
Dieser Text ist so geheim, dass es geheimer nicht geht. Supergeheim. 
Aber so dermaßen geheim das glaubst du echt nicht! Abartig! 
Das ist alles so dermaßen krass geheim, da dreht es dir den Helm um!
Wirklich!


[02]: finishing...
[03]: finishing...
[01]: finishing...
[00]: finishing...
* finished, all threads ended
```



