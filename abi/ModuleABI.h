/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   module_abi.h
 * \brief  ABI-stable interface
 *
 * Copyright (c) 2026 Jascit
 * \author Jascit<https://github.com/Jascit>
 * \date   04.01.2026
 * \note   Do not break ABI, use pod instead.
 */

#ifndef MODULE_ABI_H
#define MODULE_ABI_H
#include "CoreABI.h" 

#if defined(_WIN32) || defined(__CYGWIN__)
  #if defined(MODULE_ABI_EXPORTS)
    #define MODULE_API __declspec(dllexport)
  #elif defined(MODULE_ABI_IMPORTS)
    #define MODULE_API __declspec(dllimport)
  #else
    #define MODULE_API
  #endif
#else
  #if defined(__GNUC__) && __GNUC__ >= 4
    #define MODULE_API __attribute__((visibility("default")))
  #else
    #define MODULE_API
  #endif
#endif

#define MODULE_ABI_VERSION_MAJOR 1
#define MODULE_ABI_VERSION_MINOR 0
#define MODULE_ABI_VERSION_PATCH 0

#ifdef __cplusplus
extern "C" {
#endif
  typedef struct {
    unsigned int size;
    unsigned int version;
    void (*process_command_buffer)(const void* buf, unsigned long long size);
    void (*on_frame)(float dt);
    void (*on_shutdown)(unsigned int generation);
  } ModuleABI;

  MODULE_API unsigned int noyx_module_init(const CoreABI* core, ModuleABI* out);
#ifdef __cplusplus
}
#endif

#endif // MODULE_ABI_H
