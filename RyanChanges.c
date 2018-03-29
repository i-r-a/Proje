/*REQUIREMENT 01
Implement priority schedling in Pintos.  When a thread is added to the ready list
that has a higher priority than the currently running thread, the current
thread should immediately yield the processor to the new thread.*/

/* thread.c change: next_thread_to_run(void) */


/*===== Replace following  Current subroutine ==============*/
static struct thread *
next_thread_to_run (void)
{
  if (list_empty (&ready_list))
    return idle_thread;
  else
    return list_entry (list_pop_front (&ready_list), struct thread, elem);
}

/*------------------- with the following -------------------*/
/* This routine replaces the round robin approach which is the default method.
   It searches the ready-list from front to back looking for the highest
   priority thread. */
static struct thread *
next_thread_to_run (void) 
{
  if (list_empty (&ready_list))
    return idle_thread;
  else {
    static struct list_elem *e, *esav;
    static int psav, tmp;
    e    = list_begin(&ready_list);
    esav = e;
    psav = list_entry(e, struct thread, elem)->priority; 
    while ( (e = list_next(e)) != list_end(&ready_list)) {
       tmp = list_entry(e, struct thread, elem)->priority;
       if (psav < tmp) { psav = tmp; esav = e; }; 
    }
   list_remove( esav );
   return list_entry(esav, struct thread, elem);
 } 
}
/*=============================================================*/
/*=============================================================*/
/*=============================================================*/


/*REQUIREMENT 02
Similarly, when threads are waiting for a lock, semaphore, or condition
variable, the highest priority waiting thread should be awakened first.

Note that semaphores, locks, and condition variables all use sema-down()
to push blocked threads onto the waiters list.  Later they all use
seme_up() to pop the threads off the waiters list.*/

/* synch.c change:  sema_down() */

/* ======== Replace the following current subroutine ========*/
void
sema_down (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  while (sema->value == 0)
    {
      list_push_back (&sema->waiters, &thread_current ()->elem);
      thread_block ();
    }
  sema->value--;
  intr_set_level (old_level);
}

/*--------------------- with the following -----------------*/

sema_down (struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());
  old_level = intr_disable ();
  while (sema->value == 0) 
    {
      if (list_empty(&sema->waiters))
      {
        list_push_back (&sema->waiters, &thread_current ()->elem);
      }
      else
      {
        struct list_elem* e = list_begin(&sema->waiters);
        int pri = thread_current()->priority;
        int ple = list_entry(e, struct thread, elem)->priority;
        while ( (ple > pri) && (e != list_end(&sema->waiters) ) )
        {
           e = list_next(e);
           ple = list_entry(e,struct thread, elem)->priority;
        };
        list_insert(e,&thread_current()->elem);
      }
      thread_block();
    }
  sema->value--;
  intr_set_level (old_level);
}

/*=============================================================*/
/*=============================================================*/
/*=============================================================*/
/*REQUIREMENT 03
A thread may raise or lower its own priority at any time, but lowering its
priority such that it no longer has the highest priority must cause it
to immediately yield the CPU*/


/* thread.c change:   thread_set_priority(new_priority) */
/*================replace the following subroutine ===========*/

void
thread_set_priority (int new_priority)
{
  thread_current ()->priority = new_priority;
}

/*----------------with the following-------------------*/

thread_set_priority (int new_priority) 
{
  thread_current ()->priority = new_priority;
  thread_yield();
}

/*=============================================================*/
/*=============================================================*/



