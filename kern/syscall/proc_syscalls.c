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
#include <synch.h>
#include <copyinout.h>
#include <vfs.h>
#include <kern/fcntl.h>
#include "opt-A2.h"

  /* this implementation of sys__exit does not do anything with the exit code */
  /* this needs to be fixed to get exit() and waitpid() working properly */

void sys__exit(int exitcode) 
{

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */

  #ifdef OPT_A2

    lock_acquire(pid_control);

    // current pid
    pid_t cur_pid = p->p_pid;

    // find all the children
    struct pid_info* temp = NULL;
    unsigned i = 0;
    unsigned size = array_num(total_proc);
    while(i < size)
    {
      temp = (struct pid_info *)array_get(total_proc, i);
      if (temp->parent == cur_pid)
      {
        // child has exit already
        if (temp->exit)
        {
          array_remove(total_proc, i);
          pid_t* child_pid = &temp->current;
          array_add(reuse_pid, child_pid, NULL);
          pid_info_destroy(temp);
          temp = NULL;
          size--;
          continue;
        }
        // child has not yet exit
        else 
        {
          temp->parent = 0;
        }
      }
      i++;
    }

    // find the current pid infomation in the proc arr
    i = 0;
    temp = NULL;
    while(i < size)
    {
      // find the target proc
      temp = (struct pid_info *)array_get(total_proc, i);
      if (temp->current == cur_pid)
      {
        break;
      }
      i++;
    }  

    // if current proc has no parent
    if (temp->parent == 0)
    {
      pid_t* child_pid = &temp->current;
      array_add(reuse_pid, child_pid, NULL);
      pid_info_destroy(temp);
    }
    else
    {
      temp->exit = 1;
      temp->exit_code = _MKWAIT_EXIT(exitcode);
      cv_broadcast(pid_cv, pid_control);
    }
    lock_release(pid_control);

  #else
    (void)exitcode;
  #endif

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
  *retval = curproc->p_pid;
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

  if (options != 0) 
  {
    return(EINVAL);
  }

  #ifdef OPT_A2
    lock_acquire(pid_control);

    unsigned i = 0;
    unsigned size = array_num(total_proc);
    struct pid_info* temp = NULL;
    while (i < size)
    {
      temp = array_get(total_proc, i);
      if (pid == temp->current)
      {
        break;
      }
      i++;
    }

    // if not find
    if (i == size)
    {
      lock_release(pid_control);
      return(ESRCH);
    }

    // if the parent is not corrent
    if (temp->parent != curproc->p_pid)
    {
      lock_release(pid_control);
      return(ECHILD);
    }

    // while not exit
    while (!temp->exit)
    {
      cv_wait(pid_cv, pid_control);
    } 
    exitstatus = temp->exit_code;
    lock_release(pid_control);
  #else
    /* for now, just pretend the exitstatus is 0 */
    exitstatus = 0;
  #endif
  result = copyout((void *)&exitstatus,status,sizeof(int));
  if (result) 
  {
    return(result);
  }
  *retval = pid;
  return(0);
}

#ifdef OPT_A2
int sys_fork(pid_t *retval, struct trapframe *tf)
{
  // create new process
  struct proc* p = proc_create_runprogram("process");
  if (p == NULL)
  {
    return ENOMEM;
  }

  // if too many process
  if (p->p_pid > PID_MAX)
  {
    proc_destroy(p);
    return ENPROC;
  }

  // create new address space
  struct addrspace* as;
  int as_check = 0;
  as_check = as_copy(curproc->p_addrspace, &as);
  if (as_check != 0)
  {
    proc_destroy(p);
    return ENOMEM;
  }

  // assign the as to proc
  spinlock_acquire(&p->p_lock);
  p->p_addrspace = as;
  spinlock_release(&p->p_lock);

  // create a trapfram
  struct trapframe* child_tf;
  child_tf = kmalloc(sizeof(*child_tf));
  if (child_tf == NULL)
  {
    pid_t* pt = &p->p_pid;
    array_add(reuse_pid, pt, NULL);
    kfree(as);
    proc_destroy(p);
    return ENOMEM;
  }
  *child_tf = *tf;

  // create a thread
  int td_check = 0;
  td_check = thread_fork("child_process", p, enter_forked_process, child_tf, 0);
  if (td_check != 0)
  {
    pid_t* pt = &p->p_pid;
    array_add(reuse_pid, pt, NULL);
    kfree(as);
    kfree(child_tf);
    proc_destroy(p);
    return ENOMEM;
  }

  // assign parent pid
  lock_acquire(pid_control);
  struct pid_info* child_pid_info;
  child_pid_info = pid_info_create(p->p_pid, curproc->p_pid);
  if (child_pid_info == NULL)
  {
    lock_release(pid_control);
    return ENOMEM;
  }
  p->p_pid_info = child_pid_info;
  array_add(total_proc, child_pid_info, NULL);
  lock_release(pid_control);

  // assign return value
  *retval = p->p_pid;
  return 0;

}

int sys_execv(userptr_t progname, userptr_t args)
{
  struct addrspace *as;
  struct addrspace *as_old;
  struct vnode *v;
  vaddr_t entrypoint, stackptr;
  int result;

  // count the number of arguments
  unsigned long count = 0;
  while(((char **)args)[count] != NULL)
  {
    count++;
  }

  size_t arr_len = sizeof(char *) * (count + 1);
  char** copy_arr = kmalloc(arr_len);

  result = copyin(args, copy_arr, arr_len);
  if(result){
    kfree(copy_arr);
    return result;
  }

  // copy args
  size_t str_len = 0;
  for (unsigned long i = 0; i < count; ++i)
  {
    str_len = strlen(((char **)args)[i]) + 1;

    copy_arr[i] = kmalloc(str_len);

    result = copyinstr((userptr_t)((char**)args)[i], copy_arr[i], str_len, NULL);
    if (result)
    {
      for (unsigned long j = 0; j <= i; ++j)
      {
        kfree(copy_arr[j]);
      } 
      kfree(copy_arr);
      return result;
    }
  }

  size_t path_len = strlen((char *)progname) + 1;
  char* copy_path = kmalloc(path_len);

  result = copyinstr(progname, copy_path, path_len, NULL);
  if (result)
  {
    for (unsigned long i = 0; i < count; ++i)
    {
      kfree(copy_arr[i]);
    }
    kfree(copy_arr);
    kfree(copy_path);
    return result;
  }

  /* Open the file. */
  result = vfs_open(copy_path, O_RDONLY, 0, &v);
  if (result) {
    for (unsigned long i = 0; i < count; ++i)
    {
      kfree(copy_arr[i]);
    }
    kfree(copy_arr);
    return result;
  }

  /* Create a new address space. */
  as = as_create();
  if (as == NULL) 
  {
    for (unsigned long i = 0; i < count; ++i)
    {
      kfree(copy_arr[i]);
    }
    kfree(copy_arr);
    vfs_close(v);
    return ENOMEM;
  }

  /* Switch to it and activate it. */
  as_old = curproc_getas();
  // destroy the old one
  as_destroy(as_old);
  curproc_setas(as);
  as_activate();


  /* Load the executable. */
  result = load_elf(v, &entrypoint);
  if (result) {
    /* p_addrspace will go away when curproc is destroyed */
    for (unsigned long i = 0; i < count; ++i)
    {
      kfree(copy_arr[i]);
    }
    kfree(copy_arr);
    vfs_close(v);
    return result;
  }

  /* Done with the file now. */
  vfs_close(v);

  /* Define the user stack in the address space */
  result = as_define_stack(as, &stackptr);
  if (result) {
    /* p_addrspace will go away when curproc is destroyed */
    for (unsigned long i = 0; i < count; ++i)
    {
      kfree(copy_arr[i]);
    }
    kfree(copy_arr);
    return result;
  }

  /* copy the arguments into new address space */
  for (unsigned long i = 0; i < count; ++i)
  {
    str_len = strlen(copy_arr[i]) + 1;
    stackptr = stackptr - ROUNDUP(str_len, 8);

    result = copyoutstr(copy_arr[i], (userptr_t)stackptr, str_len, NULL);
    if (result)
    {
      for (unsigned long j = 0; j < count; ++j)
      {
        kfree(copy_arr[j]);
      }
      kfree(copy_arr);
      return result;
    }
    kfree(copy_arr[i]);
    copy_arr[i] = (char *)stackptr;
  }


  // copy arr
  stackptr = stackptr - ROUNDUP(arr_len, 8);
  result = copyout(copy_arr, (userptr_t)stackptr, arr_len);
  if (result)
  {
    kfree(copy_arr);
    return result;
  }

  userptr_t user_stack = (userptr_t)stackptr;
  /* Warp to user mode. */
  enter_new_process(count /*argc*/, user_stack /*userspace addr of argv*/,
        stackptr, entrypoint);
  
  /* enter_new_process does not return. */
  panic("enter_new_process returned\n");
  return EINVAL;
}
#endif
