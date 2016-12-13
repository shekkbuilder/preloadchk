#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>
#include <errno.h>

/*
$ gcc -o fakememory_preload.so -shared -fpic -ldl fakememory_preload.c
$ LD_PRELOAD=./fakememory_preload.so ./memory_detect 
Memory maps are clean
*/

FILE* (*o_fopen)(const char*, const char*) = NULL;
char *soname = "fakememory_preload.so";

void fakeMaps(char *original_path, char *fake_path, char *pattern)
{
    FILE *original, *fake;
    char buffer[PATH_MAX];
    original = o_fopen(original_path, "r");
    fake = o_fopen(fake_path, "w");
    // Copy original in fake but discard the lines containing pattern
    while(fgets(buffer, PATH_MAX, original))
        if(strstr(buffer, pattern) == NULL)
            fputs(buffer, fake);
    fclose(fake);
    fclose(original);
}

FILE* fopen(const char *path, const char *mode)
{
    char real_path[PATH_MAX], maps_path[PATH_MAX];
    pid_t pid = getpid();
    if(!o_fopen)
        // Find the real function pointer
        o_fopen = dlsym(RTLD_NEXT, "fopen");
    // Resolve symbolic links and dot notation fu
    realpath(path, real_path);
    snprintf(maps_path, PATH_MAX, "/proc/%d/maps", pid);
    if(strcmp(real_path, maps_path) == 0)
    {
        snprintf(maps_path, PATH_MAX, "/tmp/%d.fakemaps", pid);
        // Create a file in tmp containing our fake map
        fakeMaps(real_path, maps_path, soname);
        return o_fopen(maps_path, mode);
    }
    // Everything is ok, call the real open
    return o_fopen(path, mode);
}
