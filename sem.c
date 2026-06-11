/* MAMAN 01
 * TZ: 329604441
 * Name: Sacha HADJADJ
 *
 * Binary semaphore implemented using synchronous signals (SIGUSR1)
 * 3 Functons as asked:
 *   int  init_sem(int status);   // 1 = open, 0 = closed
 *   void down_sem(void);         // wait operation
 *   void up_sem(void);           // signal operation
 *
 * The semaphore is shared between threads of the same process.
 */

#include <signal.h>
#include <unistd.h>

// Global variables
static sigset_t sem_set;          // signal set used by sigwait         
static volatile int sem_val = 0;  // binary value: 0 = closed, 1 = open  

// Initialize the semaphore to "status" (0 = closed, 1 = open).
int init_sem(int status)
{
    // Prepare the signal set with SIGUSR1.
    sigemptyset(&sem_set);
    sigaddset(&sem_set, SIGUSR1);

    // Blocks SIGUSR1 at this process so sigwait() can be used.
    if (sigprocmask(SIG_BLOCK, &sem_set, NULL) == -1) {
        return -1;  // Error
    }

    sem_val = (status != 0) ? 1 : 0;
    return 0;     // Sucess
}

// Down operation: wait until the Semaphore is available, then take it.
void down_sem(void)
{
    int sig;

    while (1) {
        if (sem_val > 0) {
            // Takes the Semaphore.
            sem_val = 0;
            return;
        }

        // Wait synchronously for SIGUSR1.
        sigwait(&sem_set, &sig);
        // When sigwait returns, some thread has called up_sem().
        // Loop again and try to take the semaphore. 
    }
}

// Up operation: release the semaphore and wake one waiting thread.
void up_sem(void)
{
    if (sem_val == 0) {
        sem_val = 1;
        // Send SIGUSR1 to this process, waking exactly one thread
        // that is blocked in sigwait(). 
        kill(getpid(), SIGUSR1);
    }
}
