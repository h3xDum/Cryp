
/**********************************************
* Simple crypto cli that will encrypt         * 
* any given directory & sub direcotry with    *
* user chosen ecnryption method               *
**********************************************/


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

#include "dir.h"
#include "aes.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "dir.h"

const char *program_version = "Cryp 1.0.0";
const char *bug_address = "Hakmon.lior1@gmail.com";
const char *documentation = "Simple crpyto program that allow files & directories\nencryption, use with caution!";

// linked --options arguments
static struct option long_options[] = {
   {"enc", required_argument , NULL , 'e'},
   {"dec", required_argument , NULL , 'x'},
   {"file" , required_argument , NULL , 'f'},
   {"directory" , required_argument , NULL , 'd'},
   {"version" , no_argument , NULL , 'v'},
   {"usage" , no_argument , NULL , 'u'}, 
   {"help" , no_argument , NULL , 'h'},
   {NULL , 0 , NULL , 0}

};

// structure to hold user arguments
struct args {
   char encryption[32];
   char file[128];
   char directory[128];
   char decryption[32];
};


void error_msg(int n) {
   if (n == 1) {
      printf("\nconflicting arguments used.");
      printf("\n\"Crack --help\" for help.  ");
      exit(1);
   }
   else if (n == 2)
   {
      printf("\n\"Cryp --help\" for help.  ");
      exit(1);
   }
   else {
      exit(1);
   }
}  

void usage() {
   printf("Usage: Cryp -e <encryption> -f <file> \n");
}

void help() {
   printf("\n");
   usage();
   printf("\n%s \n\n\n" , documentation);
   printf("    -e, --enc                         Chose encryption method AES/TDES \n"); 
   printf("    -x, --dec                         Chose decryption method AES/TDES \n"); 
   printf("    -f, --file                        Choose file OR file path             \n"); 
   printf("    -d, --directory                   Choose directory OR directory path   \n");  
   printf("    -v, --version                     Display version information          \n"); 
   printf("    -u, --usage                       Display usage information            \n"); 
   printf("    -h, --help                        Display this usage screen            \n"); 
   printf("\n\nPlease reaport any bugs \nfound at %s\n" , bug_address);
   exit(1);
}


// @TODO disallow argument values starting with -, for exmple: ./Crypt -e -type
// @TODO add safe option for checking every file   
// @TODO handle huge directory tree 
int main(int argc , char **argv) {

   if (argc < 2) {
      help();
   }

   struct args arguments;
   int option_arg ;
   bool flag , enc_flag , fflag, dflag , dec_flag ,  directory_mode , file_mode ;  //validating conflicting argumetns
       
   
   while ((option_arg= getopt_long(argc, argv, "e:x:f:d:vuh", long_options, NULL)) != -1) {
      // @TODO disallow argument values starting with -, for exmple: ./Crypt -e -type
      // @TODO add safe option for checking every file   
      // @TODO handle huge directory tree    
      switch (option_arg)
      {
      case 'e': //encryption 
         if (enc_flag || dec_flag) {
            error_msg(1);
         }
         else if((strcmp(optarg , "AES")) == 0  || (strcmp(optarg , "RC4")) == 0 || (strcmp(optarg , "TDES")) == 0) {
            enc_flag = true;
            strncpy(arguments.encryption, optarg ,31);
            arguments.encryption[31] = '\0';
         }
         else {
            printf(RED"\n[!] only valid encryption types are AES/RC4/TDES ... aborting .. \n");
            error_msg(2);
         }
         break;
      
      case 'x': // decryption
         if (enc_flag || dec_flag) {
            error_msg(1);
         }
         else if((strcmp(optarg , "AES")) == 0  || (strcmp(optarg , "RC4")) == 0 || (strcmp(optarg , "TDES")) == 0) {
            dec_flag = true;
            strncpy(arguments.decryption , optarg , 31);
            arguments.decryption[31] = '\0';
         }
         else {
            printf(RED"\n[!] only valid decryption types are AES/RC4/TDES ... aborting .. \n");
            error_msg(2);
         }
         break;
      
      case 'f': //file
         if (flag || fflag )  {   
            error_msg(1); 
         }
         else if ( strlen(optarg) > 127) {
            printf(RED "\n[!] file argument is too long ... aborting ... \n");
            error_msg(2);
         }
         flag = true;
         fflag = true;
         strncpy(arguments.file , optarg , 127);
         arguments.file[123] = '\0';
         break;
      case 'd': // directory
         if (flag || dflag ) {
            error_msg(1);
         }
         else if (strlen(optarg) > 127 ) {
            printf(RED "\n[!] directory argument is too long ... aborting ... \n");
            error_msg(2);
         } 
         flag = true;
         dflag = true;  
         strncpy(arguments.directory , optarg , 127);
         arguments.directory[123] = '\0';
         break;
      case 'v': //version
         printf("\n%s", program_version);
         exit(1);
         break;
      case 'u': //usage
         printf("\n");
         usage();
         exit(0);
         break;
      case 'h': //help
         help();
         break;
      case '?':
         error_msg(2);
         break;
      default:
         break;
      }
   }


   // check for lack of arguenets
   if ( fflag && enc_flag  ) {
      printf(GRN"\n[+] " RESET "File chosen: %s\n", arguments.file );
      printf(GRN"\n[+] " RESET "Encryption method: %s\n" , arguments.encryption);
      file_mode = true;
   }

   else if ( fflag && dec_flag  ) {
      printf(GRN"\n[+] " RESET "File chosen: %s\n", arguments.file );
      printf(GRN"\n[+] " RESET " method: %s\n" , arguments.decryption);
      file_mode = true;
   }      
   
   else if ( dflag && enc_flag ) {
      printf(GRN"\n[+] " RESET "Directory chosen: %s\n", arguments.directory) ; 
      printf(GRN"\n[+] " RESET "Encryption method: %s\n" , arguments.encryption);
      directory_mode = true;
   }

   else if ( dflag && dec_flag ) {
      printf(GRN"\n[+] " RESET "Directory chosen: %s\n", arguments.directory) ; 
      printf(GRN"\n[+] " RESET "Decryption method: %s\n" , arguments.decryption);
      directory_mode = true;
   }

   else if ( (enc_flag || dec_flag ) && dflag == false && fflag == false ) {
      printf(GRN"\n[+] " RESET "Encryption method: %s\n", arguments.encryption) ;
      printf(RED"\n[!] no file specifier ... aborting ...  \n ");
      error_msg(2); 
   }
   else {
      printf(GRN"\n[+] " RESET "file specifier chosen\n ");
      printf(RED"\n[!] no encryption/decryption flag set ... aborting ... \n ");
      error_msg(2);
   }


   // handle file mode 
   if (file_mode) {
      // check file intergrity 
      
      struct stat buf;
      int fcheck = lstat(arguments.file , &buf );
      
      if (S_ISDIR(buf.st_mode)) {
         printf(RED"\n[!] \"%s\" is a directory not a file ... aborting ...\n " , arguments.file);
         error_msg(2);   
      }
      
      else if (S_ISLNK (buf.st_mode)) {
         printf(RED "\n[!] \"%s\" is a symbolic link file ... aborting ...  \n" , arguments.file);
         error_msg(2);
      }

      
      else if (S_ISREG (buf.st_mode)) {
         unsigned char key[20];
         
         // enc the file
         if (enc_flag){
            printf(GRN "\n[+]" RESET" enter a key to encrypt the file: ");
            scanf("%20s" , key );
            if (Encrypt_AES(arguments.file , key) == 0 ) 
               printf(GRN "\n[+]" RESET" successfully encrypted ! ");
         }

         // decrypt the file
         else {
            printf(GRN "\n[+]" RESET" enter a key to decrypt the file: ");
            scanf("%20s" , key );
            if (Decrypt_AES(arguments.file , key) == 0 ) {
               printf(GRN "\n[+]" RESET" successfully decrypted ! ");
            }
         }
      }
      
      else {
         printf(RED "\n[!] \"%s\" does not exist  ... aborting ...  \n" , arguments.file);
         error_msg(2);
      }    
     
   }  

    



   // handle directory mode 
   else if (directory_mode) {
      
      
      DIR* dir = opendir(arguments.directory);
      struct dirent* entity;

      if (dir == NULL) {
         printf(RED "\n[!] \"%s \" is not a directory! ");
         error_msg(2); 
      }

      printf("\n\n" GRN"[+]" RESET " Listing all the Files in: %s" , arguments.directory);
      entity = readdir(dir);
      while (entity != NULL) {
        if ( (entity->d_type == DT_DIR  || entity -> d_type == DT_REG ) && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            printf(GRN "\n     -----------> " RESET "%s" , entity ->d_name);
        }
        entity = readdir(dir);
    }

    closedir(dir);
      

      char **directory_array = directory_tree(arguments.directory);
      
      if ( directory_array != NULL ) {
      
         for (size_t i = 1; i < atoi(directory_array[0]) + 1 ; i ++   ) {
            
            dir = opendir(directory_array[i]);
            entity = readdir(dir);
            printf(GRN"\n\n[+]" RESET " Listing all the files in: %s" , directory_array[i]);
         
            while(entity != NULL) {   
            
               if ( (entity->d_type == DT_DIR  || entity -> d_type == DT_REG ) && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0)  {
                 printf(GRN "\n     -----------> " RESET "%s" , entity ->d_name);
               }

               entity = readdir(dir);
            }
            
            closedir(dir);      
         }         
      }



      if ( enc_flag ) {

         
         // validate user choice 
         char answer[5];
         unsigned char key[20];
         printf(RED "\n\n[!]" RESET " Are you sure you want to encrypt all those files ? (yes/no): ");
         scanf("%4s" , answer);
         if ( strcmp (answer , "yes") !=  0 ) { printf("\n NOT ENCRYPTING THE FILES !"); exit(1);}
         printf(GRN "\n[+]" RESET " Please enter a 16 charecters long key to enc the files : ");
         scanf("%s19" , key);

         // enc the current folder
         dir = opendir(arguments.directory);
         entity = readdir(dir);
         while ( entity != NULL ) {
            if (entity -> d_type == DT_REG) {
               char path[100] = { 0 };
               strcpy(path , arguments.directory);
               strcat(path , "/");
               strcat(path , entity -> d_name);

               if (Encrypt_AES(path , key ) == 0) {printf(GRN"\n     -----------> " RESET "encrypted \"%s\" successfully" , path);}
               else {printf(RED"\n     -----------> didnt managed to enc \"%s\"" RESET , path);}
            
            }
            entity = readdir(dir);            
         }
         closedir(dir);

         // enc sub directories
         if ( directory_array != NULL) {
            for (size_t i = 1; i < atoi(directory_array[0]) + 1 ; i ++   ) {
               dir = opendir(directory_array[i]);
               entity = readdir(dir);
               while ( entity != NULL ) {
                  if ( entity -> d_type == DT_REG) {
                     char sub_path[100] = { 0 };
                     strcpy(sub_path , directory_array[i]);
                     strcat(sub_path , "/");
                     strcat(sub_path , entity -> d_name);
                     
                     if (Encrypt_AES(sub_path , key ) == 0){printf(GRN"\n     -----------> " RESET "encrypted \"%s\" successfully" , entity -> d_name);}
                     else {printf(RED"\n     -----------> didnt managed to enc \"%s\"" RESET , sub_path);}
                  
                  }
                  entity = readdir(dir);
               }
               closedir(dir);
            }
         }     
      }



      else if (dec_flag) {

         // validate user choice 
         char answer[5];
         unsigned char key[20];
         printf(RED "\n\n[!]" RESET " Are you sure you want to try and decrypt all those files ? (yes/no): ");
         scanf("%4s" , answer);
         if ( strcmp (answer , "yes") !=  0 ) { printf("\n NOT DECRYPTING THE FILES !"); exit(1);}
         printf(GRN "\n[+]" RESET " Please enter a 16 charecters long key to enc the files : ");
         scanf("%s19" , key);

         // dec the current folder
         dir = opendir(arguments.directory);
         entity = readdir(dir);
         while ( entity != NULL ) {
            if (entity -> d_type == DT_REG) {

               char path[100] = { 0 };
               strcpy(path , arguments.directory);
               strcat(path , "/");
               strcat(path , entity -> d_name);

               if (Decrypt_AES(path , key) == 0){printf(GRN"\n     -----------> " RESET "decrypted \"%s\" successfully" , path );}
               else {printf(RED"\n     -----------> didnt managed to decrypt \"%s\"" RESET , path);}
            }
            entity = readdir(dir);            
         }
         closedir(dir);

         // decrypt sub directories
         if ( directory_array != NULL) {
            for (size_t i = 1; i < atoi(directory_array[0]) + 1 ; i ++   ) {
               dir = opendir(directory_array[i]);
               entity = readdir(dir);
               while ( entity != NULL ) {
                  if ( entity -> d_type == DT_REG) {

                     char sub_path[100] = { 0 };
                     strcpy(sub_path , directory_array[i]);
                     strcat(sub_path , "/");
                     strcat(sub_path , entity -> d_name);
                     
                     if (Decrypt_AES(sub_path , key) == 0){printf(GRN"\n     -----------> " RESET "decrypted \"%s\" successfully" , sub_path );}
                     else {printf(RED"\n     -----------> didnt managed to decrypt \"%s\"" RESET , sub_path);}
                  }
                  entity = readdir(dir);
               }
               closedir(dir);
            }
         }     
      }
   } // directory mode
   
   
   
      























   
















   
   return 0;
}  