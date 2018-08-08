#!/bin/sh
openssl enc -ciphers | grep -v "Supported ciphers" | sed -r 's/\s+/\n/g'| grep "-" | sed -r 's/^-//'
