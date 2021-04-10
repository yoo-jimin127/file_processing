#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (void) {
	char buf[51] = "";
	char data[6]= "";

	strcpy(data, "abcde");

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			strcpy(&buf[j + (i * 10)], &data[i]);
		}
	}

	printf("%s\n", buf);

	return 0;
}
