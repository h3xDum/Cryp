#include "aes.h"
#include <openssl/opensslconf.h>
#include <stddef.h>
#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/rand.h>
#include <sys/stat.h>


struct ctr_state { 
    unsigned char ivec[16];   
    unsigned int num; 
    unsigned char ecount[16]; 
}; 

FILE *fp;
FILE *rp;
FILE *op;   
char *buffer; 
AES_KEY key; 


int bytes_read, bytes_written ;  
unsigned char indata[AES_BLOCK_SIZE]; 
unsigned char outdata[AES_BLOCK_SIZE];  
struct ctr_state state;   

int init_ctr(struct ctr_state *state, const unsigned char iv[16]){     
    state->num = 0; 
    memset(state->ecount, 0, 16);      
    memset(state->ivec + 8, 0, 8);  
    memcpy(state->ivec, iv, 8); 
} 

void validate_key(unsigned char *usr_key) {

    if (strlen(usr_key) != 16  ) { 
      size_t count = 0;
      while (count < 3) {
          buffer = malloc(sizeof(unsigned char) * 20);
          printf("\x1B[31m" "\n[!]" "\x1B[0m" " WRONG KEY SIZE! , Please enter a 16 charecter long key: ");
          scanf("%20s" , buffer);
          if (strlen(buffer) == 16 ) {
            strcpy(usr_key , buffer);
            free(buffer);
            break;
          }
          free(buffer);
          count++;
        }
        if ( count > 2 ) {fputs("\x1B[31m" "\n[!!]" "\x1B[0m" " Too many wrong keys " , stderr); exit (1);}
    }    

}

int Encrypt_AES(char *file_name  , unsigned char *usr_key ) {

    
    // Key & IV initialization
    unsigned char iv[16] = {0};
    validate_key(usr_key);
    AES_set_encrypt_key(usr_key, 128, &key);
    RAND_bytes(iv , 16 );
    
    
    // opening file & creating enc file       
    buffer = malloc(sizeof (char) * strlen(file_name) + sizeof(char) * 4   );
    strcpy(buffer, file_name);
    strcat(buffer, ".enc");
    fp=fopen(file_name ,"a+b");
    op=fopen(buffer,"w");
    if (fp==NULL) {printf("\x1B[31m" "\n[!!]" "\x1B[0m" " couldn't open \"%s\" " , file_name ); return -1;}   
    if (fp==NULL) {printf("\x1B[31m" "\n[!!]" "\x1B[0m" " couldn't write to files in \"%s\" directory " , file_name ); return -1;}       


    // setting enc file first 16 bytes as the IV 
    bytes_written = fwrite(iv , 1 , AES_BLOCK_SIZE , op );
    
    // encryption  
    while (1) {     
        // ctr initialization
        init_ctr(&state, iv);
        bytes_read = fread(indata, 1, AES_BLOCK_SIZE, fp); 
        AES_cfb128_encrypt(indata, outdata, bytes_read, &key, state.ivec, state.ecount, &state.num);    
        bytes_written = fwrite(outdata, 1, bytes_read, op); 
        // end of the file 
        if (bytes_read < AES_BLOCK_SIZE)  
            break; 
    }    

    // cleanup
    fclose (fp); 
    fclose (op);
    free(buffer);
    remove(file_name);
    return 0;
}

int Decrypt_AES(char *file_name , unsigned char *usr_key) {
    
    validate_key(usr_key);

    // opening enc file & creating dec file
    buffer = malloc(sizeof (char) * strlen(file_name) + sizeof(char) * 4   );
    strcpy(buffer, file_name);
    strcat(buffer, ".dec");
    rp=fopen(buffer,"w");
    op=fopen(file_name,"a+b");
    if (op==NULL) {printf("\x1B[31m" "\n[!!]" "\x1B[0m" " couldn't open \"%s\" " , file_name ); return -1;}
    if (rp ==NULL){printf("\x1B[31m" "\n[!!]" "\x1B[0m" " couldn't write to files in \"%s\" directory " , file_name ); return -1;}


    // IV extraction
    unsigned char iv[16] = {0};        
    bytes_read = fread( iv , 1 , AES_BLOCK_SIZE , op);
    if (bytes_read != 16) {
        printf("\x1B[31m" "\n[!!]" "\x1B[0m" " couldn't extract \"%s\" iv " , file_name ); 
        fclose(op);
        fclose(rp);
        remove(buffer);
        free(buffer);        
        return -1;
    }
    
    // setting the new file pointer position & initializing key
    fseek(op , AES_BLOCK_SIZE , SEEK_SET);
    AES_set_encrypt_key( usr_key, 128, &key);

  
    // decryption   
    while (1) {    
       // ctr initialization
      init_ctr(&state, iv);
      bytes_read = fread(indata, 1, AES_BLOCK_SIZE, op);
      AES_cfb128_encrypt(indata, outdata, bytes_read, &key, state.ivec, state.ecount, &state.num); 
      bytes_written = fwrite(outdata, 1, bytes_read, rp);
      
      // end of file 
      if (bytes_read < AES_BLOCK_SIZE) 
        break; 
      }   

    // cleanup
    fclose (rp); 
    fclose (op);
    free(buffer);
    remove(file_name);
    return 0;
}