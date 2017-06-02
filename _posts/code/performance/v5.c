#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 5

#define MEGABYTE 1048576
#define SIZE (1*MEGABYTE)

char buf[SIZE];

static void setBuf() {
	for (int i = 0; i < SIZE; i++) {
		buf[i] = 0;
	}
}

int main() {

	/* How long does setBuf take? */
	printf("setBuf: ");
	setBuf(); /* Avoid measuring first-time cost */
	for (int i = 0; i < ITERATIONS; i++) {
		double start = omp_get_wtime();
		setBuf();
		printf(" %f", omp_get_wtime() - start);
	}
	printf(" seconds\n");

	/* How long does memset take? */
	printf("memset: ");
	memset(buf, 0, SIZE); /* Avoid measuring first-time cost */
	for (int i = 0; i < ITERATIONS; i++) {
		double start = omp_get_wtime();
		memset(buf, 0, SIZE);
		printf(" %f", omp_get_wtime() - start);
	}
	printf(" seconds\n");

	return 0;
}
