#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEGABYTE 1048576
#define SIZE (1*MEGABYTE)

char buf[SIZE];

static void setBuf() {
	for (int i = 0; i < SIZE; i++) {
		buf[i] = 0;
	}
}

int main() {
	/* How long does memset take? */
	printf("memset: ");
	double start = omp_get_wtime();
	memset(buf, 0, SIZE);
	printf(" %f seconds\n", omp_get_wtime() - start);

	/* How long does setBuf take? */
	printf("setBuf: ");
	start = omp_get_wtime();
	setBuf();
	printf(" %f seconds\n", omp_get_wtime() - start);

	return 0;
}
