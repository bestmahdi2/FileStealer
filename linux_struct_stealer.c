#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define MAX_PATH 1024

char *searchingPathes[] = {"/home", "/mnt", "/media"};
FILE *logFile;


void fsize(char *name, int indentation);
void dirwalk(char *, int indentation, void (*fcn)(char *, int));


int main(){
    int n = sizeof(searchingPathes) / sizeof(searchingPathes[0]);
    if((logFile = fopen("./log.txt", "w")) == NULL){
        printf("Can't open log file!");
        return 1;
    }
    for(int i = 0; i < n; i++){
        fsize(searchingPathes[i], 0);
    }
    fclose(logFile);
}

/* Print size of file "name" */
void fsize(char *name, int indentation){
    struct stat stbuf;

    if(stat(name, &stbuf) == -1){
        fprintf(stderr, "fsize: can't access %s\n", name);
        return;
    }

    char indentStr[indentation + 1];
    indentStr[indentation] = '\0';
    for(int i = 0; i < indentation; i++) indentStr[i] = '\t';

    if((stbuf.st_mode & S_IFMT) == S_IFDIR){
        fprintf(logFile, "%s%s\n", indentStr, name);
        dirwalk(name, indentation + 1, fsize);
    }else {
        fprintf(logFile, "%s%s %8ld\n", indentStr, name, stbuf.st_size);

    }

}

void dirwalk(char *dir, int indentation, void (*fcn)(char *, int)){
    char name[MAX_PATH];
    DIR *dfd;
    struct dirent *dp;

    if((dfd = opendir(dir)) == NULL){
        fprintf(stderr, "dirwalk: can't open %s\n", dir);
        return;
    }

    while((dp = readdir(dfd)) != NULL){
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
            continue;
        }

        if(strlen(dir) + strlen(dp->d_name) + 2 > sizeof(name)){
            fprintf(stderr, "dirwalk: name %s/%s is too long\n", dir, dp->d_name);
        } else {
            sprintf(name, "%s/%s", dir, dp->d_name);
            (*fcn)(name, indentation);
        }
    }
    closedir(dfd);
}