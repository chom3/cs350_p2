#include <stdio.h>
#include "time_functions.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>


using namespace std;
//global vars
const char *buffer[9];
sem_t s;
sem_t e;
sem_t n;
FILE *iFP, *oFP;
char input[256];
//compile with g++ HomCoreyP2.cpp -lpthread

void *readFunc(void *arg);
void *writeFunc(void *arg);


int main(int argc, const char* argv[]){
	//initialize the semaphores
	sem_init(&s, 0, 1);
	sem_init(&e, 0, 10);
	sem_init(&n, 0, 0);

	//we use output/input [256] to take in the max size for an input in scan
	printf("Name of an input file? \n");
	scanf("%s", input);

	//our first thread identifier
	//it's function will be to read a line of the file into the buffer...
	pthread_t first_thread, sec_thread;

	//Starts timing using the start_timing() function
	start_timing();

	pthread_create(&first_thread, NULL, readFunc, NULL);
	pthread_create(&sec_thread, NULL, writeFunc, NULL);
	pthread_join(first_thread, NULL);
	pthread_join(sec_thread, NULL);

	stop_timing();
	printf("Wall clock time difference is %10.3f seconds \n ", get_wall_clock_diff());
	printf("CPU time difference is %10.3f seconds\n", get_CPU_time_diff());
	// cout << "did this happen" << endl;
	string leaveOpen;
	cout << "Enter a key to continue." << endl;
	cin >> leaveOpen;
	return 0;
}

// the consumer
// (1)	reads a line of the file into a “buffer” (a “circular” array with fixed size = 10 slots).
// Each subscript element of the buffer will hold one “line” of the file.
// (2)	when “end of file” is detected, puts a “special code” in the buffer
// (3)	continues reading until the buffer is full or waits for an indication that there is space in the buffer,
// then continues reading
void *readFunc(void *arg)
{

	FILE *iFP = fopen(input, "r");
	FILE *help = fopen("helper.txt", "w");
	char buf[1025];
	int i = 0;
	bool loopCheck = true;
	while (loopCheck)
	{
		sem_wait(&e);
		sem_wait(&s);
		//reads a line into a buffer
		if (fgets(buf, 102, iFP) != NULL)
		{
			//printf("%s", buf);
			char line[1024];
			memcpy(line, buf, 1024);
			buffer[i] = line;
			fwrite(buffer[i], sizeof(char), 1024, help);
			// cout << "read: " << i << endl;
			i = (i + 1) % 10;
		}
		//how we'll put in the special code
		else if (fgets(buf, 1024, iFP) == NULL)
		{
			buffer[i] = "!END OF FILE!";
			loopCheck = false;
		}
		sem_post(&s);
		sem_post(&n);
	}
	fclose(iFP);
	return 0;
}

//the producer
// (1)	gets a line from the buffer and writes the line to a 2nd file
// (2)	exits when the special code is in the buffer.
// (3)	repeats the previous steps (a, b) as long as there is data in the buffer, waits if the buffer is empty
void *writeFunc(void *arg)
{
	//we generate some unspecified output file
	FILE *oFP = fopen("output.txt", "w");
	int i = 0;
	bool loopCheck = true;
	while (loopCheck)
	{
		sem_wait(&n);
		sem_wait(&s);
		//write one line from the buffer into the output file
		//fwrite(buffer[i], sizeof(char), 1024, oFP);
		char line[1024];
		memcpy(line, buffer[i], 1024);
		fwrite(line, sizeof(char), 1024, oFP);
		i = (i + 1) % 10;
		// cout << "write: " << i << endl;
		if (buffer[i] == "!END OF FILE!")
		{
			// cout << "reached?" << endl;
			loopCheck = false;
		}
		sem_post(&s);
		sem_post(&e);
	}
	fclose(oFP);
	return 0;
}
