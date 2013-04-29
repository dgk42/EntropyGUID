/*
 * File: entropy_guid.c
 *
 * Entropy-GUID - Globally Unique IDentifier (GUID) entropy-based generator
 *   with 6 DOF
 * by Dionisis G. Kakoliris - dgk@bugsense.com
 *
 * 2012-05-11
 *
 * C (on Linux) sample implementation [requires glibc2 or higher]
 */

/*
 * DISCLAIMER: Please note that the following sample implementation is just
 *   a proof of concept. One can refine it and produce better results.
 *   Especially the final digest step is implemented in a "naive" way for the
 *   sake of simplicity (using the MD5 algorithm - don't use this).
 *   This sample implementation works under Linux only
 *   (don't try it on MacOS for example - it doesn't work as expected, due
 *   to the crypt() call).
 */


#define _BSD_SOURCE
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <time.h>
#include <inttypes.h>


typedef char tag_unique_id[256];


/**
 * \brief Read Time Stamp Counter
 * \return the counter value
 *
 * On modern multi-core, super-scalar systems with out-of-order execution and
 *   ACPI features, the return value may not be accurate.
 */
static inline uint64_t
rdtsc(void)
{
	uint64_t x;

	__asm__ volatile ("rdtsc" : "=A" (x));

	return x;
}

/**
 * \brief This function generates a GUID.
 * \param result a string containing the generated GUID
 * \return 0 on error, != 0 otherwise
 *
 * This is a sample implementation of the Entropy-GUID algorithm.
 */
int
get_uuid(tag_unique_id *result)
{
	char *out;
	char *ch;
	DIR *dir;
	int count;
	uint64_t tsc;
	struct timeval tv1, tv2;
	double dtv1, dtv2;
	char s1[64], s2[128], s3[64], s4[64], s5[64], s6[64],
	     sall[256] = { 0 };

	// STEP 1: get time since Epoch.
	// OUTPUT: timestamp in millisecond (or finer) granularity.
	// RATIONALE: collision only when app is deployed at the exact same
	//   time since Epoch (no local time).
	gettimeofday(&tv1, NULL);
	snprintf(s1, 64, "%ld%ld", (long) tv1.tv_sec, (long) tv1.tv_usec);

	// STEP 2: get an object reference.
	// OUTPUT: memory address or another distinguishable object property.
	// RATIONALE: very little probability for two runs to get the
	//   same address (debatable when in a sandbox).
	// NOTE: the following static buffer allocation (s2 of MAX size)
	//   is able to hold words with varying sizes below 512
	//   (e.g. size of 256 bits), such that:
	//   (((int) ceil(word_size_in_bits / 8.0)) * 2 + 2) < MAX
	ch = (char *) malloc(sizeof(char));
	if (!ch)
		exit(1);
	snprintf(s2, 128, "%p", ch);
	free(ch);
	ch = NULL;

	// STEP 3: sleep for a little time.
	// OUTPUT: time delta in _finer_ than millisecond granularity.
	// RATIONALE: os fine timing granularity is not guaranteed
	//   (debatable when dealing with real-time/hard real-time
	//   kernels).
	// NOTE: The RDTSC calls may be ommitted, since on a modern
	//   system they may not produce correct values (nonetheless, for the
	//   sake of "randomness" one may choose to keep them).
	tsc = rdtsc();
	gettimeofday(&tv1, NULL);
	dtv1 = tv1.tv_sec * 1000.0 + tv1.tv_usec / 1000.0;
	usleep(256 * 1000);
	gettimeofday(&tv2, NULL);
	dtv2 = tv2.tv_sec * 1000.0 + tv2.tv_usec / 1000.0;
	tsc = rdtsc() - tsc;

	snprintf(s3, 64, "%f:%" PRIu64, dtv2 - dtv1, tsc);

	// STEP 4: get a platform-dependent "distinguishable" context value.
	// OUTPUT: something different without needing escalated priviles.
	// RATIONALE: a value that may differentiate two contexts.
	//   Examples include: a device ID (without permissions for
	//   accessing it), number of physical processors,
	//   os version, file count of current directory,
	//   user name, number of processes spawned from the user, etc.
	count = 2;
	if ((dir = opendir("."))) {
		for (count = 0;  readdir(dir); count++);
		closedir(dir);
	}
	snprintf(s4, 64, "%d", count);

	// STEP 5: generate a pseudo-random number.
	// OUTPUT: random integer.
	// RATIONALE: the pseudo-random generator may be the same, but there
	//   is a slight chance that something might have changed in the
	//   seed (may be a second passed in some cases).
	//   Also, some systems (e.g. a JVM) have their own pseudo-random
	//   generators that are "fed" with their own random seed
	//   implementations, narrowing the probability for same results.
	srand(time(NULL));
	snprintf(s5, 64, "%d", rand());

	// STEP 6: do a complementary step.
	// OUTPUT: the "age" of the implementation as a kick-starter.
	// RATIONALE: this is not a crucial step for the algorithm. We just
	//   provide an indication on the implementation "age". Examples
	//   include: an arbitrary string value, an integer,
	//   the publisher's name, etc.
	snprintf(s6, 64, "%s::%d", "www.bugsense.com", 1);

	// The output string.
	snprintf(sall, 256, "%s%s%s%s%s%s", s1, s2, s3, s4, s5, s6);
#ifndef NDEBUG
	printf("str = %s\n", sall);
#endif /* NDEBUG */

	// MD5 from output string.
	out = crypt(sall, "$1$xx$789012");

	// RESULT.
	snprintf(*result, 256, "%s", &out[6]);
#ifndef NDEBUG
	printf("digest = %s\n", *result);
#endif /* NDEBUG */

	return 1;
}


#ifdef TEST1
int
main(void)
{
	int r;
	tag_unique_id a_unique_id;

	r = get_uuid(&a_unique_id);
	if (!r)
		fprintf(stderr, "Internal error.\n");
#ifdef NDEBUG
	if (r)
		printf("|%s|\n", a_unique_id);
#endif /* NDEBUG */

	return 0;
}
#endif /* TEST1 */


/* EOF */
