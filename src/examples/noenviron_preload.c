#define _GNU_SOURCE
#include <unistd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
$ gcc -o noenviron_preload.so -shared -fpic -ldl -Wl,-init,init noenviron_preload.c
$ LD_PRELOAD=./noenviron_preload.so ./nocall_detect 
Environment is clean
*/

extern char **environ;

int (*o_execve)(const char *path, char *const argv[], char *const envp[]) = NULL;

char *sopath;

// Called as soon as the library is loaded, the program has not executed any 
// instructions yet.
void init()
{
    int i, j;
    static const char *ldpreload = "LD_PRELOAD";
    // First save the value of LD_PRELOAD
    int len = strlen(getenv(ldpreload));
    sopath = (char*) malloc(len+1);
    strcpy(sopath, getenv(ldpreload));
    // unsetenv() has a weird behavior, this is a custom implementation
    // Look for LD_PRELOAD variable
    for(i = 0; environ[i]; i++)
    {
        int found = 1;
        for(j = 0; ldpreload[j] != '\0' && environ[i][j] != '\0'; j++)
            if(ldpreload[j] != environ[i][j])
            {
                found = 0;
                break;
            }
        if(found)
        {
            // Set to zero the variable
            for(j = 0; environ[i][j] != '\0'; j++)
                environ[i][j] = '\0';
            break;
            // Free that memory
            free((void*)environ[i]);
        }
    }
    // Remove the string pointer from environ
    for(j = i; environ[j]; j++)
        environ[j] = environ[j+1];
}


int execve(const char *path, char *const argv[], char *const envp[])
{
    int i, j, ldi = -1, r;
    char** new_env;
    if(!o_execve)
        o_execve = dlsym(RTLD_NEXT,"execve");
    // Look if the provided environment already contains LD_PRELOAD
    for(i = 0; envp[i]; i++)
    {
        if(strstr(envp[i], "LD_PRELOAD"))
            ldi = i;
    }
    // If it doesn't, add it at the end
    if(ldi == -1)
    {
        ldi = i;
        i++;
    }
    // Create a new environment
    new_env = (char**) malloc((i+1)*sizeof(char*));
    // Copy the old environment in the new one, except for LD_PRELOAD
    for(j = 0; j < i; j++)
    {
        // Overwrite or create the LD_PRELOAD variable
        if(j == ldi)
        {
            new_env[j] = (char*) malloc(256);
            strcpy(new_env[j], "LD_PRELOAD=");
            strcat(new_env[j], sopath);
        }
        else
            new_env[j] = (char*) envp[j];
    }
    // That string array is NULL terminated
    new_env[i] = NULL;
    r = o_execve(path, argv, new_env);
    free(new_env[ldi]);
    free(new_env);
    return r;
}
// You also have to patch all the other variants of exec
