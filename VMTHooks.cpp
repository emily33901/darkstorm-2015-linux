
//#include "../libex.h"
#include "VMTHooks.h"
#include "memory.h"
#include <cstdlib>

namespace toolkit {

unsigned int CountFuncs(void **vmt) {
  unsigned int i = -1;
  do
    ++i;
  while (vmt[i]);
  return i;
}

int FindFunc(void **vmt, void *func, unsigned int vfuncs) {
  if (!vfuncs)
    vfuncs = CountFuncs(vmt);
  for (unsigned int i = 0; i < vfuncs; i++) {
    if (vmt[i] == func)
      return i;
  }
  return -1;
}

// VMTBaseManager
VMTBaseManager &VMTBaseManager::Init(void *inst, unsigned int offset,
                                     unsigned int vfuncs) {
  _vftable = &_getvtbl(inst, offset);
  _oldvmt = *_vftable;
  // Count vfuncs ourself if needed
  if (!vfuncs) {
    vfuncs = CountFuncs(_oldvmt);
    assert(vfuncs >= 1);
  }
  // Allocate room for the new vtable + 3 places before + 1 place after for a
  // null ptr
  void **arr = _array = (void **)::malloc((vfuncs + 4) * sizeof(void *));
  arr[0] = this;            // Ptr back to the hook object
  arr[1] = (void *)GUARD;   // Guard tells us if the vtable is hooked
  (arr + 3)[vfuncs] = NULL; // Marks the end of the vtable
  // Copy over the other vfuncs + RTTI ptr
  {
    unsigned int i = -1;
    do
      arr[i + 3] = _oldvmt[i];
    while (++i < vfuncs);
  }
  return *this;
}
void VMTBaseManager::Kill() {
  if (IsInitialized()) {
    Unhook();
    _vftable = NULL;
  }
  ::free(_array);
  _array = NULL;
}
void VMTBaseManager::EraseHooks() {
  unsigned int i = 0;
  void **vmt = _array + 3;
  do
    vmt[i] = _oldvmt[i];
  while (vmt[++i]);
}
}
