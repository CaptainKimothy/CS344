#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUF_SIZE 80000

/*******************
 * client1.c
 * ****************/


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //char *host = "localhost";
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost"); //localhost;//gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");








/************MY STUFF*****************/



	char newdel = ')';
	n = write(sockfd, &newdel, 1);
	if(n < 0) error("ERROR writing to socket");

	// send name to accept or block connection
	 n = write(sockfd, "otp_dec", 7);
	if(n < 0) error("ERROR writing to socket");

	char delim = '\x1';
	n = write(sockfd, &delim, 1);
	if(n < 0) error("ERROR writing to socket");

	// plaintext to transfer
        FILE *fp1 = fopen(argv[1],"r");
        if(fp1==NULL)
        {
            printf("File open error");
            return 1;
        }

	fseek(fp1, 0L, SEEK_END);
	int plainsize = ftell(fp1);
	fseek(fp1, 0L, SEEK_SET);

	//printf("plainsize: %i\n", plainsize);
	unsigned char plaintext[BUF_SIZE]={0};
	int nread = fread(plaintext,1,(plainsize - 1),fp1);
	//printf(plaintext);
	if(nread > 0)
        {
                //printf("Sending plaintext1\n");
                write(sockfd, plaintext, nread);
        }

        if (nread < plainsize)
        {
                if (feof(fp1))
                    printf("End of file\n");
                if (ferror(fp1))
                    printf("Error reading\n");
	}

	//printf("plaintext sent successfully\n");
	delim = '\x2';
	n = write(sockfd, &delim, 1);
	if(n < 0) error("ERROR writing to socket");
	

	// keytext to transfer
        FILE *fp2 = fopen(argv[2],"r");
        if(fp2==NULL)
        {
            printf("File open error");
            return 1;
        }

	fseek(fp2, 0L, SEEK_END);
	int keysize = ftell(fp2);
	fseek(fp2, 0L, SEEK_SET);


	// check that keytext is long enough
	if(keysize < plainsize)
	{
		printf("Key file is not long enough. Try again.\n");
		exit(1);
	}



	//printf("keysize: %i\n", keysize);
	unsigned char keytext[BUF_SIZE]={0};
	nread = fread(keytext,1,(keysize - 1),fp2);
	//printf(keytext);
	if(nread > 0)
        {
                //printf("Sending keytext\n");
                write(sockfd, keytext, nread);
        }

        if (nread < keysize)
        {
                if (feof(fp2))
                    printf("End of file\n");
                if (ferror(fp2))
                    printf("Error reading\n");
	}


	delim = '\x3';
	n = write(sockfd, &delim, 1);
	if(n < 0) error("ERROR writing to socket");

	//printf("All info sent. Waiting for response\n");

	char buff[BUF_SIZE];
    	//memset(buff, '0', sizeof(buff));
	bzero(buff, BUF_SIZE);
    	while((n = read(sockfd, buff, BUF_SIZE)) > 0)  /////////////////// plainsize - 1?
    	{
        //printf("Bytes received %d\n",n);
        //printf("client received: \n");
        //printf("%c\n", buff);   //////////////////////////%i


	printf(buff);
	printf("\n");

		//fwrite(buff, 1, bytesReceived,fp);
    	}


	//n = read(sockfd, buff, BUF_SIZE - 1);
	//if(n < 0) error("ERROR reading from socket");
//printf("%s\n", buff);






    close(sockfd);
    return 0;
}
















    /*
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    */


//////////////

/*
       // plaintext to transfer
        FILE *fp1 = fopen(argv[1],"r");
        if(fp1==NULL)
        {
            printf("File open error");
            return 1;
        }

	fseek(fp1, 0L, SEEK_END);
	int plainsize = ftell(fp1);
	fseek(fp1, 0L, SEEK_SET);

	//strtok(fp1, "\n");//////////////////////
	//plainsize--;
        // Read data from file and send it //
       // for (;;) //while(true)
        //{
            // First read file in chunks of BUF_SIZE bytes 
            unsigned char plaintext[BUF_SIZE]={0};
            int nread = fread(plaintext,1,BUF_SIZE - 1,fp1);
            printf("Bytes read %d \n", nread);
	    //strtok(plaintext, "\n");
            // If read was success, send data. 
            if(nread > 0)
            {
                printf("Sending plaintext1\n");
                write(sockfd, plaintext, nread);
            }

            if (nread < BUF_SIZE)
            {
                if (feof(fp1))
                    printf("End of file\n");
                if (ferror(fp1))
                    printf("Error reading\n");
                //break;
            }
	//}

*/

/*
   // Receive garbage so client can write again
    int garbageReceived = 0;
    char garbage[BUF_SIZE];
    memset(garbage, '0', sizeof(garbage));
    while((garbageReceived = read(sockfd, garbage, BUF_SIZE)) > 0)
    {
        //printf("Bytes received %d\n",bytesReceived);
        printf("client received: \n");
        printf(garbage);
	//fwrite(buff, 1,bytesReceived,fp);
    }
*/
/*
	//printf("before garbage\n");
	char garbage[BUF_SIZE];
	bzero(garbage, BUF_SIZE);
	//printf("middle garbage\n");
	if((n = read(sockfd, garbage, plainsize)) > 0); //BUF_SIZE - 1
	{printf("n = %i\n", n);}
	if(n < 0){ printf("Client: Error reading from socket\n");}
	printf("client received1: \n");
        printf("%s\n", garbage);
*/

//sleep(5);


/*
// First read file in chunks of BUF_SIZE bytes 
            unsigned char plaintext2[BUF_SIZE]={0};
            nread = fread(plaintext2,1,BUF_SIZE - 1,fp1);
            printf("Bytes read %d \n", nread);
	    //strtok(plaintext, "\n");
            // If read was success, send data. 
            if(nread > 0)
            {
                printf("Sending plaintext2\n");
                write(sockfd, plaintext2, nread);
            }

            if (nread < BUF_SIZE)
            {
                if (feof(fp1))
                    printf("End of file\n");
                if (ferror(fp1))
                    printf("Error reading\n");
                //break;
            }
*/
/*
printf("Sending plaintext2\n");
                write(sockfd, plaintext, nread);


//printf("before garbage\n");
	//char garbage[BUF_SIZE];
	bzero(garbage, BUF_SIZE);
	//printf("middle garbage\n");
	if((n = read(sockfd, garbage, plainsize)) > 0); //BUF_SIZE - 1
	{printf("n = %i\n", n);}
	if(n < 0){ printf("Client: Error reading from socket\n");}
	printf("client received 2: \n");
        printf("%s\n", garbage);

*/


/*

	// keytext to transfer
        FILE *fp2 = fopen(argv[2],"r");
        if(fp2==NULL)
        {
            printf("File open error");
            return 1;
        }

	fseek(fp2, 0L, SEEK_END);
	int keysize = ftell(fp2);
	fseek(fp2, 0L, SEEK_SET);



	//strtok(fp1, "\n");//////////////////////
	//plainsize--;
        / Read data from file and send it 
       // for (;;) //while(true)
        //{
            // First read file in chunks of BUF_SIZE bytes 
            unsigned char keytext[BUF_SIZE]={0};
            nread = fread(keytext,1,BUF_SIZE - 1,fp2);
            printf("Bytes read %d \n", nread);
	    //strtok(plaintext, "\n");
            // If read was success, send data. 
            if(nread > 0)
            {
                printf("Sending keytext\n");
                write(sockfd, keytext, nread);
            }

            if (nread < BUF_SIZE)
            {
                if (feof(fp2))
                    printf("End of file\n");
                if (ferror(fp2))
                    printf("Error reading\n");
                //break;
            }
	//}
*/
//printf("%s\n", keytext);


/*	
	char key[BUF_SIZE];

	bzero(key, BUF_SIZE);
	//printf("middle garbage\n");
	if((n = read(sockfd, key, BUF_SIZE - 1)) > 0); //BUF_SIZE - 1
	{printf("n = %i\n", n);}
	if(n < 0){ printf("Client: Error reading from socket\n");}
	printf("client received: \n");
	int k;
	printf(key);
	//for(k = 0; k < plainsize; k++)
	//{
       		//printf(key[k]);
	//}
	//printf("\n");
*/


/*
	char key[BUF_SIZE];
	bzero(key, BUF_SIZE);
	n = read(sockfd, key, (BUF_SIZE - 1));
	printf("client received: \n");
printf(key);
*/
/*
	char *got;
int m = 0;
memset(key, 0, BUF_SIZE);
strcpy(got, "");
n = plainsize;
while(n >= plainsize){
	n = recv(sockfd, key, plainsize, 0);
	m += n;
	if(n < 0){
	perror("socket read");
	}
	if(n > 0){
	strcat(got, key);
	memset(got, 0, BUF_SIZE);
	}
}

*/




 
