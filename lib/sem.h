// 
// Copyright (C) 2005 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

// SysV semaphores
//
// key_t ftok(char *pathname, char proj_id);
//	The most commonly used method of generating an IPC key that is
//	reasonably unique.
//
// int semget ( key_t key, int nsems, int semflg )
//	The  function returns the semaphore set identifier associated
//	to the value of the argument key.
//
// int  semctl  (int  semid, int semnum, int cmd, union semun arg)
//	The function performs the control operation  specified  by
//	cmd on the semaphore set (or on the semnum-th semaphore of
//	the set) identified by semid.  The first semaphore of  the
//	set is indicated by a value 0 for semnum.
//
// int semop ( int semid, struct sembuf *sops, unsigned nsops)
//	The function performs operations on  selected  members  of the
//	semaphore  set indicated by semid.  Each of the nsops elements
//	in the array pointed to by sops specify an operation  to  be
//	performed  on a semaphore by a struct sembuf including the
//	following members:
//
//  POSIX semaphores
// 
// int sem_init(sem_t *sem, int pshared, unsigned int value);
// 
//     * sem points to a semaphore object to initialize
//     * pshared is a flag indicating whether or not the semaphore should
//     be shared with fork()ed processes. LinuxThreads does not currently
//     support shared semaphores
//     * value is an initial value to set the semaphore to
// 
// int sem_wait(sem_t *sem);
// 
//     * sem_wait blocks until the specified semaphore object's value is
//     greater than zero. it then decrements the semaphore's value by one 
//     and returns 
// 
// int sem_trywait(sem_t *sem);
// 
//     * like sem_wait, but returns immediately and does not decrement
//     the semaphore if it is zero 
// 
// int sem_post(sem_t *sem);
// 
//     * increments the value of a semaphore
// 
// int sem_getvalue(sem_t *sem, int *valp);
// 
//     * gets the current value of sem and places it in the location
//     pointed to by valp
// 
// int sem_destroy(sem_t *sem);
// 
//     * destroys the semaphore; no threads should be waiting on the
//     semaphore if its destruction is to succeed.
//


// This shuts up warnings about virtual functions. We don't care,
// because this isn't implemented yet.
//#warning "Unimplemented"
#pragma GCC system_header

extern "C" {
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
  /* union semun is defined by including <sys/sem.h> */
#else
  /* according to X/OPEN we have to define it ourselves */
  union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
  };
#endif
#include <semaphore.h>
}

#include <string>
#include <map>

#define MEMBERS 1

class Sem {
 private:
  static bool initialized;
  std::map<std::string, int> tbl;
 public:
  Sem (void);
  ~Sem (void);
  virtual int Init (void);
  virtual int Try (void);
  virtual int Wait (void);
  virtual int Destroy (void);
  virtual int GetValue (void);
  virtual int Post (void);
};

class SysVSem: public Sem {
 private:
  static key_t key;
  static int semid;
  static std::string keypath;
 public:
  SysVSem (void);
  ~SysVSem (void);
  int Init (void);
  int Try (void);
  int Wait (void);
  int Destroy (void);
  int GetValue (void);
  int Post (void);
};

class POSIXSem: public Sem {
 private:
  sem_t *sem;
 public:
  POSIXSem (void);
  ~POSIXSem (void);
  int Init (void);
  int Try (void);
  int Wait (void);
  int Destroy (void);
  int GetValue (void);
  int Post (void);
};

