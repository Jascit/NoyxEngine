/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   main.cpp
 * \brief  MVP for module initialization, pseudo-code for future implementation
 *
 * Copyright (c) 2026 Jascit
 * \author Jascit <https://github.com/Jascit>
 * \date   January 2026
 * \note   Currently without optimizations and dynamic version checks; needs to be implemented
 */

#define MODULE_ABI_IMPORTS
#include <Windows.h>
#include <module_abi.h>
#include <iostream>
#define CORE_API_VERSION_MAJOR 1
#define CORE_API_VERSION_MINOR 0
#define CORE_API_VERSION_PATCH 0
// test implemantation of core api
void core_push_outgoing(const void* buf, size_t size) {
  if (!buf || size == 0) {
    std::cout << "push_outgoing: invalid buffer or size 0" << std::endl;
    return;
  }
  std::cout << "push_outgoing: buf=" << buf << " size=" << size << std::endl;
}

void core_free(void* ptr) {
  if (!ptr) {
    std::cout << "core_free: null pointer" << std::endl;
    return;
  }
  std::cout << "core_free: ptr=" << ptr << std::endl;
}


uint32_t core_get_generation() {
  std::cout << "core_get_generation" << std::endl;
  return 1;
}

void core_log(int level, const char* msg) {
  if (!msg) msg = "(null)";
  std::cout << "core_log: level=" << level << " msg=" << msg << std::endl;
}

void* core_alloc(size_t);

static CoreAPI core_api = {
  .size = sizeof(CoreAPI),
  .version = (CORE_API_VERSION_MAJOR << 16) | (CORE_API_VERSION_MINOR << 8) | (CORE_API_VERSION_PATCH),
  .alloc = core_alloc,
  .free = core_free,
  .push_outgoing = core_push_outgoing,
  .get_generation = core_get_generation,
  .log = core_log,
};

void* core_alloc(size_t size) {
  if (size == 0) {
    std::cout << "core_alloc: size 0, returning nullptr" << std::endl;
    return nullptr;
  }
  std::cout << "core_alloc: size=" << size << std::endl;
  return std::addressof(core_api);
}

int main() {
  ModuleAPI resource_module = { 0 };

  using init_func = uint32_t(*)(const CoreAPI*, ModuleAPI*);
  HMODULE hLib = LoadLibraryA("NoyxResource.dll");
  if (!hLib) {
    std::cerr << "Failed to load DLL" << std::endl;
    return 1;
  }

  init_func resource_module_init = (init_func)GetProcAddress(hLib, "noyx_module_init");
  if (!resource_module_init) {
    std::cerr << "Failed to find function noyx_module_init" << std::endl;
    FreeLibrary(hLib);
    return 1;
  }

  uint32_t err_code = resource_module_init(&core_api, &resource_module);
  if (err_code != 0) {
    if (err_code == 1) {
      std::cerr << "Core: version mismatch" << std::endl;
    }
    else {
      std::cerr << "Module init failed, code: " << err_code << std::endl;
    }
    FreeLibrary(hLib);
    return static_cast<int>(err_code);
  }

  auto safe_invoke = [](auto fn, const char* name, auto&&... args) {
    if (fn) {
      fn(std::forward<decltype(args)>(args)...);
    }
    else {
      std::cerr << "Cannot call '" << name << "': function pointer is null" << std::endl;
    }
    };

  safe_invoke(resource_module.on_frame, "on_frame", 14.f);
  safe_invoke(resource_module.on_shutdown, "on_shutdown", 10);
  safe_invoke(resource_module.process_command_buffer, "process_command_buffer", static_cast<const void*>(nullptr), static_cast<size_t>(52));

  FreeLibrary(hLib);
  return 0;
}