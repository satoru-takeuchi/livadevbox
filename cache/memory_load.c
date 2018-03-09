#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define CACHE_LINE_SIZE	64

int main(int argc, char *argv[])
{
	char *progname;
	progname = argv[0];

	if (argc != 2) {
		fprintf(stderr, "usage: %s <size[KB]>\n", progname);
		exit(EXIT_FAILURE);
	}

	register int size;
	size = atoi(argv[1]) * 1024;
	if (!size) {
		fprintf(stderr, "size should be >= 1: %d\n", size);
		exit(EXIT_FAILURE);
	}

	char *buffer;
	buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (buffer == (void *) -1)
		err(EXIT_FAILURE, "mmap() failed");

        struct timespec before, after;

	clock_gettime(CLOCK_MONOTONIC, &before);

	int i;
	for (;;) {
		long j;
		for (j = 0; j < size; j += CACHE_LINE_SIZE)
			buffer[j] = 0;
	}

	if (munmap(buffer, size) == -1)
		err(EXIT_FAILURE, "munmap() failed");
	exit(EXIT_SUCCESS);
}
