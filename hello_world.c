#include <stdio.h>

void helloWorlds(int);

int main(int argc, char** argv) {
	for (int i = 0; i < 5; i++) {
		puts("Hello, World!");
	}
	
	int i = 5;
	
	while (i > 0) {
		puts("Hello, World!");
		i -= 1;
	}
	
	helloWorlds(5);
	
	return 0;
}

void helloWorlds(int nTimes) {
	for (int i = 0; i < nTimes; i++) {
		puts("Hello, World!");
	}
}