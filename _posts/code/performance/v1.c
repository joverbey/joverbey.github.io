#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEGABYTE 1048576
#define SIZE (1*MEGABYTE)

/* We will measure two ways to clear this buffer */
char buf[SIZE];

/* Clear buf using a simple for-loop */
static void setBuf() {
	for (int i = 0; i < SIZE; i++) {
		buf[i] = 0;
	}
}

int main() {
	/* How long does setBuf take? */
	printf("setBuf: ");
	double start = omp_get_wtime();
	setBuf();
	printf(" %f seconds\n", omp_get_wtime() - start);

	/* How long does memset take? */
	printf("memset: ");
	start = omp_get_wtime();
	memset(buf, 0, SIZE);
	printf(" %f seconds\n", omp_get_wtime() - start);

	return 0;
}
