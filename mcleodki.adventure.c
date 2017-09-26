/* 
 * File:   adventure.c
 * Author: Kim McLeod
 *
 * Created on October 29, 2015, 4:07 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>





struct ROOM
{
	char* roomName;
	int connections;
	char* roomType;
	char* roomConnex[10];
};

// function prototypes
void getStart();
void showConnections(int roomPlace);
char* getType(char* userInput);
void getName(char* userInput);
int checkChosenRoom(char* userInput);


//global variables! didn't have enough time to work without these :(
char* canGo[30];
FILE* openFileArray[7];
int lines[10];
char field1[30];
char field2[30];
char field3[30];
int place;
int numLines;
char* current;
char* type;
int connections = 0;



void main() {
    
//create new directory
int pid = getpid();
char user[] = "./mcleodki.rooms.";
char dirName[30];
sprintf(dirName, "%s%d", user, pid);
mkdir(dirName, 0777);

//change to new directory
chdir(dirName);	

  
 // seed clock to get truly random numbers   
 srand(time(NULL));
 

    
 // create room names
 char* namesArray[] = {"Dungeon", "Lockers", "Pool", "Gym", "Bedroom", "Hallway", "Tennis-Courts", "Greenhouse", "Servants-Room", "Garage"};   
     

   
// loop to open files and enter room names
int i;
FILE* roomArray[7]; // holds file names
int numPickArray[] = {99, 99, 99, 99, 99, 99, 99};



// assign room names, randomly!
char* newNamesArray[7];
int nameNum;
char* nameAssign;
for(i = 0; i < 7; i++)
{
    char buffer[32]; //filename buffer
    snprintf(buffer, sizeof(char) * 32, "room%i.txt", i);
    
    FILE* openRoom = fopen(buffer, "w+");
    roomArray[i] = openRoom;
    
    // get a random number for room name assignment
    // make sure that number hasn't been used already
    bool okay = false;
    int j;
        while((okay == false) || (j < sizeof(numPickArray) / 4))
        {        
            
            nameNum = rand() % 9;	
            
            
            for(j = 0; j < (sizeof(numPickArray) / 4); j++)
            {    

                if(numPickArray[j] == nameNum)
                { break; }                
            }
	    okay = true;
            
        }  

    	// record which rooms have been used already
    	numPickArray[i] = nameNum;

     	// assign the room a name
	nameAssign = namesArray[nameNum];


    	// create an array with names only
    	// to reference when adding room connections
	newNamesArray[i] = namesArray[nameNum];
	fprintf(openRoom, "ROOM NAME: %s\n", nameAssign);


    	// close file
	fclose(openRoom);
   


}



// Create structs for each room
struct ROOM *room0struct = malloc(sizeof(struct ROOM));
struct ROOM *room1struct = malloc(sizeof(struct ROOM));
struct ROOM *room2struct = malloc(sizeof(struct ROOM));
struct ROOM *room3struct = malloc(sizeof(struct ROOM));
struct ROOM *room4struct = malloc(sizeof(struct ROOM));
struct ROOM *room5struct = malloc(sizeof(struct ROOM));
struct ROOM *room6struct = malloc(sizeof(struct ROOM));




// create roomstruct array
struct ROOM roomStructs[7] = {*room0struct, *room1struct, *room2struct, *room3struct, *room4struct, *room5struct, *room6struct}; 




// initialize room structs! For Science! ... I think I'm starting to go crazy
int n;
for(n = 0; n < 7; n++)
{
	roomStructs[n].roomName = newNamesArray[n];	
	roomStructs[n].connections = 0;
	roomStructs[n].roomType = "x";
}





// connect all rooms together, randomly! 
int p;
int connexNum;
for(p = 0; p < 7; p++)
{

	while(roomStructs[p].connections < 3)
	{
		// make sure there isn't already a connection between the rooms
		connexNum = rand() % 6; 
		

		// if the room has 1 or more connections, 
		// check for pre-existing connection
		if(roomStructs[p].connections > 0)
		{
    		bool connect = false;
    		int a = 0;   



		// loop until end or connections or until connection is found
        	while((connect == false) || (a < roomStructs[p].connections)) 
        	{         
			          


            		for(a = 0; a < roomStructs[p].connections; a++)
            		{    	
             			if(roomStructs[p].roomConnex[a] == roomStructs[connexNum].roomName)
            			{ connexNum = rand() %6; break; }              
        			
			}

			
			connect = true;
            		
      		}




		}
		// if all requirements are met, make connection
		if((roomStructs[p].roomName != roomStructs[connexNum].roomName) && (roomStructs[connexNum].connections < 6))
		{
			
			roomStructs[p].roomConnex[roomStructs[p].connections] = roomStructs[connexNum].roomName;
			roomStructs[p].connections = roomStructs[p].connections + 1;
			
			roomStructs[connexNum].roomConnex[roomStructs[connexNum].connections] = roomStructs[p].roomName;	       roomStructs[connexNum].connections = roomStructs[connexNum].connections + 1;
			
		}
			
	}
	

}




// open all files for reading and appending
FILE* room0file = fopen("room0.txt", "a+");  
FILE* room1file = fopen("room1.txt", "a+");
FILE* room2file = fopen("room2.txt", "a+");
FILE* room3file = fopen("room3.txt", "a+");
FILE* room4file = fopen("room4.txt", "a+");
FILE* room5file = fopen("room5.txt", "a+");
FILE* room6file = fopen("room6.txt", "a+");





// randomly assign start and end rooms
int startRoom = rand() % 6;
roomStructs[startRoom].roomType = "START_ROOM";
int endRoom = rand() % 6;

while(endRoom == startRoom)
{ endRoom = rand() % 6; }

roomStructs[endRoom].roomType = "END_ROOM";	

int y;
for(y = 0; y < 7; y++)
{
	if(roomStructs[y].roomType == "x")
	{ roomStructs[y].roomType = "MID_ROOM"; }
}





// write connections and room types to appropriate rooms
int w; 
for(w = 0; w < roomStructs[0].connections; w++)
{ fprintf(room0file, "CONNECTION %i: %s\n", (w+1), roomStructs[0].roomConnex[w]); }
fprintf(room0file, "ROOM TYPE: %s\n", roomStructs[0].roomType);

for(w = 0; w < roomStructs[1].connections; w++)
{ fprintf(room1file, "CONNECTION %i: %s\n", (w+1), roomStructs[1].roomConnex[w]); }
fprintf(room1file, "ROOM TYPE: %s\n", roomStructs[1].roomType);


for(w = 0; w < roomStructs[2].connections; w++)
{ fprintf(room2file, "CONNECTION %i: %s\n", (w+1), roomStructs[2].roomConnex[w]); }
fprintf(room2file, "ROOM TYPE: %s\n", roomStructs[2].roomType);


for(w = 0; w < roomStructs[3].connections; w++)
{ fprintf(room3file, "CONNECTION %i: %s\n", (w+1), roomStructs[3].roomConnex[w]); }
fprintf(room3file, "ROOM TYPE: %s\n", roomStructs[3].roomType);


for(w = 0; w < roomStructs[4].connections; w++)
{ fprintf(room4file, "CONNECTION %i: %s\n", (w+1), roomStructs[4].roomConnex[w]); }
fprintf(room4file, "ROOM TYPE: %s\n", roomStructs[4].roomType);


for(w = 0; w < roomStructs[5].connections; w++)
{ fprintf(room5file, "CONNECTION %i: %s\n", (w+1), roomStructs[5].roomConnex[w]); }
fprintf(room5file, "ROOM TYPE: %s\n", roomStructs[5].roomType);


for(w = 0; w < roomStructs[6].connections; w++)
{ fprintf(room6file, "CONNECTION %i: %s\n", (w+1), roomStructs[6].roomConnex[w]); }
fprintf(room6file, "ROOM TYPE: %s\n", roomStructs[6].roomType);

// close files to open again for read only
fclose(room0file);
fclose(room1file);
fclose(room2file);
fclose(room3file);
fclose(room4file);
fclose(room5file);
fclose(room6file);




// open all files for reading
FILE* game0file = fopen("room0.txt", "r");  
FILE* game1file = fopen("room1.txt", "r");
FILE* game2file = fopen("room2.txt", "r");
FILE* game3file = fopen("room3.txt", "r");
FILE* game4file = fopen("room4.txt", "r");
FILE* game5file = fopen("room5.txt", "r");
FILE* game6file = fopen("room6.txt", "r");



// put open room files into array to access throughout game
openFileArray[0] = game0file;  
openFileArray[1] = game1file;
openFileArray[2] = game2file;
openFileArray[3] = game3file;
openFileArray[4] = game4file;
openFileArray[5] = game5file;
openFileArray[6] = game6file;



// output welcome message
printf("    Welcome to the Castle!\n");
printf("    Find your way to the end by exploring rooms.\n");
printf("\n    Good Luck!\n\n");



// find start room and initialize some variable
getStart();
int size = 50;
int typeEqualTest;
char input[100];




// allocate that sweet sweet memory
char **path;
path = malloc(sizeof(char*)*size);
int pathCount = 0;

int l;
for(l = 0; l < size; l++)
{
	path[l]=malloc(sizeof(char)*50);
}



// loop until user reaches end room
do{
	// get user input
	printf("WHERE TO?>");
	fgets(input, 100, stdin);
	printf("\n");
	// strip newline from input
	int ln;

	ln = strlen(input) - 1;

	if(input[ln] == '\n')
	{input[ln] = '\0'; }



	// loop until user enters correct input
	int matching = checkChosenRoom(input);
	while(matching == 0)
	{
		printf("\n");
		printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.");
		fgets(input, 100, stdin);
		ln = strlen(input) - 1;
		if(input[ln] == '\n')
		{ 
 		input[ln] = '\0'; }
		matching = checkChosenRoom(input);
	}	

	// update the user's path (I like to include the endpoint)
	strcpy(path[pathCount], input);
	pathCount++;

	//get type
	type = getType(input);

	// see if types are equal
	typeEqualTest = strcmp(type, "END_ROOM");

	// print new location and connections	
	getName(input);


} // do
while(typeEqualTest != 0);

printf("\n YOU REACHED THE END! GOOD JOB BUDDY!");
printf("\n YOU TOOK %i STEPS. ", pathCount);
printf("YOUR PATH TO VICTORY WAS:\n");



// print out user path
int f;
for(f = 0; f < pathCount; f++)
{	
	printf("%s\n", path[f]);
}



// close all files
fclose(game0file);
fclose(game1file);
fclose(game2file);
fclose(game3file);
fclose(game4file);
fclose(game5file);
fclose(game6file);




//deallocate memory    
free(path);   
    

}//main





// get current room information
void getStart(){
int g;
for(g = 0; g < 7; g++)
{	
	numLines = 0;
	//rewind(openFileArray[g]);
	while(fscanf(openFileArray[g], "%s %s %s", field1, field2, field3)!= EOF)
	{
		numLines++;	

		if(strcmp(field3, "START_ROOM") == 0)
		{ 
			place = g;
			rewind(openFileArray[g]);
			fscanf(openFileArray[g], "%s %s %s", field1, field2, field3);
			current = field3;
			
			printf("\n\nCURRENT LOCATION: %s\n", current);
			fseek(openFileArray[g], 0, SEEK_END);
		}
	}
	// record number of lines per file
	lines[g] = numLines;
	
	// rewind file
	rewind(openFileArray[g]);
	
	//pre-read first line for next step
	fscanf(openFileArray[g], "%s %s %s", field1, field2, field3);
}
	showConnections(place);

} 






// show user where they can go
void showConnections(int roomPlace)
{
printf("POSSIBLE CONNECTIONS: ");
connections = 0;
char roomConnect[30];
int v;
for(v = 0; v < lines[v] - 3; v++) //lines[v] - 4
{
	fscanf(openFileArray[roomPlace], "%s %s %s", field1, field2, field3);
	printf("%s, ", field3);
	connections++;
}

	fscanf(openFileArray[roomPlace], "%s %s %s", field1, field2, field3);
	printf("%s.\n", field3);
	connections++;

}







// check if user input matches available rooms
int checkChosenRoom(char* userInput)
{	
	// rewind and read first line of file to jump to connections
	rewind(openFileArray[place]);
	fscanf(openFileArray[place], "%s %s %s", field1, field2, field3);
int e;
for(e = 0; e < connections; e++)
{
	fscanf(openFileArray[place], "%s %s %s", field1, field2, field3);
	if(strcmp(field3, userInput) == 0)
	{ return 1;}
 
}
	return 0;
}







// find corresponding file and return room type
char* getType(char* userInput)
{
	int t;
	for(t = 0; t < 7; t++)
	{
		rewind(openFileArray[t]);
		fscanf(openFileArray[t], "%s %s %s", field1, field2, field3);
	
		if(strcmp(field3, userInput) == 0)
		{ 
			fseek(openFileArray[t], -9, SEEK_END);
			fscanf(openFileArray[t], "%s", field1);
			type = field1;	
			return type;
		}
	}
}







// change current location with user input
void getName(char* userInput)
{
	int t;
	for(t = 0; t < 7; t++)
	{
		rewind(openFileArray[t]);
		fscanf(openFileArray[t], "%s %s %s", field1, field2, field3);
	
		if(strcmp(field3, userInput) == 0)
		{ 
			printf("CURRENT LOCATION: %s\n", field3);
			place = t;
			showConnections(place);
		}
	}
}


