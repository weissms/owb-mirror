/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef BITHREAD_H_
#define BITHREAD_H_

#ifndef __BAL_THREAD_POSIX__
#include <pthread.h>
#else

#define __SIZEOF_PTHREAD_ATTR_T 36
#define PTHREAD_ONCE_INIT 0
#define __SIZEOF_PTHREAD_MUTEX_T 24
#define __SIZEOF_PTHREAD_MUTEXATTR_T 4


/* Mutex types.  */
enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
#ifdef __USE_UNIX98
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#endif
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
};


/* Mutex initializers.  */
#define PTHREAD_MUTEX_INITIALIZER \
  { { 0, 0, 0, 0, 0, 0 } }
#ifdef __USE_GNU
# if __WORDSIZE == 64
#  define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0 } }
#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, 0 } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, 0 } }
# else
#  define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0, 0 } }
#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, 0, 0 } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, 0, 0 } }
# endif
#endif




typedef unsigned long int pthread_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
/* Data structures for mutex handling.  The structure of the attribute
   type is not exposed on purpose.  */
typedef union
{
  struct
  {
    int __lock;
    unsigned int __count;
    int __owner;
    /* KIND must stay at this position in the structure to maintain
       binary compatibility.  */
    int __kind;
    unsigned int __nusers;
    int __spins;
  } __data;
  char __size[__SIZEOF_PTHREAD_MUTEX_T];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
} pthread_attr_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
  long int __align;
} pthread_mutexattr_t;

namespace BAL {

    /**
    * @brief the BIThread
    *
    * The thraad base class
    *
    */
    class BIThread {
        public:
        // this is mandatory
            ~BIThread(){}
            /* Create a key value identifying a location in the thread-specific
               data area.  Each thread maintains a distinct thread-specific data
               area.  DESTR_FUNCTION, if non-NULL, is called with the value
               associated to that key when the key is destroyed.
               DESTR_FUNCTION is not called if the value associated is NULL when
               the key is destroyed.  */

            static int pthread_key_create (pthread_key_t *__key,
                               void (*__destr_function) (void *));
            /*Guarantee that the initialization function INIT_ROUTINE will be called
              only once, even if pthread_once is executed several times with the
              same ONCE_CONTROL argument. ONCE_CONTROL must point to a static or
              extern variable initialized to PTHREAD_ONCE_INIT.*/
            static int pthread_once (pthread_once_t *__once_control,
                         void (*__init_routine) (void));

            /* Return current value of the thread-specific data slot identified by KEY.  */
            static void *pthread_getspecific (pthread_key_t __key);

            /* Obtain the identifier of the current thread.  */
            static pthread_t pthread_self (void);

            /* Store POINTER in the thread-specific data slot identified by KEY. */
            static int pthread_setspecific (pthread_key_t __key,
                                __const void *__pointer);

            /* Return the previously set address for the stack.  */
            /* pthread_attr_getstackaddr is deprecated */
            /* static int pthread_attr_getstackaddr (__const pthread_attr_t *attr, void **__stackaddr); */
	    static int pthread_attr_getstack(const pthread_attr_t *__attr, void **__stackaddr, size_t *__stacksize); 

            /* Initialize mutex attribute object ATTR with default attributes
               (kind is PTHREAD_MUTEX_TIMED_NP).  */
            static int pthread_mutexattr_init (pthread_mutexattr_t *__attr);

            /* Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
               PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or
               PTHREAD_MUTEX_DEFAULT).  */
            static int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind);

            /* Initialize a mutex.  */
            static int pthread_mutex_init (pthread_mutex_t *__mutex,
                               __const pthread_mutexattr_t *__mutexattr);

            /* Destroy a mutex.  */
            static int pthread_mutex_destroy (pthread_mutex_t *__mutex);

            /* Lock a mutex.  */
            static int pthread_mutex_lock (pthread_mutex_t *__mutex);

            /* Unlock a mutex.  */
            static int pthread_mutex_unlock (pthread_mutex_t *__mutex);
    };
}




inline int pthread_key_create (pthread_key_t *__key,
                               void (*__destr_function) (void *))
{
    return BAL::BIThread::pthread_key_create( __key, __destr_function );
}


inline int pthread_once (pthread_once_t *__once_control,
                         void (*__init_routine) (void))
{
    return BAL::BIThread::pthread_once( __once_control, __init_routine );
}

inline void *pthread_getspecific (pthread_key_t __key)
{
    return BAL::BIThread::pthread_getspecific( __key );
}

inline pthread_t pthread_self (void)
{
    return BAL::BIThread::pthread_self();
}

inline int pthread_setspecific (pthread_key_t __key,
                                __const void *__pointer)
{
    return BAL::BIThread::pthread_setspecific ( __key, __pointer);
}

/* inline int pthread_attr_getstackaddr (__const pthread_attr_t *__attr, void **__stackaddr)
{
    return BAL::BIThread::pthread_attr_getstackaddr (__attr, __stackaddr);
}*/

inline int pthread_attr_getstack(const pthread_attr_t *__attr, void **__stackaddr, size_t *__stacksize) 
{
	return BAL::BIThread::pthread_attr_getstack(__attr, __stackaddr, __stacksize);
}

inline int pthread_mutexattr_init (pthread_mutexattr_t *__attr)
{
    return BAL::BIThread::pthread_mutexattr_init( __attr );
}

inline int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind)
{
    return BAL::BIThread::pthread_mutexattr_settype( __attr, __kind );
}

inline int pthread_mutex_init (pthread_mutex_t *__mutex,
                               __const pthread_mutexattr_t *__mutexattr)
{
    return BAL::BIThread::pthread_mutex_init( __mutex, __mutexattr);
}

inline int pthread_mutex_lock (pthread_mutex_t *__mutex)
{
    return BAL::BIThread::pthread_mutex_lock( __mutex );
}

inline int pthread_mutex_unlock (pthread_mutex_t *__mutex)
{
    return BAL::BIThread::pthread_mutex_unlock( __mutex );
}

inline int pthread_mutex_destroy( pthread_mutex_t *__mutex)
{
    return BAL::BIThread::pthread_mutex_destroy( __mutex );
}

#endif // __THREAD__
#endif // BITHREAD_H_
