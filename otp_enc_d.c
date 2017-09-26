/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 80000


void encipher(int); // prototype


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
			encipher(newsockfd);
			exit(0);
		}
		else close(newsockfd);

	}

	close(sockfd);

	return 0;

} // end main





/*************encipher**************/


void encipher(int sock)
{
	int n, docsize;
	char tmp;
	char namebuff[BUF_SIZE];
	bzero(namebuff, BUF_SIZE);

	char plainbuff[BUF_SIZE];
	bzero(plainbuff, BUF_SIZE);

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

		
		plainbuff[buflen] = tmp;
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
char name[7] = {"otp_enc"};
if(namebuff[4] != 'e')
{
	error("ERROR: Connection Not Allowed!\n");
	return;
}



	char plainhalf;
	char keyhalf;
	char newKey;
	char encrypt[BUF_SIZE];
	char num[5];
	
	bzero(encrypt, BUF_SIZE);

	int i;
	for(i = 0; i < docsize; i++)
	{			
		plainhalf = plainbuff[i];		
		
		// make space char = 26, A-Z = 0-25	
		if(plainhalf == ' ')
		{
		    plainhalf = plainhalf - 6;
			
		}
		
		// subtract to get 0-25 range
		else
		{
			plainhalf = plainhalf - 64;
		}
		
		keyhalf = keybuff[i];
		if(keyhalf == ' ')
		{
		  	keyhalf  = keyhalf - 6;
			
		}
		
		else
		{
			keyhalf = keyhalf - 64;
		}

		
		newKey = ((plainhalf + keyhalf) % 26);
		
		// account for space char
		if(newKey == 0) 
		{
			newKey = 32;
		}	
	
		else
		{
			newKey = newKey + 64;
		}
		
		// write the new char to the string
		encrypt[i] = newKey;
		
		

	}


// send the encrypted info back to client
n = write(sock, encrypt, strlen(encrypt));
if(n < 0) error("ERROR writing to socket");




return;
} //end of function


