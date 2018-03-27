#include <stdio.h>

#define NUM_CARS 8

const int DELTA_LEFT = 3, DELTA_STRAIGHT = 2, DELTA_RIGHT = 1;
const double DELTA_TICK = 0.1;

double clock = 0.0;


typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;typedef struct _car {	int cid;	double arrival_time;	directions dir;} car;void ArriveIntersection(directions dir) {	}void CrossIntersection(directions dir) {}void ExitIntersection(directions dir) {}
Car(directions dir) {
	ArriveIntersection(dir);
	CrossIntersection(dir);
	ExitIntersection(dir);
}/*car GetCars() {	car carArray[NUM_CARS] = {		{0, 1.1, { 'N', 'N' }},		{1, 2.0, { 'N', 'N' }},		{2, 3.3, { 'N', 'N' }},		{3, 3.5, { 'N', 'N' }},		{4, 4.2, { 'N', 'N' }},		{5, 4.4, { 'N', 'N' }},		{6, 5.7, { 'N', 'N' }},		{7, 5.9, { 'N', 'N' }},	};	return carArray;}*/void tick() {	//check new cars entering	//follow instructions (semaphores) for	//passing thru intersection	//tick	clock += DELTA_TICK;}
void main() {
	//car cars[NUM_CARS] = GetCars();

	car cars[NUM_CARS] = {		{ 0, 1.1,{ 'N', 'N' } },		{ 1, 2.0,{ 'N', 'N' } },		{ 2, 3.3,{ 'N', 'N' } },		{ 3, 3.5,{ 'N', 'N' } },		{ 4, 4.2,{ 'N', 'N' } },		{ 5, 4.4,{ 'N', 'N' } },		{ 6, 5.7,{ 'N', 'N' } },		{ 7, 5.9,{ 'N', 'N' } },	};

	while (1)
		tick();

	for (int i = 0; i < NUM_CARS; i++)
		printf("dir 1: %c dir 2: %c\n", cars[i].dir.dir_original, cars[i].dir.dir_target);

	printf("Hello, world!\n");
	system("pause");
}