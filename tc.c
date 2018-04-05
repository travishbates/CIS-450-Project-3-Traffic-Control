/*By Travis Bates and Jonathan Cardasis*/

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_CARS 8

const int DELTA_LEFT = 3, DELTA_STRAIGHT = 2, DELTA_RIGHT = 1;
double startTime;

int cid;
pthread_mutex_t mutex_cross, lineLock;
pthread_cond_t cond_line, cond_cross;

typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;

typedef struct _car {
	int cid;
	double arrival_time;
	directions *dir;
} car;

int lockNN, lockNE, lockNW,
lockEN, lockEE, lockES,
lockSE, lockSS, lockSW,
lockWN, lockWS, lockWW;

void acquireLine_Lock(int id) {
	pthread_mutex_lock(&lineLock);

	while (id != cid)
		pthread_cond_wait(&cond_line, &lineLock);

	pthread_mutex_unlock(&lineLock);
}

void releaseLine_Lock(int id) {
	pthread_mutex_lock(&lineLock);

	cid++;
	pthread_cond_broadcast(&cond_line);

	pthread_mutex_unlock(&lineLock);
}

/*Returns time in seconds*/
double GetTime() {
	struct timeval t;
	int rc = gettimeofday(&t, NULL);
	assert(rc == 0);
	return (double)t.tv_sec + (double)t.tv_usec / 1e6;
}

/*Spins for a given number of seconds.*/
void Spin(int howlong) {
	double t = GetTime();
	while ((GetTime() - t) < (double)howlong)
		; // do nothing in loop
}

void printCar(char *str, car *car) {
	printf("Time %.1f: ", round((GetTime() - startTime) * 10) / 10);
	printf("Car %d (->%c ->%c) ", car->cid, car->dir->dir_original, car->dir->dir_target);
	printf(str);
	printf("\n");
}

/*Returns the time needed to wait, in seconds*/
double getWait(car *car) {
	return ((car->arrival_time) - (GetTime() - startTime));
}

void ArriveIntersection(car *car) {
	double wait = getWait(car);

	while (wait >= 1) {
		usleep(1000000);
		wait = getWait(car);
	}

	if (wait > 0 && wait < 1)
		usleep((int)(wait * 1e6));

	printCar("arriving", car);
}

/*Determines if a traffic flow is safe to use*/
int canCross(char dir_original, char dir_target) {
	/*=====DIR_ORIGINAL NORTH COLLISION DETECTION=====*/
	if (dir_original == 'N' && dir_target == 'N' //Straight
		&& lockWS == 0 && lockEN == 0 && lockSE == 0 //Left coll.
		&& lockWW == 0 && lockEE == 0 //Straight coll.
		&& lockWN == 0) //Right coll.
		return 1;

	else if (dir_original == 'N' && dir_target == 'E' //Right
		&& lockSE == 0 //Left coll.
		&& lockEE == 0) //Straight coll.
		return 1;

	else if (dir_original == 'N' && dir_target == 'W' //Left
		&& lockEN == 0 && lockWS == 0 //Left coll.
		&& lockEE == 0 && lockWW == 0 && lockSS == 0 //Straight coll.
		&& lockSW == 0) //Right coll.
		return 1;

	/*=====DIR_ORIGINAL EAST COLLISION DETECTION=====*/
	else if (dir_original == 'E' && dir_target == 'N' //Left
		&& lockSE == 0 && lockNW == 0 //Left coll.
		&& lockWW == 0 && lockSS == 0 && lockNN == 0 //Straight coll.
		&& lockWN == 0) //Right coll.
		return 1;

	else if (dir_original == 'E' && dir_target == 'E' //Straight
		&& lockNW == 0 && lockWS == 0 && lockSE == 0 //Left coll.
		&& lockNN == 0 && lockSS == 0 //Straight coll.
		&& lockNE == 0) //Right coll.
		return 1;

	else if (dir_original == 'E' && dir_target == 'S' //Right
		&& lockWS == 0 //Left coll.
		&& lockSS == 0) //Straight coll.
		return 1;

	/*=====DIR_ORIGINAL SOUTH COLLISION DETECTION=====*/
	else if (dir_original == 'S' && dir_target == 'E' //Left
		&& lockWS == 0 && lockEN == 0 //Left coll.
		&& lockWW == 0 && lockEE == 0 && lockNN == 0 //Straight coll.
		&& lockNE == 0) //Right coll.
		return 1;

	else if (dir_original == 'S' && dir_target == 'S' //Straight
		&& lockWS == 0 && lockEN == 0 && lockNW == 0 //Left coll.
		&& lockEE == 0 && lockWW == 0 //Straight coll.
		&& lockES == 0) //Right coll.
		return 1;

	else if (dir_original == 'S' && dir_target == 'W' //Right
		&& lockNW == 0 //Left coll.
		&& lockWW == 0) //Straight coll.
		return 1;

	/*=====DIR_ORIGINAL WEST COLLISION DETECTION=====*/
	else if (dir_original == 'W' && dir_target == 'N' //Right
		&& lockEN == 0 //Left coll.
		&& lockNN == 0) //Straight coll.
		return 1;

	else if (dir_original == 'W' && dir_target == 'S' //Left
		&& lockSE == 0 && lockNW == 0 //Left coll.
		&& lockEE == 0 && lockSS == 0 && lockNN == 0 //Straight coll.
		&& lockES == 0) //Right coll.
		return 1;

	else if (dir_original == 'W' && dir_target == 'W' //Straight
		&& lockNW == 0 && lockEN == 0 && lockSE == 0 //Left coll.
		&& lockNN == 0 && lockSS == 0 //Straight coll.
		&& lockSW == 0) //Right coll.
		return 1;

	return 0;
}

/*Increments/Decrements a traffic flow*/
void addToLock(char dir_original, char dir_target, int numToAdd) {
	if (dir_original == 'N' && dir_target == 'N')
		lockNN += numToAdd;
	else if (dir_original == 'N' && dir_target == 'E')
		lockNE += numToAdd;
	else if (dir_original == 'N' && dir_target == 'W')
		lockNW += numToAdd;

	else if (dir_original == 'E' && dir_target == 'N')
		lockEN += numToAdd;
	else if (dir_original == 'E' && dir_target == 'E')
		lockEE += numToAdd;
	else if (dir_original == 'E' && dir_target == 'S')
		lockES += numToAdd;

	else if (dir_original == 'S' && dir_target == 'E')
		lockSE += numToAdd;
	else if (dir_original == 'S' && dir_target == 'S')
		lockSS += numToAdd;
	else if (dir_original == 'S' && dir_target == 'W')
		lockSW += numToAdd;

	else if (dir_original == 'W' && dir_target == 'N')
		lockWN += numToAdd;
	else if (dir_original == 'W' && dir_target == 'S')
		lockWS += numToAdd;
	else if (dir_original == 'W' && dir_target == 'W')
		lockWW += numToAdd;
}

/*Function used to help a car check and safely cross the intersection*/
void testAndAcquire(char dir_original, char dir_target) {
	pthread_mutex_lock(&mutex_cross);

	/*Wait until car can cross*/
	while (canCross(dir_original, dir_target) == 0)
		pthread_cond_wait(&cond_cross, &mutex_cross);

	pthread_mutex_unlock(&mutex_cross);

	/*Increment appropriate lock*/
	addToLock(dir_original, dir_target, 1);
}

void CrossIntersection(car *car) {
	acquireLine_Lock(car->cid);

	/*Test for locks and then acquire the proper lock*/
	testAndAcquire(car->dir->dir_original, car->dir->dir_target);

	printCar("crossing", car);

	releaseLine_Lock(car->cid);

	char dir_original = car->dir->dir_original;
	char dir_target = car->dir->dir_target;

	/*straight*/
	if (dir_original == dir_target)
		sleep(2);

	/*left*/
	else if (
		(dir_original == 'N' && dir_target == 'W')
		|| (dir_original == 'W' && dir_target == 'S')
		|| (dir_original == 'S' && dir_target == 'E')
		|| (dir_original == 'E' && dir_target == 'N')
		)
		sleep(3);

	/*right*/
	else sleep(1);
}

void ExitIntersection(car *car) {
	printCar("exiting", car);

	char dir_original = car->dir->dir_original;
	char dir_target = car->dir->dir_target;

	/*Decrement appropriate lock*/
	addToLock(dir_original, dir_target, -1);

	pthread_mutex_lock(&mutex_cross);
	pthread_cond_broadcast(&cond_cross);
	pthread_mutex_unlock(&mutex_cross);
}

/*Car thread launching function to be used by all cars*/
void *Car(void* arg) {
	car *threadcar = (car*)arg;

	ArriveIntersection(threadcar);
	CrossIntersection(threadcar);
	ExitIntersection(threadcar);
}

/*Returns a car array equal to the assignment description list of cars*/
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

/*Frees the car array memory*/
void freeCars(car* carArray) {
	for (int i = 0; i < NUM_CARS; i++)
		free(carArray[i].dir);

	free(carArray);
}

void main() {
	cid = 0;

	/*Init all locks being used*/
	pthread_mutex_init(&mutex_cross, NULL);
	pthread_mutex_init(&lineLock, NULL);

	/*Setup and print initial car array*/
	car* cars = GetCars();

	printf("Initial car array:\n");

	for (int i = 0; i < NUM_CARS; i++)
		printf("dir 1: %c dir 2: %c\n", cars[i].dir->dir_original, cars[i].dir->dir_target);

	/*Start global timer*/
	printf("Starting timer...\n");
	startTime = GetTime();

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