#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define CACHE_LINE_SIZE	64
#define NLOOP		(64*1024*1024)
#define BUFFER_SIZE	(16*1024*1024)
#define NCORE		24

#define NSECS_PER_SEC	1000000000UL

static inline long diff_nsec(struct timespec before, struct timespec after)
{
        return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
                - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

static void setcpu(int n)
{
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(n, &set);
	if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1)
		err(EXIT_FAILURE, "sched_setaffinity() failed");
}

int main(int argc, char *argv[])
{
	char *progname;
	progname = argv[0];

	register int size = BUFFER_SIZE;

	setcpu(0);
	char *buffer;
	buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (buffer == (void *) -1)
		err(EXIT_FAILURE, "mmap() failed");

	int i;
	for (i = 0; i < NCORE; i++) { 
		setcpu(i);

		struct timespec before, after;
		clock_gettime(CLOCK_MONOTONIC, &before);
		int j;
		for (j = 0; j < NLOOP / (size / CACHE_LINE_SIZE); j++) {
			long k;
			for (k = 0; k < size; k += CACHE_LINE_SIZE)
				buffer[k] = 0;
		}
		clock_gettime(CLOCK_MONOTONIC, &after);
		printf("%d\t%f\n", i, (double)diff_nsec(before, after) / NLOOP);		
	}




	
	if (munmap(buffer, size) == -1)
		err(EXIT_FAILURE, "munmap() failed");
	exit(EXIT_SUCCESS);
}
