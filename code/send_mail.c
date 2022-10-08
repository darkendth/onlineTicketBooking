# include "csapp.h"
# define smtp_domain_name "smtpout.asia.secureserver.net"
# define UID ""
# define PWD ""
# define domain "smtp.gmail.com"

void main()
{
    int clientfd,sd;
    char buf[MAXLINE],*encd;
    rio_t rio;
    clientfd = open_clientfd(smtp_domain_name,25);
    if (clientfd == -1)
    {
        printf("unable to connect.\n");
        exit(1);
    }
    // rio_readinitb(&rio,clientfd);
    // rio_readlineb(&rio,buf,MAXLINE);
    recv(clientfd,buf,sizeof(buf),0);
    printf("%s\n",buf);

    printf("EHLO:\n");
    strcpy(buf,"EHLO ");
    strcat(buf,domain);
    strcat(buf,"\r\n");
    send(clientfd,buf,strlen(buf),0);
    recv(clientfd,buf,sizeof(buf),0);
    printf("%s\n",buf);
    printf("out\n");

    /* Now send Auth details in base64 encoded */
    printf("auth \n");
    strcpy(buf,"AUTH LOGIN\r\n");
    send(clientfd,buf,strlen(buf),0);
    recv(clientfd,buf,sizeof(buf),0);
    printf("%s\n",buf);
    

    encd = base64_encode(UID,strlen(UID),NULL);
    strcpy(buf,encd);
    strcat(buf,"\r\n");
    send(clientfd,buf,strlen(buf),0);
    recv(clientfd,buf,sizeof(buf),0);
    printf("%s\n",buf);
    free(encd);

    encd = base64_encode(PWD,strlen(PWD),NULL);
    strcpy(buf,encd);
    strcat(buf,"\r\n");
    send(clientfd,buf,strlen(buf),0);
    recv(clientfd,buf,sizeof(buf),0);
    printf("%s\n",buf);
    free(encd);

    // /* Now send the mail details */
    // printf("Mail from : \n");
    // strcpy(buf,"MAIL FROM: ");
    // strcat(buf,"<darkendths66@gmail.com>\r\n");
    // rio_writen(clientfd,buf,sizeof(buf));
    // rio_readlineb(&rio,buf,MAXLINE);
    // printf("%s\n",buf);

    // strcpy(buf,"RCPT TO: ");
    // strcat(buf,"<2021AIM1003@iitrpr.ac.in>\r\n");
    // rio_writen(clientfd,buf,sizeof(buf));
    // rio_readlineb(&rio,buf,MAXLINE);
    // printf("%s\n",buf);

    // /* 
    //     DATA command return 354 code.
    //     send email after receiving 354 code.
    // */
    // printf("data\n");
    // strcpy(buf,"DATA\r\n");
    // rio_writen(clientfd,buf,sizeof(buf));
    // rio_readlineb(&rio,buf,MAXLINE);
    // printf("%s\n",buf);

    close(clientfd);
}

