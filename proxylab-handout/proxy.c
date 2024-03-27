#include <stdio.h>
#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
//cache line
#define CACHELINE 10

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

typedef struct URLs {
    char host[MAXLINE];
    char port[MAXLINE];
    char path[MAXLINE];
}URL;
void sighandler(int sig) {	;	}

void doit(int fd);
void parse_uri(char *uri, URL* head);
void build_header(rio_t *rp, char* serverbuf, URL* headers); 
//part2
void *thread(void *vargp);

//part3
typedef struct {
	char buf[MAX_OBJECT_SIZE];
	char url[MAXLINE];
	int size;
	int valid;
	int time;
} cacheLine;
typedef struct{
	cacheLine line[CACHELINE];
	int readcnt, curtime;
	sem_t mutex, writer;
} cache_t;

cache_t cache;

void cache_init();
int getCacheIndex(char *url);
void cacheWrite(char* buf, char *url, int size);


int main(int argc, char **argv)
{
	int listenfd;
	socklen_t clientlen;
	char hostname[MAXLINE], port[MAXLINE];
	struct sockaddr_storage clientaddr;
	
	//part2
	sem_t mutex;
	int *connfdp;
	pthread_t tid;
	cache_init();
	
	
	/* Check command line args */
	if(argc != 2){
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	//signal(SIGPIPE, sighandler);	//捕获SIGPIPE信号
	listenfd = Open_listenfd(argv[1]);
	while(1) {
		clientlen = sizeof(clientaddr);
		connfdp = Malloc(sizeof(int));
		//P(&mutex);
		*connfdp = Accept(listenfd, (SA*) &clientaddr, &clientlen);
		//V(&mutex);
		Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accepted connection from (%s, %s)\n", hostname, port); 
		Pthread_create(&tid, NULL, thread, connfdp);

	}
    //printf("%s", user_agent_hdr);
    return 0;
}

/* $end proxymain */

/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int fd) {
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char buf2server[MAXLINE], uri1[MAXLINE];
    URL header;
	rio_t rio, rio2server;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))  //line:netp:doit:readrequest
        return;
    // printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);       //line:netp:doit:parserequest
    if (strcasecmp(method, "GET")) {                     //line:netp:doit:beginrequesterr
        printf("Proxy does not implement this method");
        return;
    } 
    strcpy(uri1, uri); // uri will be changed in parse_uri, so it needs to be preserved
    int idx = getCacheIndex(uri);
    if(idx != -1){
    	P(&cache.mutex);
    	cache.readcnt++;
    	if(cache.readcnt == 1){
    		P(&cache.writer);
    	}
    	V(&cache.mutex);
    	Rio_writen(fd, cache.line[idx].buf, cache.line[idx].size);
    	P(&cache.mutex);
    	cache.readcnt--;
    	if(cache.readcnt == 0){
    		P(&cache.writer);
    	}
    	V(&cache.mutex);
    	printf("Cache refresh!\n");
    	return ;
    }
    
    parse_uri(uri, &header);
    build_header(&rio, buf2server, &header);
    
    int serverfd = Open_clientfd(header.host, header.port); // p631
    
    Rio_readinitb(&rio2server, serverfd); // p600
    Rio_writen(serverfd, buf2server, strlen(buf2server));
    ssize_t n;
    
    int cacheSize = 0;
    memset(buf, 0, sizeof(buf));
    while((n = Rio_readlineb(&rio2server, buf2server, MAXLINE)) != 0){
    	printf("Proxy received %ld bytes from server, and send to client\n", n);
    	//Rio_writen(fd, buf2server, strlen(buf2server) ); 
    	Rio_writen(fd, buf2server, n ); 
    	
    	cacheSize+= n;
    	strcat(buf, buf2server);
    	//sprintf(buf, "%s%s", buf, buf2server);
    	// p602, when next write to buf, last write will not be discard, so the return of Rio_readlineb is key
    }
    cacheWrite(buf, uri1, cacheSize);
    Close(serverfd);
}
/*
 * parse_uri - parse URI into server's host and port
 */
/* $begin parse_uri */
void parse_uri(char *uri, URL* head) 
{
    char *hostptr = strstr(uri, "//");
	if(hostptr == NULL){
		char *pathptr = index(uri, '/');
		/* if(pathptr != NULL){
			strcpy(head->path, pathptr);
		} */
		strcat(head->path, pathptr);
		strcpy(head->port, "80"); // default port is 80
		return;
	}
	else{
		char *portptr = index(hostptr+2, ':');
		char* idx = index(hostptr + 2, '/');
		if(portptr != NULL){
			int tmp;
			sscanf(portptr + 1, "%d%s", &tmp, head->path);
			sprintf(head->port, "%d", tmp);
			*portptr = '\0';
		}else {
            //example: GET http://www.cmu.edu/hub/index.html HTTP/1.1
            sscanf(idx, "%s", head->path);
            strcpy(head->port, "80");
            *idx = '\0';
        }
		strcpy(head->host, hostptr+2);
	}
	
}

/*
 * read_requesthdrs - read HTTP request headers
 */
/* $begin read_requesthdrs */
void build_header(rio_t *rp, char* serverbuf, URL* headers) 
{
	char buf[MAXLINE];
	sprintf(serverbuf, "GET %s HTTP/1.0\r\n", headers->path);
	sprintf(serverbuf, "%sHost: %s\r\n", serverbuf, headers->host);
    sprintf(serverbuf, "%sUser-Agent: %s", serverbuf, user_agent_hdr);
    sprintf(serverbuf, "%sConnection: close\r\n", serverbuf);
    sprintf(serverbuf, "%sProxy-Connection: close\r\n", serverbuf);
    Rio_readlineb(rp, buf, MAXLINE);
    
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
		if(!strncasecmp(buf, "Host", strlen("Host")) || !strncasecmp(buf, "User-Agent", strlen("User-Agent")) 
           || !strncasecmp(buf, "Connection", strlen("Connection")) 
           || !strncasecmp(buf, "Proxy-Connection", strlen("Proxy-Connection"))) {
                Rio_readlineb(rp, buf, MAXLINE);
                continue;
           }
        sprintf(serverbuf,"%s%s", serverbuf, buf); 
        Rio_readlineb(rp, buf, MAXLINE);
    }
    //printf("%s\n", serverbuf);
    sprintf(serverbuf, "%s\r\n", serverbuf);
    return;
}

/* Thread routine */
void *thread(void *vargp){
	int connfd = *((int*)vargp);
	Pthread_detach(Pthread_self());
	Free(vargp);
	doit(connfd);
	Close(connfd);
	return NULL;
}

/* init cache */
void cache_init(){
	cache.readcnt = 0;
	cache.curtime = 0;
	Sem_init(&cache.mutex, 0, 1);
	Sem_init(&cache.writer, 0, 1);
	for(int i = 0; i < CACHELINE; ++i){
		cache.line[i].valid = 0;
		cache.line[i].time = 0;
		cache.line[i].size = 0;
	}
}

/* return cacheindex if the url is in the cache 
 * return -1 if the url is not in the cache
 */
int getCacheIndex(char *url){
	int returnv = -1;
	for(int i = 0; i < CACHELINE; ++i){
		if(cache.line[i].valid && !strcmp(cache.line[i].url, url)){
			returnv = i;
			break;
		}
	}
	return returnv;
}

void cacheWrite(char *buf, char * url, int size){
	if(size > MAX_OBJECT_SIZE) return;
	int idx = -1;
	for(int i = 0; i < CACHELINE; ++i){
		if(cache.line[i].valid == 0){
			idx = i;
			break;
		}
	}
	if(idx == -1){
		int maxtime = 0;
		for(int i =0; i < CACHELINE; ++i){
			if(cache.line[i].valid && (cache.curtime - cache.line[i].time) > maxtime){
				idx = i;
				maxtime = cache.curtime - cache.line[i].time;
			}
		}
	}
	P(&cache.writer);
	strcpy(cache.line[idx].buf, buf);
	strcpy(cache.line[idx].url, url);
	cache.line[idx].size = size;
	cache.line[idx].time = ++cache.curtime;
	cache.line[idx].valid = 1;
	V(&cache.writer);
}









