/* CalculatePriority.c -- schema to calculate priority */
/* These changes assumes a couple of things that are worth noting
   (1) A single ready_list is assumed -- not a set of 64 lists
   (2) This adds global and thread struct variables to support
       the dynamic calculation of priority. It is unclear if this
       is what the spec actually means by priority scheduling.
   (3) There is a #define variable in thread_tick that can be
       set to 0 to enable dynamic update of a threads priority.
       Right now it is set to 1, so even though all the work is
       being done, it prevents prevents the actual update of the
       thread priority variable.
*/

/*************************************************************/
/* thread.h change:  Add 3 variables to the thread struct    */
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
/* thread.c change:  Add 4 global variables & 1 definition   */
/*************************************************************/
   #define ONE_SECOND 100          /* number of clock ticks in 1 second */
   static long long load_avg_cnt;  /* sum clock ticks for load_avg interval */
   static long long RT;            /* Used to count ready threads */
   static long long load_avg;      /* 1000*load_avg for priority calcs*/
   static long long alpha;         /* 1000*alpha for priority calcs */
   static list_element* lptr;      /* used to search lists */
   static struct thread* tptr;


/*************************************************************/
/* tread.c change:  >>init_thread( ... )                     */
/*************************************************************/
   t->priority = priority;
   t->base_priority = priority;  /*initialize base_priority*/
   t->recent_cpu = 0;		/*initialize recent_cpu*/
   t->recent_nice = 0;		/*initialize "nice" value*/
   t->magic = THREAD_MAGIC;

/*************************************************************/
/* thread.c change: >> thread_tick()                         */
/*************************************************************/
   else
      kernel_ticks++;
/*---------------------*/

/* Increment recent_cpu value of running thread every clock tick*/
   if (t != idle_thread) t->recent_cpu++;

/* Perform Priority Calculations once per second */
   if ( ++load_avg_cnt > ONE_SECOND )
   {
      load_avg = (4028*load_avg + 68267*RT)/4096;         //Scaled by 1000
      alpha = (2000*load_avg)/(1000+load_avg+load_avg);   //Scaled by 1000
      load_avg_cnt = 0;					  //Reset counter

/* Update all thread priorities once per second */
/* note: list picked is all_list.  should it be ready list??? */
/* if so, remember to change allelem to elem */
      lptr = list_head(&all_list);
      while ( (lptr = list_next(lptr)) != list_tail )
      {
         tptr = list_entry( lptr, struct thread, allelem );
         tptr->recent_cpu = alpha * tptr->recent_cpu + 1000 * tptr->nice;
#define DISABLE_PRIORITY_CHANGE 1
         if ( DISABLE_PRIORITY_CHANGE != 1)
         {
            tptr->priority = tptr->priority - (tptr->recent_cpu/4000) - 2 * tptr->nice;
            if (tptr->priority > PRI_MAX) tptr->priority = PRI_MAX;
            if (tptr->priority < PRI_MIN) tptr->priority = PRI_MIN;
         }
      }
   }
/*---------------*/
  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE)
    intr_yield_on_return ();
}

/*************************************************************/
/* thread.c change: >> next_thread_to_run()                  */
/*************************************************************/
/*replace the entire function with the following new code    */
/*note: new else clause solves 2 problems:
  (1) it returns the next thread based on highest priority
  (2) It counts the number of threads in the ready_list. (RT)*/

static struct thread *
next_thread_to_run (void) 
{
   if (list_empty (&ready_list))
      return idle_thread;
   else
   {
      static struct list_elem *e, *esav;
      static int psav, tmp;
      e    = list_begin(&ready_list);
      esav = e;
      psav = list_entry(e, struct thread, elem)->priority; 
      RT = 0;
      while ( (e = list_next(e)) != list_end(&ready_list))
      {
         tmp = list_entry(e, struct thread, elem)->priority;
         if (psav < tmp) { psav = tmp; esav = e; }; 
         RT++;
      }
      list_remove( esav );
       return list_entry(esav, struct thread, elem);
   } 
}

/*************************************************************/
/* thread.c change: >> thread_set_priority                   */
/*************************************************************/
void
thread_set_priority (int new_priority) 
{
  thread_current ()->priority = new_priority;
  thread_current ()->base_priority = new_priority;
  thread_yield();
}


/*************************************************************/
/* thread.c change: >> thread_set_nice                       */
/*************************************************************/
void
thread_set_nice (int nice ) 
{
  thread_current ()->nice = nice;
}

/*************************************************************/
/* thread.c change: >> thread_get_nice                       */
/*************************************************************/
/* Returns the current thread's nice value. */
int
thread_get_nice (void) 
{
  return thread_current ()->nice;
}


/*************************************************************/
/* thread.c change: >> thread_get_load_avg                   */
/*************************************************************/
/* Returns 100 times the system load average. */
int
thread_get_load_avg (void) 
{
  return load_avg/10;
}


/*************************************************************/
/* thread.c change: >> thread_get_recent_cpu                 */
/*************************************************************/
/* Returns 100 times the current thread's recent_cpu value. */
int
thread_get_recent_cpu (void) 
{
  return thread_current ()->recent_cpu;
}







