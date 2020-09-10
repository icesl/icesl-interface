#pragma once

#include "UniqueID.h"

// {1B24298D-B7AF-4fcc-A078-5989597D2E16}
ITF_GUID(Itf_IInterface,
  0x1b24298d, 0xb7af, 0x4fcc, 0xa0, 0x78, 0x59, 0x89, 0x59, 0x7d, 0x2e, 0x16);

class ITF_UUID("{1B24298D-B7AF-4fcc-A078-5989597D2E16}") IInterface
{
public:

  IInterface() = default;
  virtual ~IInterface() = default;

  virtual IInterface *cast(const UniqueID *id) = 0;

  // Not until this is supported with all platforms/compilers
  //template <class T, const UniqueID *pid = &__uuidof(T)>
  //T *cast() {
  //  return (T*)cast(pid);
  //}
};
