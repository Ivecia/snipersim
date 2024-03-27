#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <pthread.h>

typedef unsigned long uint64_t;
typedef long off64_t;
typedef long intptr_t;

// Mapping virtual address to physical address
static int fd_va = -1;
static uint64_t va2pa(uint64_t vp)
{
    // Ignore vsyscall range
    if (vp >= 0xffffffffff600ULL && vp < 0xfffffffffffffULL)
        return vp;

    if (fd_va == -1)
    {
        fd_va = open("/proc/self/pagemap", O_RDONLY);
        assert(fd_va != -1);
    }
    off64_t index = vp * sizeof(intptr_t);
    off64_t offset = lseek64(fd_va, index, SEEK_SET);
    assert(offset == index);
    intptr_t pp;
    ssize_t size = read(fd_va, &pp, sizeof(intptr_t));

    if (size != sizeof(intptr_t))
    {
        // Lookup failed. This happens for [vdso] sections.
        return vp;
    }

    // From: https://stackoverflow.com/questions/5748492/is-there-any-api-for-determining-the-physical-address-from-virtual-address-in-li
    // From: https://stackoverflow.com/a/45128487

    intptr_t pfn = pp & (((intptr_t)1 << 54) - 1);
    bool present = (pp >> 63) & 1;

    if (!present)
    {
        // Lookup failed, Use pfn == vp
        return vp;
    }

    // From: https://www.kernel.org/doc/Documentation/vm/pagemap.txt
    // Since kernel 4.2, access to the pagemap is restricted, and the pfn is zeroed
    // Check for present and pfn == 0 to see if we have permission to run here

    // A zero-valued pfn is suspicious
    assert(pfn != 0);

    return pfn;
}

// Register the id of the main thread
static pthread_t main_thread_id;
__thread FILE *trace_file = NULL;
void registerMainThreadID()
{
    main_thread_id = pthread_self();
    trace_file = fopen("trace_main.txt", "w");
}

// Enter a function (with parallel function check)
pthread_mutex_t trace_id_mutex = PTHREAD_MUTEX_INITIALIZER;
static int trace_id = 0;
__thread bool trace_file_created = false;
void enterFunction()
{
    pthread_t pid = pthread_self();
    // Thread checking
    if (pid == main_thread_id) {
        return;
    }
    if (!trace_file_created) {
        trace_file_created = true;
        char trace_file_name[2048];
        // get trace_id in critical section
        pthread_mutex_lock(&trace_id_mutex);
        sprintf(trace_file_name, "trace_%d.txt", trace_id);
        trace_id++;
        pthread_mutex_unlock(&trace_id_mutex);
        trace_file = fopen(trace_file_name, "w");
    }
}

// Print Load and Store addresses
void printMemoryAccess(uint64_t addr)
{
    fprintf(trace_file, "%lu\n", va2pa(addr));
}

// Print whether a conditional branch is taken or not
void printBranchCondition(bool cond)
{
    if (cond)
        fprintf(trace_file, "1\n");
    else
        fprintf(trace_file, "0\n");
}
