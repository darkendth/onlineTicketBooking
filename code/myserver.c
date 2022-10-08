# include "csapp.h"
# include "myheader.h"
# include "myvariable.h"

enum ticketCategory {Normal, Deluxe, Premium};
enum ticketCategory tkt;

void main(int argc,char **argv)
{
    int listenfd,*connfdp;
    int port;
    char hostname[MAXLINE],cport[MAXLINE];
    pthread_t tid;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if (argc != 2)
    {
        printf("usage : %s <port> \n",argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);
    
    listenfd = open_listenfd(port);
    if (listenfd == -1)
    {
        fprintf(stderr,"Error: Unable to listen! %d\n",listenfd);
        exit(1);
    }

    initialize_semaphore();
    while (1)
    {
        printf("listen ....\n");
        clientlen = sizeof(clientaddr);
        connfdp = malloc(sizeof(int));
        *connfdp = accept(listenfd,(SA *) &clientaddr,&clientlen);
        getnameinfo((SA *) &clientaddr,clientlen,hostname,MAXLINE,cport,MAXLINE,0);
        printf("Accepted connection from (%s, %s)\n",hostname,cport);
        pthread_create(&tid,NULL,thread, connfdp);
    }

}
