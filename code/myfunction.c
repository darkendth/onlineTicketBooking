# include "csapp.h"
# include "myheader.h"

/* shared variable */
int normalc = 100;
int deluxec = 50;
int premiumc = 30;

/* Semaphore */
sem_t mutex1; // for normalc
sem_t mutex2; // for deluxec
sem_t mutex3; // for premiumc

void initialize_semaphore()
{
    sem_init(&mutex1,0,1);
    sem_init(&mutex2,0,1);
    sem_init(&mutex3,0,1);
}

void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    dojob(connfd);
    close(connfd);
    printf("connection closed!\n");
    return NULL;
}

void dojob(int fd)
{
    rio_t rio;
    char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
    int is_static;
    char filename[MAXLINE],cgiargs[MAXLINE];
    struct stat sb;
    rio_readinitb(&rio,fd);
    rio_readlineb(&rio,buf,MAXLINE);
    printf("request Headers: \n");
    printf("%s\n",buf);
    sscanf(buf,"%s %s %s",method,uri,version);

    if (strcasecmp(method,"GET"))
    {
        printf("Method Not implemented:");
        return ;
    }
    skip_requesthdrs(&rio);

    is_static = parse_uri(uri,filename,cgiargs);
    // printf("filename %s\n",filename);

    if (stat(filename,&sb) < 0)
    {
        printf("%s file not found!\n",filename);
        return ;
    }

    if (is_static)
    {
        if (!(S_ISREG(sb.st_mode)) || !(S_IRUSR & sb.st_mode))
        {
            printf("unable to read file!\n");
            return ;
        }
        serve_static(fd,filename,sb.st_size);
    }
    else
    {
        serve_dynamic(fd,filename,cgiargs);
    }
 }

void skip_requesthdrs(rio_t *rip)
{
    char buf[MAXLINE];
    rio_readlineb(rip,buf,sizeof(buf));
    while (strcmp(buf,"\r\n"))
    {
        rio_readlineb(rip,buf,MAXLINE);
        // printf("%s",buf);
    }
}

int parse_uri(char *uri,char *filename,char *cgiargs)
{
    if (!strstr(uri,"cgi-bin"))
    {
        strcpy(cgiargs,"");
        strcpy(filename,"home.html");
        return 1;
    }
    else
    {
        char *ptr = index(uri,'?');
        if (ptr)
        {
            strcpy(cgiargs,ptr+1);
            *ptr = '\0';
        }
        else    
            strcpy(cgiargs,"");
        // strcpy(filename,uri);
        strcpy(filename,"send_mail");
        return 0;
    }
}

void serve_static(int fd,char *filename,int fileSize)
{
    int sfd;
    char filetype[MAXLINE],buf[MAXBUF];
    char *srcp;

    get_filetype(filename,filetype);
    /* headers content */
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    sprintf(buf,"%sServer: Local Web Server\r\n",buf);
    sprintf(buf,"%sConnection: close\r\n",buf);
    sprintf(buf,"%sContent-type: %s\r\n\r\n",buf,filetype);
    rio_writen(fd,buf,strlen(buf));
    printf("Response Header : \n%s \n",buf);

    /* Body content */
    sfd = open(filename,O_RDONLY,0);
    srcp = mmap(0,fileSize,PROT_READ,MAP_PRIVATE,sfd,0);
    close(sfd);
    rio_writen(fd,srcp,fileSize);
    munmap(srcp,fileSize);
}

void get_filetype(char *filename,char *filetype)
{
    if (strstr(filename,".html"))
        strcpy(filetype,"text/html");
    else if (strstr(filename,".gif"))
        strcpy(filetype,"image/gif");
    else if (strstr(filename,".png"))
        strcpy(filetype,"image/png");
    else if (strstr(filename,".jpg"))
        strcpy(filetype,"image/jpeg");
    else
        strcpy(filetype,"text/plain");
}

void serve_dynamic(int fd,char *filename,char *args)
{
    char buf[MAXLINE];
    char *empt[] = {NULL};
    char *env[] = {NULL};
    char *p,ctg[50],notkt[50];
    int n,booked=0;
    /* Http header */
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    sprintf(buf,"%sServer: Local Web Server\r\n",buf);
    rio_writen(fd,buf,strlen(buf));
    
    /* parse parameters */
    strcpy(buf,args);
    p = strchr(buf,'&');
    *p = '\0';
    strcpy(notkt,p+1);

    p = strchr(notkt,'&');
    *p = '\0';
    strcpy(ctg,p+1);

    p = strchr(notkt,'=');
    *p = '\0';
    strcpy(notkt,p+1);

    p = strchr(ctg,'=');
    *p = '\0';
    strcpy(ctg,p+1);

    p = strchr(buf,'=');
    *p = '\0';
    strcpy(buf,p+1);
    if (strcmp(buf,"\0")==0)
    {
        printf("Invalid Email Address!\n");
        return ;
    }
    if (strcmp(notkt,"\0")==0)
    {
        printf("Invalid quantity requested.\n");
        return ;
    }
    n = atoi(notkt);
    if (strcmp(notkt,"\0")==0 || n<0)
    {
        printf("<%s>-Invalid quantity requested.\n",buf);
        return ;
    }

    if (strcmp(ctg,"normal")==0)
    {
        if (n<=normalc)
        {
            P(&mutex1);
            normalc -= n;
            V(&mutex1);
            booked = 1;
            printf("<%s>-The ticket booking was successful. Total tickets booked %d of normal.\n",buf,n);
        }
        else
        {
            printf("<%s>-No sufficient tickets are available in this category.\n",buf);
            return ;
        }
    }
    else if (strcmp(ctg,"deluxe")==0)
    {
        if (n<=deluxec)
        {
            P(&mutex2);
            deluxec -= n;
            V(&mutex2);
            booked = 1;
            printf("<%s>-The ticket booking was successful. Total tickets booked %d of deluxe.\n",buf,n);
        }
        else
        {
            printf("<%s>-No sufficient tickets are available in this category.\n",buf);
            return ;
        }
    }
    else if (strcmp(ctg,"premium")==0)
    {
        if (n<=premiumc)
        {
            P(&mutex3);
            premiumc -= n;
            V(&mutex3);
            booked = 1;
            printf("<%s>-The ticket booking was successful. Total tickets booked %d premium.\n",buf,n);
        }
        else
        {
            printf("<%s>-No sufficient tickets are available in this category.\n",buf);
            return ;
        }
    }
    else
    {
        printf("<%s>-Invalid Category.\n",buf);
        return ;
    }
    /* send mail */

    // if (fork() == 0)
    // {
    //     setenv("QUERY_STRING",args,1);
    //     dup2(fd,STDOUT_FILENO);
    //     execve(filename,empt,env);
    // }
    
    printf("ticket remaining : %d %d %d\n",normalc,deluxec,premiumc);
}