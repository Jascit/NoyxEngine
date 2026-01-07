/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   CoreABI.h
 * \brief  
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   04.01.2026
 * \note   
 */

#ifndef CORE_API_H
#define CORE_API_H

#define CORE_ABI_VERSION_MAJOR 1
#define CORE_ABI_VERSION_MINOR 0
#define CORE_ABI_VERSION_PATCH 0

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus

  typedef struct {
    unsigned int size;
    unsigned int version;
    void* (*alloc)(unsigned long long);
    void     (*free)(void*);
    void     (*push_outgoing)(const void* buf, unsigned long long size);
    unsigned int (*get_generation)();
    void     (*log)(int level, const char* msg);
  } CoreABI;

#ifdef _cplusplus
}
#endif // _cplusplus

#endif // CORE_API_H
