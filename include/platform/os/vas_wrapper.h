/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     vas_wrapper.h
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#ifndef NOYX_VAS_WRAPPER_H
#define NOYX_VAS_WRAPPER_H
#ifdef __cplusplus
#include <cstdint>

extern "C" {
#else
#include <stdint.h>
#endif
//TODO: support for huge pages, code doesn't work

typedef enum {
  VAW_OK = 0,
  VAW_ERR_INVALID_ARG,
  VAW_ERR_INVALID_ADDRESS,
  VAW_ERR_OOM,
  VAW_ERR_NOT_COMMITTED,
  VAW_ERR_ALREADY_COMMITTED,
  VAW_ERR_NO_MAPPING,
  VAW_ERR_PERMISSION,
  VAW_ERR_UNSUPPORTED,
  VAW_ERR_PLATFORM,
  VAW_ERR_INTERNAL
} vaw_err_t;

typedef enum {
  VAW_FLAG_NONE = 0u,
  VAW_FLAG_PROT_NONE = 1u << 0, /* NOACCESS (default) */
  VAW_FLAG_PROT_READ = 1u << 1,
  VAW_FLAG_PROT_WRITE = 1u << 2,
  VAW_FLAG_PROT_EXEC = 1u << 3,

  /* Verhalten / Hints */
  VAW_FLAG_FIXED_ADDRESS = 1u << 8, /* erzwinge (!) preferred_addr */
  VAW_FLAG_PREFER_ADDRESS = 1u << 9, /* prefer preferred_addr wenn möglich */
  VAW_FLAG_LARGE_PAGES = 1u << 16,
  VAW_FLAG_64K_PAGES = 1u << 17
} vaw_flag_t;

typedef struct {
  uint64_t size;
  uint64_t alignment;
  void* preferred_addr;
  uint32_t alloc_flags;
} vaw_reserve_req_t;

typedef struct {
  void* base;
  uint64_t size;
  vaw_err_t err;
} vaw_reserve_resp_t;

vaw_reserve_resp_t vaw_reserve_memory(const vaw_reserve_req_t* req, uint64_t pg_size);

typedef struct {
  void* base;
  uint64_t size;
} vaw_release_req_t;

typedef struct {
  vaw_err_t err;
} vaw_release_resp_t;

vaw_release_resp_t vaw_release_memory(const vaw_release_req_t* req);

typedef struct {
  void* base;
  uint64_t offset;
  uint64_t size;
  uint32_t prot;
  uint32_t alloc_flags;
} vaw_commit_req_t;

typedef struct {
  vaw_err_t err;
} vaw_commit_resp_t;

vaw_commit_resp_t vaw_commit_pages(const vaw_commit_req_t* req, uint64_t pg_size);
vaw_commit_resp_t vaw_decommit_pages(const vaw_commit_req_t* req);

typedef struct {
  void* base;
  uint64_t offset;
  uint64_t size;
  uint32_t prot;
} vaw_map_req_t;

typedef struct {
  vaw_err_t err;
  uint64_t ptr;
} vaw_map_resp_t;

vaw_map_resp_t vaw_map(const vaw_map_req_t* req);

typedef struct {
  void* ptr;
  uint64_t size;
} vaw_unmap_req_t;

typedef struct {
  vaw_err_t err;
} vaw_unmap_resp_t;

vaw_unmap_resp_t vaw_unmap(const vaw_unmap_req_t* req);

typedef struct {
  void* base;
  uint64_t offset;
  uint64_t size;
  int advice;
} vaw_advise_req_t;

typedef struct {
  vaw_err_t err;
} vaw_advise_resp_t;

vaw_advise_resp_t vaw_advise(const vaw_advise_req_t* req);

uint64_t page_size();

#ifdef __cplusplus
}
#endif

#endif
