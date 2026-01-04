/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   module_abi.h
 * \brief  ABI-stable interface
 *
 * Copyright (c) 2026 Jascit
 * \author Jascit<https://github.com/Jascit>
 * \date   January 2026
 * \note   Do not break ABI, use pod instead.
 */

#ifndef MODULE_ABI_H
#define MODULE_ABI_H

#define MODULE_ABI_VERSION_MAJOR 1
#define MODULE_ABI_VERSION_MINOR 0
#define MODULE_ABI_VERSION_PATCH 0

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

#ifdef __cplusplus
extern "C" {
#endif
  typedef unsigned int uint32_t;
  typedef unsigned long long size_t;

  typedef struct {
    uint32_t size;
    uint32_t version;
    void (*process_command_buffer)(const void* buf, size_t size);
    void (*on_frame)(float dt);
    void (*on_shutdown)(uint32_t generation);
  } ModuleAPI;

  typedef struct {
    uint32_t size;
    uint32_t version;
    void* (*alloc)(size_t);
    void     (*free)(void*);
    void     (*push_outgoing)(const void* buf, size_t size);
    uint32_t (*get_generation)();
    void     (*log)(int level, const char* msg);
  } CoreAPI;

  MODULE_API uint32_t noyx_module_init(const CoreAPI* core, ModuleAPI* out);

#ifdef __cplusplus
}
#endif

#endif // MODULE_ABI_H
