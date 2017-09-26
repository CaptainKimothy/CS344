#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUF_SIZE 80000


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
        char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost"); 
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



	// create delimiters for server to catch when parsing data
	// enables us to separate files
	char newdel = ')';
	n = write(sockfd, &newdel, 1);
	if(n < 0) error("ERROR writing to socket");

	// send name to accept or block connection
	 n = write(sockfd, "otp_enc", 7);
	if(n < 0) error("ERROR writing to socket");

	// add delimiter
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

	// get size of plaintext
	fseek(fp1, 0L, SEEK_END);
	int plainsize = ftell(fp1);
	fseek(fp1, 0L, SEEK_SET);

	// send data to server
	unsigned char plaintext[BUF_SIZE]={0};
	int nread = fread(plaintext,1,(plainsize - 1),fp1);

	// screen for bad files
	if(plaintext[0] == '$')
	{	
		error("ERROR: Invalid Characters\n");
		return 1;	
	}
	
	if(nread > 0)
        {
                
                write(sockfd, plaintext, nread);
        }

        if (nread < plainsize)
        {
                if (feof(fp1))
                    printf("End of file\n");
                if (ferror(fp1))
                    printf("Error reading\n");
	}

	// add delimiter
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

	// get size of key text
	fseek(fp2, 0L, SEEK_END);
	int keysize = ftell(fp2);
	fseek(fp2, 0L, SEEK_SET);


	// check that keytext is long enough
	if(keysize < plainsize)
	{
		printf("Key file is not long enough. Try again.\n");
		exit(1);
	}



	// send key to server
	unsigned char keytext[BUF_SIZE]={0};
	nread = fread(keytext,1,(keysize - 1),fp2);
	
	if(nread > 0)
        {
                write(sockfd, keytext, nread);
        }

        if (nread < keysize)
        {
                if (feof(fp2))
                    printf("End of file\n");
                if (ferror(fp2))
                    printf("Error reading\n");
	}

	// add delimiter
	delim = '\x3';
	n = write(sockfd, &delim, 1);
	if(n < 0) error("ERROR writing to socket");

	
	// create buffer to receive data
	char buff[BUF_SIZE];
	bzero(buff, BUF_SIZE);
    	while((n = read(sockfd, buff, BUF_SIZE)) > 0)  /////////////////// plainsize - 1?
    	{
        	printf(buff);
		// add newline for files
		printf("\n");

    	}



    close(sockfd);
    return 0;
}










