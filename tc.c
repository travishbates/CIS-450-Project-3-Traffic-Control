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
pthread_mutex_t mutex, lineLock;
pthread_cond_t c;

typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;

typedef struct _car {
	int cid;
	double arrival_time;
	directions *dir;
} car;

typedef struct _int_lock {
	int numLocked;
	pthread_mutex_t lock;
} int_lock;

int_lock *lockNN, *lockNE, *lockNS, *lockNW,
*lockEN, *lockEE, *lockES, *lockEW,
*lockSN, *lockSE, *lockSS, *lockSW,
*lockWN, *lockWE, *lockWS, *lockWW;

void acquireInt_Lock(int_lock *int_lock) {
	pthread_mutex_lock(&mutex);

	if (int_lock->numLocked == 0)
		pthread_mutex_lock(&int_lock->lock);

	int_lock->numLocked++;

	pthread_mutex_unlock(&mutex);
}

void releaseInt_Lock(int_lock *int_lock) {
	pthread_mutex_lock(&mutex);

	int_lock->numLocked--;

	if (int_lock->numLocked == 0)
		pthread_mutex_unlock(&int_lock->lock);

	pthread_mutex_unlock(&mutex);
}

void acquireLine_Lock(int id) {
	pthread_mutex_lock(&lineLock);

	if (id != cid)
		pthread_cond_wait(&c, &lineLock);

	pthread_mutex_unlock(&lineLock);
}

void releaseLine_Lock(int id) {
	pthread_mutex_lock(&lineLock);

	cid++;
	pthread_cond_broadcast(&c);

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
	printf("Time %lf: ", round((GetTime() - startTime) * 10) / 10);
	printf("Car %d (->%c ->%c) ", car->cid, car->dir->dir_original, car->dir->dir_target);
	printf(str);
	printf("\n");
}

/*Returns the time needed to wait, in microseconds*/
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

void CrossIntersection(car *car) {
	acquireLine_Lock(car->cid);

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

initLock(int_lock *lock) {
	lock = malloc(sizeof(int_lock));

	lock->numLocked = 0;
}

void main() {
	cid = 0;

	/*Init all locks being used*/
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&lineLock, NULL);

	initLock(lockNN);
	initLock(lockNE);
	initLock(lockNS);
	initLock(lockNW);

	initLock(lockEN);
	initLock(lockEE);
	initLock(lockES);
	initLock(lockEW);

	initLock(lockSN);
	initLock(lockSE);
	initLock(lockSS);
	initLock(lockSW);

	initLock(lockWN);
	initLock(lockWE);
	initLock(lockWS);
	initLock(lockWW);

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