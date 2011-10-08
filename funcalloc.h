// Utilities to manage function memory. Taken from potion.

#ifdef __MINGW32__
#include <windows.h>
#include <sys/unistd.h>

void *funcalloc(size_t length) { 
  void *mem = VirtualAlloc(NULL, length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  return mem;
}

int funcfree(void *mem, size_t len) {
  return VirtualFree(mem, len, MEM_DECOMMIT) != 0 ? 0 : -1;
}

#else
#include <sys/mman.h>

void *funcalloc(size_t length) {
  void *mem = mmap(NULL, length, PROT_EXEC|PROT_READ|PROT_WRITE, (MAP_PRIVATE|MAP_ANON), -1, 0);
  if (mem == MAP_FAILED) return NULL;
  return mem;
}

int funcfree(void *mem, size_t len) {
  return munmap(mem, len);
}

#endif
