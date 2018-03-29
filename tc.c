/*By Travis Bates and Jonathan Cardasis*/

#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_CARS 8

const int DELTA_LEFT = 3, DELTA_STRAIGHT = 2, DELTA_RIGHT = 1;
struct timeGlobal;

typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;

typedef struct _car {
	int cid;
	double arrival_time;
	directions *dir;
} car;

void PrintTime() {
	/*TODO: IMPLEMENT TIMER... USE GLOBAL TIMER SET IN MAIN TO DETERMINE CURRENT TIME*/
	/*PROFESSOR OKAY'ED ROUNDING TIME TO NEAREST TENTH OF SECOND TO MATCH PROGRAM OUTPUT*/
	/*MIGHT SEE SLIGHT TIME DELAY IF NOT ROUNDING TO NEAREST TENTH.*/
	printf("Time %ld: ", timeGlobal);
}

void PrintCar(car *car) {
	printf("Car %d (->%c ->%c) ", car->cid, car->dir->dir_original, car->dir->dir_target);
}

void PrintOutput(char *str, car *car) {
	/*TODO: IMPLEMENT TIMER... USE GLOBAL TIMER SET IN MAIN TO DETERMINE CURRENT TIME*/
	/*PROFESSOR OKAY'ED ROUNDING TIME TO NEAREST TENTH OF SECOND TO MATCH PROGRAM OUTPUT*/
	/*MIGHT SEE SLIGHT TIME DELAY IF NOT ROUNDING TO NEAREST TENTH.*/
	printf("Time %ld: ", 1);
	printf("Car %d (->%c ->%c) ", car->cid, car->dir->dir_original, car->dir->dir_target);
	printf(str);
	printf("\n");
}

void ArriveIntersection(car *car) {
	/*usleep until desired arrival time*/

	PrintOutput("arriving", car);

	/*do logic here*/
}

void CrossIntersection(car *car) {
	PrintOutput("crossing", car);

	/*spin while crossing*/
}

void ExitIntersection(car *car) {
	PrintOutput("exiting", car);
}

void *Car(void* arg) {
	car *threadcar = (car*)arg;

	ArriveIntersection(threadcar);
	CrossIntersection(threadcar);
	ExitIntersection(threadcar);
}

car* GetCars() {
	car *carArray = malloc(NUM_CARS * sizeof(car));

	for (int i = 0; i < NUM_CARS; i++)
		carArray[i].dir = malloc(sizeof(directions));

	carArray[0].cid = 0;
	carArray[1].cid = 1;
	carArray[2].cid = 2;
	carArray[3].cid = 3;
	carArray[4].cid = 4;
	carArray[5].cid = 5;
	carArray[6].cid = 6;
	carArray[7].cid = 7;

	carArray[0].arrival_time = 1.1;
	carArray[1].arrival_time = 2.0;
	carArray[2].arrival_time = 3.3;
	carArray[3].arrival_time = 3.5;
	carArray[4].arrival_time = 4.2;
	carArray[5].arrival_time = 4.4;
	carArray[6].arrival_time = 5.7;
	carArray[7].arrival_time = 5.9;

	carArray[0].dir->dir_original = 'N';
	carArray[1].dir->dir_original = 'N';
	carArray[2].dir->dir_original = 'N';
	carArray[3].dir->dir_original = 'S';
	carArray[4].dir->dir_original = 'S';
	carArray[5].dir->dir_original = 'N';
	carArray[6].dir->dir_original = 'E';
	carArray[7].dir->dir_original = 'W';

	carArray[0].dir->dir_target = 'N';
	carArray[1].dir->dir_target = 'N';
	carArray[2].dir->dir_target = 'W';
	carArray[3].dir->dir_target = 'S';
	carArray[4].dir->dir_target = 'E';
	carArray[5].dir->dir_target = 'N';
	carArray[6].dir->dir_target = 'N';
	carArray[7].dir->dir_target = 'N';

	return carArray;
}

void freeCars(car* carArray) {
	for (int i = 0; i < NUM_CARS; i++)
		free(carArray[i].dir);

	free(carArray);
}

void main() {
	/*Setup and print initial car array*/
	car* cars = GetCars();

	printf("Initial car array:\n");
	
	for (int i = 0; i < NUM_CARS; i++)
		printf("dir 1: %c dir 2: %c\n", cars[i].dir->dir_original, cars[i].dir->dir_target);

	/*Start global timer*/
	printf("Starting timer...\n");
	timeGlobal = time(NULL);

	/*Create and launch threads*/
	printf("Creating threads...\n");
	pthread_t threads[NUM_CARS];
	for (int i = 0; i < NUM_CARS; i++) {
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&threads[i], &attr, Car, &cars[i]);
	}

	/*Join all threads*/
	for (int i = 0; i < NUM_CARS; i++)
		pthread_join(threads[i], NULL);

	/*Program is done, free all memory*/
	freeCars(cars);
 
   	return 0;
}