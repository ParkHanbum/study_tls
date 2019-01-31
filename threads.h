/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBS_CUTILS_THREADS_H
#define _LIBS_CUTILS_THREADS_H

/***********************************************************************/
/***********************************************************************/
/*****                                                             *****/
/*****         local thread storage                                *****/
/*****                                                             *****/
/***********************************************************************/
/***********************************************************************/

#include  <pthread.h>

typedef void  (*thread_store_destruct_t)(void*  value);

/***********************************************************************/
/***********************************************************************/
/*****                                                             *****/
/*****         mutexes                                             *****/
/*****                                                             *****/
/***********************************************************************/
/***********************************************************************/

typedef pthread_mutex_t   mutex_t;


static __inline__ void  mutex_lock(mutex_t*  lock)
{
    pthread_mutex_lock(lock);
}
static __inline__ void  mutex_unlock(mutex_t*  lock)
{
    pthread_mutex_unlock(lock);
}
static __inline__ int  mutex_init(mutex_t*  lock)
{
    return pthread_mutex_init(lock, NULL);
}
static __inline__ void mutex_destroy(mutex_t*  lock)
{
    pthread_mutex_destroy(lock);
}


#ifdef __cplusplus
}
#endif

#endif /* _LIBS_CUTILS_THREADS_H */
