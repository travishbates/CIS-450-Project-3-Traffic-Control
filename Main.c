/*By Travis Bates and Jonathan Cardasis*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_CARS 8

const int DELTA_LEFT = 3, DELTA_STRAIGHT = 2, DELTA_RIGHT = 1;
const double DELTA_TICK = 0.1;

double intersectionClock = 0.0;

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
	printf("Time %d: ", intersectionClock);
}

void PrintCar(directions *dir) {
	printf("Car %d (->%c ->%c) ", dir->dir_original, dir->dir_target);
}

void PrintOutput(char *str, directions *dir) {
	PrintTime();
	PrintCar(dir);
	printf(str);
}

void ArriveIntersection(directions *dir) {
	PrintOutput("arriving", dir);

	/*do logic here*/
}

void CrossIntersection(directions *dir) {
	PrintOutput("crossing", dir);
}

void ExitIntersection(directions *dir) {
	PrintOutput("exiting", dir);
}

void *Car(void* arg) {
	car *threadcar = (car*)arg;

	ArriveIntersection(threadcar->dir);
	CrossIntersection(threadcar->dir);
	ExitIntersection(threadcar->dir);
}

void tick() {
	intersectionClock += DELTA_TICK;
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
	car* cars = GetCars();

	for (int i = 0; i < NUM_CARS; i++)
		printf("dir 1: %c dir 2: %c\n", cars[i].dir->dir_original, cars[i].dir->dir_target);

	/*Thread stuff goes here*/

	freeCars(cars);
}