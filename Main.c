#include <stdio.h>

const int DELTA_LEFT = 3, DELTA_STRAIGHT = 2, DELTA_RIGHT = 1;

typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;typedef struct _car {	int cid;	directions dir;	double arrival_time;} car;void ArriveIntersection(directions dir) {	}void CrossIntersection(directions dir) {}void ExitIntersection(directions dir) {}
Car(directions dir) {
	ArriveIntersection(dir);
	CrossIntersection(dir);
	ExitIntersection(dir);
}
void main() {
	//test..
	printf("Hello, world!\n");
	system("pause");
}