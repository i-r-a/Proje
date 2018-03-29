/* DonateChng.c */
/* My approach is to ensure that the priority of the lock holder is one
   more than the priority of any of the threads that are waiting on it.
*/

/*************************************************************/
/* thread.h change:  Add 3 variables to the thread struct    */
/* This change was also proposed in CalcPriority.c           */
/*************************************************************/
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
#endif

    /* priority calc variables */
    int base_priority;			/*creation/set priority value*/
    int recent_cpu;			/*Counts cpu ticks used*/
    int nice;				/*thread's "nice" value*/

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/*************************************************************/
/* synch.c change:  lock_acquire( ... )                      */
/*************************************************************/ 

/* ============ Replace the current function ==============*/
void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  sema_down (&lock->semaphore);
  lock->holder = thread_current ();
}

/*------------------With the Following--------------------*/
void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  static int pri01, pri02;
  if (lock->holder != NULL)
  {
    /* force lock_holder pri > pri's of waiters */
    pri01 = lock->holder->priority;
    pri02 = thread_current ()->priority;
    if ( pri01 <= pri02 ) pri01 = pri02 + 1;
    if ( pri01 > PRI_MAX ) pri01 = PRI_MAX;
    lock->holder->priority = pri01;
  }  

  sema_down (&lock->semaphore);
  lock->holder = thread_current ();
}

/*************************************************************/ 

/*************************************************************/
/* synch.c change:  lock_release( ... )                      */
/*************************************************************/ 


/* ============ Replace the current function ==============*/
void
lock_release (struct lock *lock) 
{
  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  lock->holder = NULL;
  sema_up (&lock->semaphore);
}

/*------------------With the Following--------------------*/
void
lock_release (struct lock *lock) 
{
  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  /* ensure restoration of base priority */
  lock->holder->priority = lock->holder->base_priority;
  lock->holder = NULL;
  sema_up (&lock->semaphore);
}

/*************************************************************/ 



