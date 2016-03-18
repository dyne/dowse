#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "sha256.h"
#include "base64.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// openssl
#include <openssl/sha.h>
#include <stddef.h>

void die(int code, char *msg) {
    if(msg) fprintf(stderr,"ERR: %s\n", msg);
    exit(code);
}

int main(int argc, char **argv) {
    char path[512];
    struct stat fs;
    FILE *fd;
    uint8 *buf;
    size_t len, dlen;
    uint8 digest[32];
    char *digest64;
    sha256_context sha;

    int c;

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char myoutput[65];
    char output[65];

    snprintf(path,512,"%s",argv[1]);
    if( stat(path,&fs) <0) die(1, "stat");
    fprintf(stderr,"%s %ld\n",path, fs.st_size);

    if(!fs.st_size) die(1, "stat size");

    fd = fopen(path,"r");
    if(!fd) die(1, "fopen");

    buf = malloc(fs.st_size);
    if(!buf) die(1, "malloc");

    len = fread(buf,1,fs.st_size,fd);
    if(len != fs.st_size) die(1, "fread");

    sha256_starts(&sha);
    sha256_update(&sha, buf, (uint32)len);
    sha256_finish(&sha, digest);

    for(c = 0; c<32; c++)
        sprintf(myoutput + (c * 2),"%02x",digest[c]);
    fprintf(stderr,"%s\n", myoutput);


    SHA256_Update(&sha256, buf, len);
    SHA256_Final(hash, &sha256);
    for(c = 0; c < SHA256_DIGEST_LENGTH; c++)
        sprintf(output + (c * 2), "%02x", (unsigned char)hash[c]);
    output[64] = '\0';
    fprintf(stderr,"%s\n",output);




    output[64] = '\0';
    free(buf);
    exit(0);
}
