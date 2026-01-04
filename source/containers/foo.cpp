#define MODULE_ABI_IMPORTS
#include <iostream>
#include <module_abi.h>
#define CORE_API_VERSION_MAJOR 1
#define CORE_API_VERSION_MINOR 0
#define CORE_API_VERSION_PATCH 0
// test implemantation of core api
void     core_push_outgoing(const void* buf, size_t size) { std::cout << "push_outgoing"; };
void     core_free(void*) { std::cout << "core_free"; };
void*    core_alloc(size_t);
uint32_t core_get_generation() { std::cout << "core_get_generation"; return 1; };
void     core_log(int level, const char* msg) { std::cout << "core_log"; };

static CoreAPI core_api = {
  .size = sizeof(CoreAPI),
  .version = (CORE_API_VERSION_MAJOR << 16) | (CORE_API_VERSION_MINOR << 8) | (CORE_API_VERSION_PATCH),
  .alloc = core_alloc,
  .free = core_free,
  .push_outgoing = core_push_outgoing,
  .get_generation = core_get_generation,
  .log = core_log,
};

void*    core_alloc(size_t) { std::cout << "core_alloc"; return std::addressof(core_api); };
