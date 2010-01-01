// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
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


#include <string>
#include <map>
#include "sem.h"
#include <iostream>

using namespace std;

bool Sem::initialized;
key_t SysVSem::key;
int SysVSem::semid;
string SysVSem::keypath;

Sem::Sem (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
}

Sem::~Sem (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
}

int
Sem::Init (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
Sem::Try (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
Sem::Wait (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
Sem::Destroy (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
Sem::GetValue (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
Sem::Post (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

// SysV style semaphores
SysVSem::SysVSem (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
}

SysVSem::~SysVSem (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
}

int
SysVSem::Init (void) {
  union semun semopts;
  int i;

  key = ftok(".", 's');

  if((semid = semget(key, MEMBERS, IPC_CREAT|IPC_EXCL|0666)) == -1) {
    fprintf(stderr, "Semaphore set already exists!\n");
    exit(1);
  }
  
  semopts.val = 1;
  
  /* Initialize all members (could be done with SETALL) */        
  for(i=0; i < MEMBERS; i++) {
    semctl(semid, i, SETVAL, semopts);
  }
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
SysVSem::Try (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
SysVSem::Wait (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
SysVSem::Destroy (void) {
  return semctl(semid, 0, IPC_RMID, 0);
}

int
SysVSem::GetValue (void) {
  return semctl(semid, MEMBERS, GETVAL, 0);
}

int
SysVSem::Post (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}


// POSIX pthread style semaphores
POSIXSem::POSIXSem (void) {
  Init();
}

POSIXSem::~POSIXSem (void) {
  Destroy();
}

int
POSIXSem::Init (void) {
  // the middle paramemter must be 0 on Linux, cause process
  // side semaphores aren't supported.
  return sem_init(sem, 0, 1);
}

int
POSIXSem::Try (void) {
  //  return sem_trywait(sem);
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
POSIXSem::Wait (void) {
  //  return sem_wait(sem);
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
POSIXSem::Destroy (void) {
  //  return sem_destroy(sem);
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
POSIXSem::GetValue (void) {
  //  return sem_getvalue(sem);
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}

int
POSIXSem::Post (void) {
  cerr << __PRETTY_FUNCTION__ << "ERROR: unimplemented!" << endl;
  return -1;
}



