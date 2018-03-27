#include <stdio.h>

typedef struct _directions {
	char dir_original;
	char dir_target;
} directions;void ArriveIntersection(directions dir) {}void CrossIntersection(directions dir) {}void ExitIntersection(directions dir) {}
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