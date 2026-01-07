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

#if defined(_WIN32)
#include <Windows.h>
#include <Platform/Arch.hpp>
#include <Platform/OS.hpp>
using LibraryHandle = HMODULE;
#elif defined(LINUX) || defined(__APPLE__)
#include <dlfcn.h>
using LibraryHandle = void*;
#endif
#if defined(NOYX_CORE_WINDOWS)
constexpr const char* MODULE_NAME = "NoyxResource.dll";
#elif defined(NOYX_CORE_MACOS) || defined(NOYX_CORE_APPLE) || defined(NOYX_CORE_IOS)
constexpr const char* MODULE_NAME = "libNoyxResource.dylib";
#else
constexpr const char* MODULE_NAME = "libNoyxResource.so";
#endif
#define MODULE_ABI_IMPORTS
#include <ModuleABI.h>
#include <iostream>

// test implemantation of core api
void core_push_outgoing(const void* buf, unsigned long long size) {
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


unsigned int core_get_generation() {
  std::cout << "core_get_generation" << std::endl;
  return 1;
}

void core_log(int level, const char* msg) {
  if (!msg) msg = "(null)";
  std::cout << "core_log: level=" << level << " msg=" << msg << std::endl;
}

void* core_alloc(unsigned long long);

static CoreABI core_api = {
  .size = sizeof(CoreABI),
  .version = (CORE_ABI_VERSION_MAJOR << 16) | (CORE_ABI_VERSION_MINOR << 8) | (CORE_ABI_VERSION_PATCH),
  .alloc = core_alloc,
  .free = core_free,
  .push_outgoing = core_push_outgoing,
  .get_generation = core_get_generation,
  .log = core_log,
};

void* core_alloc(unsigned long long size) {
  if (size == 0) {
    std::cout << "core_alloc: size 0, returning nullptr" << std::endl;
    return nullptr;
  }
  std::cout << "core_alloc: size=" << size << std::endl;
  return std::addressof(core_api);
}

LibraryHandle load_library(const char* path) {
#if defined(_WIN32)
  return LoadLibraryA(path);
#else
  return dlopen(path, RTLD_NOW);
#endif
}

void* load_symbol(LibraryHandle lib, const char* name) {
#if defined(_WIN32)
  return reinterpret_cast<void*>(GetProcAddress(lib, name));
#else
  return dlsym(lib, name);
#endif
}

void unload_library(LibraryHandle lib) {
#if defined(_WIN32)
  FreeLibrary(lib);
#else
  dlclose(lib);
#endif
}

int main() {
  ModuleABI resource_module = {};

  using init_func = unsigned int(*)(const CoreABI*, ModuleABI*);

  LibraryHandle lib = load_library(MODULE_NAME);
  if (!lib) {
#if !defined(_WIN32)
    std::cerr << "dlopen error: " << dlerror() << std::endl;
#else
    std::cerr << "Failed to load library" << std::endl;
#endif
    return 1;
  }

  auto resource_module_init =
    reinterpret_cast<init_func>(load_symbol(lib, "noyx_module_init"));

  if (!resource_module_init) {
    std::cerr << "Failed to find symbol noyx_module_init" << std::endl;
    unload_library(lib);
    return 1;
  }

  unsigned int err = resource_module_init(&core_api, &resource_module);
  if (err != 0) {
    std::cerr << "Module init failed: " << err << std::endl;
    unload_library(lib);
    return static_cast<int>(err);
  }

  auto safe_invoke = [](auto fn, const char* name, auto&&... args) {
    if (fn) fn(std::forward<decltype(args)>(args)...);
    else std::cerr << "Cannot call " << name << ": null pointer" << std::endl;
    };

  safe_invoke(resource_module.on_frame, "on_frame", 14.f);
  safe_invoke(resource_module.on_shutdown, "on_shutdown", 10);
  safe_invoke(resource_module.process_command_buffer,
    "process_command_buffer", nullptr, 52ull);

  unload_library(lib);
  return 0;
}