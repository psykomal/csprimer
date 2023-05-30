#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#define SLEEP_SEC 1
#define NUM_MULS 100000000
#define NUM_MALLOCS 100000
#define MALLOC_SIZE 1000
#define RUSAGE_SELF 0

#define TOTAL_USEC(tv) (tv).tv_sec * 1000000 + (tv).tv_usec

uint64_t get_cpu_freq(void)
{
    uint64_t freq = 0;
    size_t size = sizeof(freq);

    if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) < 0)
    {
        perror("sysctl");
    }
    return freq;
}

// TODO define this struct
struct profile_times
{
    uint64_t real_usec;
    uint64_t user_usec;
    uint64_t system_usec;
};

// TODO populate the given struct with starting information
void profile_start(struct profile_times *t)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t->real_usec = TOTAL_USEC(tv);

    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    t->user_usec = TOTAL_USEC(ru.ru_utime);
    t->system_usec = TOTAL_USEC(ru.ru_stime);
}

// TODO given starting information, compute and log differences to now
void profile_log(struct profile_times *t)
{

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t current_real_usec = TOTAL_USEC(tv);

    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    uint64_t current_user_usec = TOTAL_USEC(ru.ru_utime);
    uint64_t current_system_usec = TOTAL_USEC(ru.ru_stime);

    fprintf(stderr, "[pid: %d | cpu: %d] real: %0.03f user: %0.05f system: %0.05f \n", getpid(), get_cpu_freq(),
            (current_real_usec - t->real_usec) / 1000000.0,      // real
            (current_user_usec - t->user_usec) / 1000000.0,      // user
            (current_system_usec - t->system_usec) / 1000000.0); // system
}

int main(int argc, char *argv[])
{
    struct profile_times t;

    // TODO profile doing a bunch of floating point muls
    float x = 1.0;
    profile_start(&t);
    for (int i = 0; i < NUM_MULS; i++)
        x *= 1.1;
    profile_log(&t);

    // TODO profile doing a bunch of mallocs
    profile_start(&t);
    void *p;
    for (int i = 0; i < NUM_MALLOCS; i++)
        p = malloc(MALLOC_SIZE);
    profile_log(&t);

    // TODO profile sleeping
    profile_start(&t);
    sleep(SLEEP_SEC);
    profile_log(&t);
}
