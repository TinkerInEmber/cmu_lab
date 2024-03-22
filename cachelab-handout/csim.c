#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
struct cache{
    int valid;
    unsigned int tag;
    int time;
}cache1;

int main(int argc, char * argv[])
{
    int hit = 0, miss = 0, eviction = 0;
    int ch;
    int v_flag = 0; // Optional verbose flag that displays trace info, value = 1 then show
    int s, E, b, i, j, s2;
    FILE* fp = NULL;
    while((ch = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch(ch)
        {
             case 'h':
                printf("-h: Optional help flag that prints usage info\n-v: Optional verbose flag that displays trace info\n");
                printf("-s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
                printf("-E <E>: Associativity (number of lines per set)\n");
                printf("-b <b>: Number of block bits (B = 2^b is the block size)\n");
                printf("-t <tracefile>: Name of the valgrind trace to replay\n");
             case 'v':
                v_flag = 1;
                break;
             case 's':
                s = atoi(optarg);
                s2 = pow(2, s);
                break;
             case 'E':
                E = atoi(optarg);
                break;
             case 'b':
                b = atoi(optarg);
                break;
             case 't':
                fp = fopen(optarg, "r");
                break;
        }
            
    }
    /*initial cache*/
    struct cache **ca = (struct cache **)malloc(sizeof(struct cache *)*s2); //simulation of cache
    for(i = 0;i < s2; ++i)
    {
        ca[i] = (struct cache *)malloc(sizeof(struct cache)*E); 
        //ca[i] = (struct cache *)malloc(sizeof(cache1)*E);//another to achieve the same result
    }
    for(i = 0;i < s2; ++i)
    {
        for(j = 0; j < E; ++j)
        {
            ca[i][j].valid = 0;
            ca[i][j].time = 0;
        }
    }
    /*read trace*/
    char identifier;
    unsigned address;
    int size;
    while(fscanf(fp, " %c %x,%d", &identifier, &address, &size)>0)
    {
        if(v_flag)
        {
            printf("%c %u,%d",identifier,address,size);
        }
        unsigned tag = address>>(s+b);
        unsigned index = (address&((1<<(s+b))-1))>>b;
        //printf("%u",index);
        int lru_j = 0;
        int maxtime = ca[index][0].time;
        switch(identifier)
        {
            case 'I':
                break;
            case 'L':
            case 'S':
            case 'M':
                for(j = 0; j < E; ++j)
                {
                    if(ca[index][j].valid == 0) // miss
                    {
                        ca[index][j].valid = 1;
                        ca[index][j].tag = tag;
                        ca[index][j].time = 0;  
                        miss++;
                        if(v_flag)
                        {
                            printf(" miss");
                        }
                        break;
                    }
                    else //hit
                    {
                        if(ca[index][j].tag != tag) continue;
                        else
                        {
                            hit++;
                            if(v_flag)
                            {
                                printf(" hit");
                            }
                            ca[index][j].time = 0;
                            break;
                        }
                    }
                }
                if(j == E) // this situation indicates eviction
                {
                    miss++;
                    eviction++;
                    if(v_flag)
                    {
                        printf(" miss eviction");
                    }
                    for(j = 1;j < E; ++j)
                    {
                        if(ca[index][j].time > maxtime)
                        {
                            lru_j = j;
                            maxtime = ca[index][j].time; 
                        }
                    }
                    ca[index][lru_j].time = 0;
                    ca[index][lru_j].tag = tag;
                }
                if('M' == identifier)
                {
                    hit++;
                    if(v_flag)
                    {
                        printf(" hit");
                    }
                }
                printf("\n");
                break;                
        }
        /*refresh time*/
        for(j = 0; j < E; ++j)
        {
            ca[index][j].time++;
        }
    }
    
    
    // free malloc
    for(i = 0;i < s; ++i)
    {
        free(ca[i]);
    }
    free(ca);
    
    fclose(fp);
    printSummary(hit, miss, eviction);
    return 0;
}
