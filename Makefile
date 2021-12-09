#This is the makefile for 3 source files: aes.c , dir.c and Cryp.c 
#compilation is done with gcc linked to lcrypto & lm libraries

$(CC) = gcc
Cryp:
		$(CC) -w src/aes.c src/dir.c src/Cryp.c -o Cryp -lcrypto -lm


