#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
 


int main(int argc, char** argv)
{

// seed the random function
srand(time(NULL));
	


// create an array to choose random chars from
char alpha_array[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' ,'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '}; 



	// get the length of the key from the arguments array
	long keyLen;
	keyLen = strtol(argv[1], NULL, 10);
	
	// create an array to hold the random characters
	char *key_array = malloc(keyLen * sizeof(char));

	// fill the array
	int i;
	for (i = 0; i < keyLen; i++)
	{	key_array[i] = alpha_array[rand() % 27]; 		
		
	}
	
	
	// print array plus newline for file and delete array
	printf(key_array); //%s
	printf("\n");
	free(key_array);



}
















