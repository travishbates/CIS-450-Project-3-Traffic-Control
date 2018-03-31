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

pthread_mutex_t lineLockN, lineLockE, lineLockS, lineLockW;
pthread_mutex_t rightLockN, rightLockE, rightLockS, rightLockW;
pthread_mutex_t leftLockNWClose, leftLockNEClose, leftLockSEClose, leftLockSWClose;
pthread_mutex_t leftLockNWFar, leftLockNEFar, leftLockSEFar, leftLockSWFar;
pthread_mutex_t centerLock;

int numLeftNS, numLeftEW;

typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;

typedef struct _car {
	int cid;
	double arrival_time;
	directions *dir;
} car;

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

void acquireStraightLocks(pthread_mutex_t *lock1, pthread_mutex_t *lock2,
	pthread_mutex_t *lock3, pthread_mutex_t *lock4, pthread_mutex_t *lock5) {
	pthread_mutex_lock(lock1);
	pthread_mutex_lock(lock2);
	pthread_mutex_lock(lock3);
	pthread_mutex_lock(lock4);
	pthread_mutex_lock(lock5);
}

void acquireLeftLocks() {
	/*NOTE: INCREMENT NUMLEFTLS OR NUMLEFTRS, TAKE AS PARAMETER*/
}

void acquireLocks(char dir_original, char dir_target) {
	pthread_mutex_t *lineLock;

	/*Determine line lock*/
	if (dir_original == 'N')
		lineLock = &lineLockN;
	else if (dir_original == 'E')
		lineLock = &lineLockE;
	else if (dir_original == 'S')
		lineLock = &lineLockS;
	else lineLock = &lineLockW;

	/*Acquire line lock*/
	pthread_mutex_lock(lineLock);

	/*Acquire locks to cross*/
	if (dir_original == 'N' && dir_target == 'N')
		acquireStraightLocks(&leftLockSEClose, &leftLockSEFar, &leftLockNEClose, &leftLockNEFar, &rightLockN);
	else if (dir_original == 'E' && dir_target == 'E')
		acquireStraightLocks(&leftLockSWClose, &leftLockSWFar, &leftLockSEClose, &leftLockSEFar, &rightLockE);
	else if (dir_original == 'S' && dir_target == 'S')
		acquireStraightLocks(&leftLockNWClose, &leftLockNWFar, &leftLockSWClose, &leftLockSWFar, &rightLockS);
	else if (dir_original == 'W' && dir_target == 'W')
		acquireStraightLocks(&leftLockNEClose, &leftLockNEFar, &leftLockNWClose, &leftLockNWFar, &rightLockW);

	else if (dir_original == 'N' && dir_target == 'E')
		pthread_mutex_lock(&rightLockE);
	else if (dir_original == 'E' && dir_target == 'S')
		pthread_mutex_lock(&rightLockS);
	else if (dir_original == 'S' && dir_target == 'W')
		pthread_mutex_lock(&rightlockW);
	else if (dir_original == 'W' && dir_target == 'N')
		pthread_mutex_lock(&rightLockN);

	/*TODO: WRITE LOCKS FOR LEFT TURNS*/

	/*Release line lock*/
	pthread_mutex_unlock(lineLock);
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

	acquireLocks(car->dir->dir_original, car->dir->dir_target);
}

void CrossIntersection(car *car) {
	printCar("crossing", car);

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

void releaseLocks(char dir_original, char dir_target) {
	/*Release locks to cross, in reverse order in which they were acquired*/

	/*TODO: RELEASE LOCKS FOR STRAIGHTS*/
	/*TODO: RELEASE LOCKS FOR RIGHTS*/
	/*TODO: RELEASE LOCKS FOR LEFTS*/
}

void ExitIntersection(car *car) {
	printCar("exiting", car);

	releaseLocks(car->dir->dir_original, car->dir->dir_target);
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
	/*Init all locks being used*/
	pthread_mutex_init(&lineLockN, NULL);
	pthread_mutex_init(&lineLockE, NULL);
	pthread_mutex_init(&lineLockS, NULL);
	pthread_mutex_init(&lineLockW, NULL);

	pthread_mutex_init(&rightLockN, NULL);
	pthread_mutex_init(&rightLockE, NULL);
	pthread_mutex_init(&rightLockS, NULL);
	pthread_mutex_init(&rightLockW, NULL);

	pthread_mutex_init(&leftLockNWClose, NULL);
	pthread_mutex_init(&leftLockNEClose, NULL);
	pthread_mutex_init(&leftLockSEClose, NULL);
	pthread_mutex_init(&leftLockSWClose, NULL);

	pthread_mutex_init(&leftLockNWFar, NULL);
	pthread_mutex_init(&leftLockNEFar, NULL);
	pthread_mutex_init(&leftLockSEFar, NULL);
	pthread_mutex_init(&leftLockSWFar, NULL);

	pthread_mutex_init(&centerLock, NULL);
	numLeftNS = 0;
	numLeftEW = 0;

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