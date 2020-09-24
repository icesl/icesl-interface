/*
 *  Copyright (c) 2020, INRIA
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <tuple>

#ifdef WIN32
#ifdef ICESL_CONSUME_API
#define ICESL_PUBLIC_CLASS __declspec(dllimport)
#else
#define ICESL_PUBLIC_CLASS __declspec(dllexport)
#endif
#else
#define ICESL_PUBLIC_CLASS __attribute__((visibility("default")))
#endif

#ifdef WIN32
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#endif

/// Class structure to generate a Unique ID
struct ICESL_PUBLIC_CLASS _UniqueID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];

    // Creates a UniqueID guaranteed to be unique within the current process, and for all currently loaded modules
#ifdef WIN32
    void generate()
    {
      Data1 = 0;
      Data2 = Data3 = 0;
      for (int i = 0; i < 8; i++) {
        Data4[i] = 0;
      }
      ::UuidCreateSequential((UUID*)&Data1);
    }
#else
    static long UniqueData1;
    void generate()
    {
      Data1 = UniqueData1++;
      Data2 = Data3 = 0;
      for (int i=0; i < 8; i++)
        Data4[i] = 0;
    }
#endif

    bool operator<(const struct _UniqueID& rhs) const
    {
      return std::tie(Data1, Data2, Data3, Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7])
        <    std::tie(rhs.Data1, rhs.Data2, rhs.Data3, rhs.Data4[0], rhs.Data4[1], rhs.Data4[2], rhs.Data4[3], rhs.Data4[4], rhs.Data4[5], rhs.Data4[6], rhs.Data4[7]);
    }

    bool operator==(const struct _UniqueID& rhs) const
    {
      return Data1 == rhs.Data1  && 
        Data2 == rhs.Data2       &&
        Data3 == rhs.Data3       &&
        Data4[0] == rhs.Data4[0] &&
        Data4[1] == rhs.Data4[1] &&
        Data4[2] == rhs.Data4[2] &&
        Data4[3] == rhs.Data4[3] &&
        Data4[4] == rhs.Data4[4] &&
        Data4[5] == rhs.Data4[5] &&
        Data4[6] == rhs.Data4[6] &&
        Data4[7] == rhs.Data4[7];
    }

    std::string toString()
    {
      std::ostringstream struid;
      struid << std::hex << std::setfill('0') << std::setw(2);
      for (unsigned int i = 0u; i < sizeof(long);i++) {
        struid << (unsigned int)((Data1 >> (i * 8))&255u);
      }
      for (unsigned int i = 0u; i < sizeof(short); i++) {
        struid << (unsigned int)((Data2 >> (i * 8)) & 255u);
      }
      for (unsigned int i = 0u; i < sizeof(short); i++) {
        struid << (unsigned int)((Data3 >> (i * 8)) & 255u);
      }
      for (unsigned int i = 0u; i < 8u; i++) {
        struid << (unsigned int)(Data4[i]);
      }
      return struid.str();
    }

};

typedef struct _UniqueID UniqueID;

#define ITF_EXTERN_C extern "C" 

#ifdef ITF_INITGUID
#define ITF_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
         ITF_EXTERN_C const UniqueID name \
                 = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7, b8 } }
#else
#define ITF_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
     ITF_EXTERN_C const UniqueID name
#endif // ITF_INITGUID

#define IsEqualUniqueID(a,b) ( !memcmp(&(a), &(b), sizeof(UniqueID)) )

//#ifdef WIN32
//#define ITF_UUID(u) __declspec(uuid(u))
//#else
#define ITF_UUID(u)
//#endif
