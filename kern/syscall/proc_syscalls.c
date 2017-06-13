#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h>
#include <mips/trapframe.h>
#include <limits.h>
#include "opt-A2.h"

  /* this implementation of sys__exit does not do anything with the exit code */
  /* this needs to be fixed to get exit() and waitpid() working properly */

void sys__exit(int exitcode) {

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */
  (void)exitcode;

  DEBUG(DB_SYSCALL,"Syscall: _exit(%d)\n",exitcode);

  KASSERT(curproc->p_addrspace != NULL);
  as_deactivate();
  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

  /* if this is the last user process in the system, proc_destroy()
     will wake up the kernel menu thread */
  proc_destroy(p);
  
  thread_exit();
  /* thread_exit() does not return, so we should never get here */
  panic("return from thread_exit in sys_exit\n");
}


/* stub handler for getpid() system call                */
int
sys_getpid(pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
  #ifdef OPT_A2
  *retval = curproc->pid;
  #else
  *retval = 1;
  #endif
  return(0);
}

/* stub handler for waitpid() system call                */

int
sys_waitpid(pid_t pid,
	    userptr_t status,
	    int options,
	    pid_t *retval)
{
  int exitstatus;
  int result;

  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.   
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

  if (options != 0) {
    return(EINVAL);
  }
  /* for now, just pretend the exitstatus is 0 */
  exitstatus = 0;
  result = copyout((void *)&exitstatus,status,sizeof(int));
  if (result) {
    return(result);
  }
  *retval = pid;
  return(0);
}

#if OPT_A2
int sys_fork(pid_t *retval, struct trapframe *tf){
  // create new process
  struct proc* p = proc_create_runprogram("process");
  if (p == NULL)
  {
    return ENOMEM;
  }

  // if too many process
  if (p->pid > PID_MAX)
  {
    proc_destroy(p);
    return ENPROC;
  }

  // create new address space
  struct addrspace* as;
  int as_check = 0;
  as_check = as_copy(curproc->p_addrspace, &as)
  if (as_check != 0)
  {
    proc_destroy(p);
    return ENOMEM;
  }
  // assign the as to proc
  spinlock_acquire(p->p_lock);
  p->p_addrspace = as;
  spinlock_release(p->p_lock);

  // create a trapfram
  struct trapframe* child_tf;
  child_tf = kmalloc(sizeof(*child_tf));
  if (child_tf = NULL)
  {
    array_add(reuse_pid, p->pid, NULL);
    kfree(as);
    proc_destroy(p);
    return ENOMEM;
  }
  *child_tf = *tf;

  // create a thread
  int td_check = 0;
  td_check = thread_fork("child_process", p, enter_fork_process, child_tf, 0);
  if (td_check != 0)
  {
    array_add(reuse_pid, p->pid, NULL);
    kfree(as);
    kfree(child_tf);
    proc_destroy(p);
    return ENOMEM;
  }

  // assign parent pid
  lock_acquire(pid_control);
  struct pid* child_pid_info;
  child_pid_info = pid_struct_create(p->pid, curproc->pid);
  p->p_pid_info = child_pid_info;
  array_add(total_proc, child_pid_info, NULL);
  lock_release(pid_control);

  // assign return value
  *retval = p->pid;
  return 0;

}
#endif
