#ifndef __CACHE_H
#define __CACHE_H

struct Block{
    int valid;
    long dirty;
    char tag[48];
};
 
struct Set{
    int fullBlocks;
    int firstIn;
    struct Block b[64];
};

int powerOfTwo(int);
void initializeArray(char*, int); 
void initializeCache(struct Set[], int, int, int );
int logHelper(int);
int powHelper(int);
void hexToBin(char[], char*, char[], int, int);
int binToDec(char*, int);
void fixTag(struct Set[], int, int, int);
void addToCache(struct Set[], int, int , char[] , int, long);
int assocInCache(struct Set[], int, char[], int, long);
void replaceDirect(struct Set[], int, int, char* , long);
void fifo(struct Set[], int, char*,int, long, int);
void lru(struct Set[], int, char*, int, long, int);

#endif