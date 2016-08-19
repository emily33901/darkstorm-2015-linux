#pragma once

//----------------------------------------------------------------
// Toolkit: Virtual method table hooks
//----------------------------------------------------------------
// These can be hooked in two distinct manners.
//
// The VMTManager class hooks by replacing the vmt pointer with a custom crafted
// one.
// Only calls through this instance will be hooked.
//
// The VMTPointer class hooks a pointer to an interface.
// Only calls through this specific variable will be hooked.
//
// The VMTHook class hooks by editing the vtable in the dll's .rdata.
// Every instance using that vtable will be hooked.
//
// A general hint:
// Abuse the __fastcall convention ( look it up on the msdn ) to catch the this
// pointer.
//
// More specific information follows the class declaration.
//
// Each class has a basic version (no constructors/destructors) and a RAII
// version.
//
// This code is public domain but credits (to Casual_Hacker on
// www.gamedeception.net) are always appreciated.

#include <cassert>
#include <cstddef>

namespace toolkit {

// Find the number of vfuncs in a vtable
unsigned int CountFuncs(void **pVMT);

// Find the index for a vfunc, result is negative if not found
int FindFunc(void **pVMT, void *pFunc, unsigned int vfuncs = 0);

//----------------------------------------------------------------
// Class: VMTManager
//----------------------------------------------------------------
// Hooks virtual functions by replacing the vtable pointer from an instance.
// VMTManager is the RAII version of VMTBaseManager
class VMTBaseManager {
public:
  // TODO: Abstractions can never hurt...
  // typedef const void* pfn_t;
  // struct vtbl_t
  //{
  //	pfn_t data[1];
  //	inline const pfn_t& operator[] ( unsigned int i ) const { return
  //data[i]; }
  //	inline pfn_t& operator[] ( unsigned int i ) { return data[i]; }
  //};

  // Guard identifies if an instance is hooked.
  // enum { GUARD = __COUNTER__ * 0xDE4DB3EF + 12345 }; // This should be random
  // for each project... But is this safe?
  enum { GUARD = 0xDE4DB3EF };

  // Init the hook, will not hook the instance yet.
  VMTBaseManager &Init(void *inst, unsigned int offset = 0,
                       unsigned int vfuncs = 0);
  // Unhook and forget about this instance.
  void Kill();
  // Are we initialized.
  bool IsInitialized() const;

  // Hooks a function by index.
  VMTBaseManager &HookMethod(void *newfunc, unsigned int index);
  // Unhooks a function by index.
  VMTBaseManager &UnhookMethod(unsigned int index);
  // Unhooks all functions.
  void EraseHooks();

  // Return the original vtable. Instance will appear completely unhooked.
  VMTBaseManager &Unhook();
  // Put the vtable with the hooks back.
  VMTBaseManager &Rehook();
  // Test if the hooks are active.
  bool Hooked() const;

  // Get the original function.
  // Use a function prototype for the template argument to make it very easy to
  // call this function.
  // Example syntax: hook.GetMethod<bool (__thiscall*)( void*, int )>( 12 )(
  // inst, arg );
  template <typename Fn> const Fn &GetMethod(unsigned int index) const;

  // Get/Set the userdata.
  void *UserData() const;
  void UserData(void *data);

  // Test if the instance is hooked.
  static bool HookPresent(void *inst, unsigned int offset = 0);
  // Get the hook object. Does not check if the instance is actually hooked!
  static VMTBaseManager &GetHook(void *inst, unsigned int offset = 0);

protected:
  static void **&_getvtbl(void *inst, unsigned int offset);

protected:
  void ***_vftable;
  void **_oldvmt;
  void **_array;
  void *_userdata;
};
class VMTManager : public VMTBaseManager {
  // Forbid copy constructing and assignment.
  VMTManager(const VMTManager &);
  VMTManager &operator=(const VMTManager &);

public:
  // Initialize & hook the instance.
  VMTManager(void *inst, unsigned int offset = 0, unsigned int vfuncs = 0);
  ~VMTManager();

  // If the instance is somehow destroyed before you get a chance to unhook it
  // or destruct this hook object, call this.
  // It'll prevent the destructor from crashing.
  void Poof();

  // Get the hook object. Does not check if the instance is actually hooked!
  static VMTManager &GetHook(void *inst, unsigned int offset = 0);
};
// VMTBaseManager inlines
inline bool VMTBaseManager::IsInitialized() const {
  return _vftable != NULL;
}
inline VMTBaseManager &VMTBaseManager::HookMethod(void *newfunc,
                                                  unsigned int index) {
  assert(index < CountFuncs(_array + 3) && IsInitialized());
  _array[index + 3] = newfunc;
  return *this;
}
inline VMTBaseManager &VMTBaseManager::UnhookMethod(unsigned int index) {
  assert(index < CountFuncs(_array + 3) && IsInitialized());
  _array[index + 3] = _oldvmt[index];
  return *this;
}
inline VMTBaseManager &VMTBaseManager::Unhook() {
  *_vftable = _oldvmt;
  return *this;
}
inline VMTBaseManager &VMTBaseManager::Rehook() {
  *_vftable = _array + 3;
  return *this;
}
inline bool VMTBaseManager::Hooked() const { return *_vftable != _oldvmt; }
template <typename Fn>
inline const Fn &VMTBaseManager::GetMethod(unsigned int index) const {
  assert(index < CountFuncs(_array + 3));
  return *(const Fn *)&_oldvmt[index];
}
inline void *VMTBaseManager::UserData() const { return _userdata; }
inline void VMTBaseManager::UserData(void *data) { _userdata = data; }
inline bool VMTBaseManager::HookPresent(void *inst, unsigned int offset) {
  void **vmt = _getvtbl(inst, offset);
  return vmt[-2] == (void *)GUARD;
}
inline VMTBaseManager &VMTBaseManager::GetHook(void *inst,
                                               unsigned int offset) {
  void **vmt = _getvtbl(inst, offset);
  return *reinterpret_cast<VMTBaseManager *>(vmt[-3]);
}
inline void **&VMTBaseManager::_getvtbl(void *inst, unsigned int offset) {
  return *reinterpret_cast<void ***>((char *)inst + offset);
}
// VMTManager inlines
inline VMTManager::VMTManager(void *inst, unsigned int offset,
                              unsigned int vfuncs) {
  Init(inst, offset, vfuncs).Rehook();
}
inline VMTManager::~VMTManager() { Kill(); }
inline void VMTManager::Poof() { _vftable = NULL; }
inline VMTManager &VMTManager::GetHook(void *inst, unsigned int offset) {
  return static_cast<VMTManager &>(VMTBaseManager::GetHook(inst, offset));
}
}
