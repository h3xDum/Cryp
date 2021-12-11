
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "aes.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

static char fldr_Arr[200][200] = { "" "" };
static size_t fldr_Count = 0;




 void find_sFolders(const char* dirname ) {
    
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        return; 
    }

    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL) {
        if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[100] = { 0 };
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);
            strcat(fldr_Arr[fldr_Count], path );
            fldr_Count ++;
            find_sFolders(path);
        }
        entity = readdir(dir);
    }

    closedir(dir);
}



char **directory_tree(char* folder){

  // filling sub folder array
  find_sFolders(folder);
 
  // no sub directories 
  if (fldr_Count  < 1 ) {
    return NULL; 
  }

  char **ans = malloc((sizeof(char*) * fldr_Count) + 1 );
  // first string will be the array length (fldr_Count)
  ans[0] = malloc((int)((ceil(log10(fldr_Count))+1)*sizeof(char)));
  sprintf(ans[0], "%d" , fldr_Count);
  
  for ( size_t i = 1 ; i < fldr_Count+1; i++) {
    ans[i] = malloc((sizeof(char) * sizeof(fldr_Arr[i])) );
    strcpy(ans[i] , fldr_Arr[i-1]);  
  }
  return ans;
  
}













