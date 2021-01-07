#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cache.h"

//utility method to find if num is a power of 2 (to check validity of the inputs)
int powerOfTwo(int num){
    if(num == 0) return 0;
    while(num != 1){
        if(num % 2 != 0){
            return 0;
        }
        num /= 2;
    }
    return 1;
}
//initilizes a char array to hold zeros
void initializeArray(char* arr, int numElements){
    for(int i = 0; i<numElements-1; i++){
        arr[i] = '0';
    }
}
//initializes the cache with starting values, and mallocs the tags in the cache
void initializeCache(struct Set cache[], int numSets, int numBlocks, int tagBits){
    int i = 0;
    for(int j = 0; j < numSets; j++){
        cache[j].fullBlocks = 0;
        cache[j].firstIn = 0;
        for(i = 0; i < numBlocks; i++){
            initializeArray(cache[j].b[i].tag, tagBits+1);
            cache[j].b[i].valid = 0;
            cache[j].b[i].dirty = 0;   
        } 
    }  
}
//method to get the log (base 2) of a number
int logHelper(int num){
  int j;
  for(j = 0; j < 31; j++){
    if((num >> j) == 1){
       break; 
    } 
  }
  return j;
}
//helper method to raise 2 to the power of num,  i.e. 2^num
int powHelper(int num){
    int power = 2;
    if(num==0)return 1;
    for(int i = 1; i < num; i++){
        power *=2;
    }
    return power;
}
//method to convert from hexadecimal to binary
void hexToBin(char hex[], char* binaryRep, char byte[], int hexSize, int start){
    int index= start; 
    int hexInd = 2;//start after "0x"
    while(hex[hexInd]){
        if(hex[hexInd]=='0'){
            byte = "0000";
        }
        else if(hex[hexInd]=='1'){
            byte = "0001";
        }
        else if(hex[hexInd]=='2'){
            byte = "0010";
        }    
        else if(hex[hexInd]=='3'){
            byte = "0011";
        }
        else if(hex[hexInd]=='4'){
            byte = "0100";
        }
        else if(hex[hexInd]=='5'){
            byte = "0101";
        }
        else if(hex[hexInd]=='6'){
            byte = "0110";
        }
        else if(hex[hexInd]=='7'){
            byte = "0111";
        }
        else if(hex[hexInd]=='8'){
            byte = "1000";
        }
        else if(hex[hexInd]=='9'){
            byte = "1001";
        }
        else if(hex[hexInd]=='a'){
            byte = "1010";
        }
        else if(hex[hexInd]=='b'){
            byte = "1011";
        }
        else if(hex[hexInd]=='c'){
            byte = "1100";
        }
        else if(hex[hexInd]=='d'){
            byte = "1101";
        }
        else if(hex[hexInd]=='e'){
            byte = "1110";
        }
        else if(hex[hexInd]=='f'){
            byte = "1111";
        }
        int helper = 0;
        for(int i = index; i <= index+4; i++){
            if(i == 53 || helper == 5) return;
            binaryRep[i] = byte[helper];
            helper++;
        }
        hexInd++; 
        if(hexInd >= hexSize) break;
        byte = NULL;
        index+=4;
    }    
}
//converts a string representation of binary to decimal
int binToDec(char* binaryRep, int numDigits){
    int dec = 0;
    int power = 0;
    for(int i = numDigits-1; i>= 0; i--){
        if(binaryRep[i] == '0')
        {
            power++;
            continue;
        }
        else if(binaryRep[i] == '1'){
            dec+= powHelper(power);
            power++;
        }
        else if(binaryRep[i] == '\0') break;
    }
    return dec;
}
//fixes the length of a specific tag in the cache that is too long (i.e. its length is > tagBits)
void fixTag(struct Set cache[], int set, int block, int tagBits){
    int diff = 0;
    int h = tagBits;
    if(cache[set].b[block].tag[tagBits] != '\0'){
        while(cache[set].b[block].tag[h] != '\0' && h < 48){
            h++;
        }
        diff = h - tagBits;
    }
    if(diff > 0){
        h = tagBits;
        while(diff >0 && h < 48){
            cache[set].b[block].tag[h] = '\0';
            h++;
            diff--;
        }
    }
}
//method to add tag to cache and update the other variables connected to the cache's implementation
void addToCache(struct Set cache[], int set, int a, char* newTag, int tagBits, long time){
    int emptyInd = 0;
    int ind = 0;
    for(ind = 0; ind < a; ind++){
        if(cache[set].b[ind].valid == 0){
            emptyInd = ind;
            break;
        }
    }
    int i = 0;
    while(newTag[i] != '\0'){
        if(newTag[i] == '1'){
            cache[set].b[emptyInd].tag[i] = '1';  
        }
        i++;
        if(i>tagBits) break;
    }
    cache[set].fullBlocks++;
    cache[set].b[emptyInd].valid = 1;
    cache[set].b[emptyInd].dirty = time;
}

//checks if a tag is present in a specific set in the cache
int assocInCache(struct Set cache[], int set, char checkTag[], int numElements, long time){
    if(cache[set].fullBlocks == 0) return 0;
    for(int i = 0; i< cache[set].fullBlocks; i++){
        if(strlen(cache[set].b[i].tag)>numElements){
            fixTag(cache, set, i, numElements);
        }
        if(cache[set].b[i].valid == 1 && strcmp(cache[set].b[i].tag, checkTag) == 0){
            cache[set].b[i].dirty = time;
            return 1;
        }
        else{
            continue;
        }
    }
    return 0;
}
//replacement policy for direct cache (simply re-initializes the cache tag in a given set and replaces it)
void replaceDirect(struct Set cache[], int set, int tagBits, char* newTag, long time){
    initializeArray(cache[set].b[0].tag, tagBits);
    for(int i = 0; i < tagBits; i++){
        cache[set].b[0].tag[i] = newTag[i];
    }
    
    cache[set].b[0].dirty = time;
}
//replacement method to replace first in block
void fifo(struct Set cache[], int set, char* newTag, int tagBits, long time, int a){
    int index = cache[set].firstIn;
    initializeArray(cache[set].b[index].tag, tagBits+4);
    
    int i = 0;
    for(i = 0; i < strlen(newTag); i++){
        if(newTag[i] == '1'){
            cache[set].b[index].tag[i] = '1';
        }
        else{
            cache[set].b[index].tag[i] = '0';
        }  
    }
    cache[set].b[index].dirty = time;
    if(index + 1 >= a){
        cache[set].firstIn = 0;
    }
    else{
        cache[set].firstIn++;
    }  
}
//replacement method to replace least recently used block
void lru(struct Set cache[], int set, char* newTag, int tagBits, long time, int a){
    int least = cache[set].b[0].dirty;
    int indOfLeast = 0;
    for(int i = 1; i < a; i++){
        if(cache[set].b[i].dirty < least){
            least = cache[set].b[i].dirty;
            indOfLeast = i;
        }
    }
    for(int j = 0; j < tagBits; j++){
        cache[set].b[indOfLeast].tag[j] = newTag[j];
    }
    cache[set].b[indOfLeast].dirty = time;
}

int main(int argc, char* argv[argc+1]){
    if(argc < 6){
        printf("error\n");
        return EXIT_SUCCESS;
    }
    FILE* fp = fopen(argv[5], "r");
    if(fp == 0){
        printf("error\n");
        return EXIT_SUCCESS;
    }
    //variables to hold command line input
    int cache_size = atoi(argv[1]);
    char* associativity = argv[2];
    char* cache_policy = argv[3];
    int block_size = atoi(argv[4]);

    if(powerOfTwo(cache_size)== 0 || powerOfTwo(block_size)==0){
        printf("error\n");
        return EXIT_SUCCESS;
    }
    int a = 1;
    int numSets = 1;
    int assocCase = 0; //assocCase is 0 for direct, 1 for fully associative, and 2 for assoc:n
    if(strcmp(associativity, "direct")==0){
        numSets = cache_size / block_size;
    }
    else if(strcmp(associativity, "assoc")==0){
        assocCase = 1;
        a = cache_size / (numSets*block_size);
    } 
    else{
        assocCase = 2;
        a = associativity[6] - '0';
        numSets = cache_size / (a * block_size);
    }

    //create variables to hold output numbers
    int mem_read = 0;
    int mem_write = 0;
    int cache_hit = 0;
    int cache_miss = 0;

    char action;
    char address[15];
    char byte[5];
    int setIndexBits = logHelper(numSets);
    int blockOffset = logHelper(block_size);
    int tagBits = 48 - setIndexBits - blockOffset;
    char setIndex[setIndexBits+1];
    long time = 0; 
    struct Set cache[numSets];
    initializeCache(cache, numSets, a, tagBits);

    char* tag; //will be dynamically allocated memory within the while loop

    while(fscanf(fp, "%c %s\n", &action, address) != EOF){
        char* binAddress = (char*)malloc(53*sizeof(char));
        initializeArray(binAddress, 53);
        tag = (char*)malloc((48)*sizeof(char)+1);
        int bitsRemove = setIndexBits + blockOffset;
        int start = 0;
        if(strlen(address) != 14) start = 48 - ((strlen(address)-2)*4);
        hexToBin(address, binAddress, byte, strlen(address), start);
        
        int i = 0;
        for(int q = strlen(binAddress) - bitsRemove; q < strlen(binAddress)-blockOffset; q++){
            setIndex[i] = binAddress[q];
            i++;
        }
        for(i = 0; i < 48; i++){
            tag[i] = binAddress[i]; 
        }
        //This segment of code will ensure that the length of the tag is equal to tagbits
        int difference = 0;
        int here = tagBits;
        if(tag[tagBits] != '\0'){
            while(tag[here] != '\0' && here < 48){
                here++;
            }
            difference = here - tagBits;
        }
        if(difference > 0){
            here = tagBits;
            while(difference >0 && here < 48){
                tag[here] = '\0';
                here++;
                difference--;
            }
        }

        free(binAddress);
        int set = binToDec(setIndex, setIndexBits);

        //read memory access
        if(action == 'R'){
            if(assocInCache(cache, set, tag, tagBits, time)){
                cache_hit++;
                time++;
            }
            else{
                mem_read++;
                cache_miss++;
                if(cache[set].fullBlocks == a && assocCase != 0){
                    if(strcmp(cache_policy, "fifo") ==0){
                        fifo(cache, set, tag, tagBits, time, a);
                        fixTag(cache, set, cache[set].firstIn-1, tagBits);
                        cache[set].fullBlocks = a;
                        time++;
                    }
                    else{
                        lru(cache, set, tag, tagBits, time, a);
                        time++;
                    }
                }
                else if(assocCase == 0 && cache[set].fullBlocks == a){
                    replaceDirect(cache, set, tagBits, tag, time);
                    time++;
                }
                else{
                    addToCache(cache, set, a, tag,tagBits, time);
                    time++;
                }
            }   
        }
        //write memory access
        else if(action == 'W'){
            mem_write++;
            if(assocInCache(cache, set, tag, tagBits, time)){
                cache_hit++;
                time++;
            } 
            else{
                mem_read++;
                cache_miss++;
                if(cache[set].fullBlocks == a && assocCase != 0){
                    if(strcmp(cache_policy, "fifo") ==0){
                        fifo(cache, set, tag, tagBits, time, a);
                        fixTag(cache, set, cache[set].firstIn-1, tagBits);
                        cache[set].fullBlocks = a;
                        time++;
                    }
                    else{
                        lru(cache, set, tag, tagBits, time, a);
                        time++;
                    }
                }
                else if(assocCase == 0 && cache[set].fullBlocks == a){ 
                    replaceDirect(cache, set, tagBits, tag, time);
                    time++;
                }
                else{
                    addToCache(cache, set, a, tag,tagBits, time);
                    time++;
                }
            }
        }
        //reinitialize arrays to be reused in the while loop and free dynamically allocated tag
        initializeArray(setIndex, setIndexBits);
        initializeArray(byte, 4);
        free(tag); 
    }
    //output:
    printf("memread:%d\n", mem_read);
    printf("memwrite:%d\n", mem_write);
    printf("cachehit:%d\n", cache_hit);
    printf("cachemiss:%d\n", cache_miss);

    return EXIT_SUCCESS; 
}