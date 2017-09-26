#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 80000


void decipher(int);


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{ 
     // the base of this code is from server.c from the class site
	
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

	// enable multiple connections 
	while(1){
		newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);	
     		if (newsockfd < 0) 
          		error("ERROR on accept");


		pid = fork();
		if (pid < 0)
			error("ERROR on fork");
		if (pid == 0)
		{	
			close(sockfd);
			decipher(newsockfd);
			exit(0);
		}
		else close(newsockfd);

	}

	close(sockfd);

	return 0;

} // end main





/*************decipher**************/


void decipher(int sock)
{
	int n, docsize;
	char tmp;
	char namebuff[BUF_SIZE];
	bzero(namebuff, BUF_SIZE);

	char cipherbuff[BUF_SIZE];
	bzero(cipherbuff, BUF_SIZE);

	char keybuff[BUF_SIZE];
	bzero(keybuff, BUF_SIZE);

	int buflen = 0;


do{
	
	n = read(sock, &tmp, 1);
	if (n < 0) error("ERROR reading from socket");

	if(tmp != ')')
		continue;
	

	// read the name to okay connection
	do{

		n = read(sock, &tmp, 1);
		if (n < 0) error("ERROR reading from socket");
		

		if(tmp == '\x1')
			break;

		
		namebuff[buflen] = tmp;
		++buflen;


	}while(1);



	

	// reset buflen and get plaintext information
	buflen = 0;


	do{
		n = read(sock, &tmp, 1);
		if (n < 0) error("ERROR reading from socket");
		
		if(tmp == '\x2')
			break;

		cipherbuff[buflen] = tmp;
		++buflen;


	}while(1);

	docsize = buflen;

	// reset buflen and get key information
	buflen = 0;

	do{
		n = read(sock, &tmp, 1);
		if (n < 0) error("ERROR reading from socket");
		

		if(tmp == '\x3')
			break;

		
		keybuff[buflen] = tmp;
		++buflen;


	}while(1);

	
	break;

}while(1);





// if not allowed to connect, disconnect
if(namebuff[4] != 'd')
{	
	error("ERROR: Connection Not Allowed!\n");
	return;
}


	// create chars for calculations
	char cipherhalf;
	char keyhalf;
	char newKey;;
	char decrypt[BUF_SIZE];
	char num;
	
	bzero(decrypt, BUF_SIZE);

	int i;
	for(i = 0; i < docsize; i++)
	{	
		cipherhalf = cipherbuff[i];		
			
		if(cipherhalf == ' ')
		{
		    cipherhalf = 0;
		}
		
		else
		{
			cipherhalf = cipherhalf - 64;
		}
		
		keyhalf = keybuff[i];
		if(keyhalf == ' ')
		{
		  	keyhalf  = keyhalf - 6;
			//printf(keyhalf);
		}
		
		else
		{
			keyhalf = keyhalf - 64;
		}

		// if new number is negative, wrap around alphabet
		num = (cipherhalf - keyhalf);
		if(num < 0)
			num = (26 + num);
		
		newKey = (num % 26);
		
		// if 0, make space
		if(newKey == 0) 
		{
			newKey = 32;
		}	
	
		else
		{
			newKey = newKey + 64;
		}
		
		// add new char to string
		decrypt[i] = newKey;
		
		

	}




// send the decrypted message back to the client
n = write(sock, decrypt, strlen(decrypt));
if(n < 0) error("ERROR writing to socket");









return;
} //end of function






