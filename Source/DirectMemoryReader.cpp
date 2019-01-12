#include "rFactor2SharedMemoryMap.hpp"          // corresponding header file
#include <stdlib.h>
#include <cstddef>                              // offsetof
#include "DirectMemoryReader.h"
#include "Utils.h"

bool DirectMemoryReader::Initialize()
{
  DEBUG_MSG(DebugLevel::DevInfo, "Initializing DMR.");
  
  auto const startTicks = TicksNow();
  auto const module = ::GetModuleHandle(nullptr);
  mpStatusMessage = reinterpret_cast<char*>(FindPatternForPointerInMemory(module,
      (unsigned char *)"\x74\x23\x48\x8D\x15\x5D\x31\xF5\x00\x48\x2B\xD3",
      (CHAR *)"xxxxx????xxx", 5));

  // TODO: store ** and derefence it all the time, will be safer.
  auto const rootPtr = FindPatternForPointerInMemory(module,
    (unsigned char *)"\x48\x8B\x05\xAA\xD1\xFF\x00\xC6\x80\xB8\x25\x00\x00\x01",
    (CHAR *)"xxx????xxxxxxx", 3);

  auto const endTicks = TicksNow();

  // TODO: verify math.
  DEBUG_FLOAT2(DebugLevel::DevInfo, "Scan time seconds: ", (endTicks - startTicks) / MICROSECONDS_IN_SECOND);

  mpMessageCenterMessages = *reinterpret_cast<char**>(rootPtr);

  auto const addr1 = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(::GetModuleHandle(nullptr)) + 0x14BA0C0);
  auto const addr2 = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(::GetModuleHandle(nullptr)) + 0x14B8738);

  DEBUG_ADDR2(DebugLevel::DevInfo, "A1", mpStatusMessage);
  DEBUG_ADDR2(DebugLevel::DevInfo, "A11", addr1);
  DEBUG_ADDR2(DebugLevel::DevInfo, "O1", reinterpret_cast<uintptr_t>(mpStatusMessage) - reinterpret_cast<uintptr_t>(module));
  DEBUG_ADDR2(DebugLevel::DevInfo, "O11", 0x14BA0C0uLL);

  DEBUG_ADDR2(DebugLevel::DevInfo, "A2", mpMessageCenterMessages);
  DEBUG_ADDR2(DebugLevel::DevInfo, "A21", addr2);
  DEBUG_ADDR2(DebugLevel::DevInfo, "O2", reinterpret_cast<uintptr_t>(rootPtr) - reinterpret_cast<uintptr_t>(module));
  DEBUG_ADDR2(DebugLevel::DevInfo, "O21", 0x14B8738uLL);

  return true;
}


void DirectMemoryReader::Read(rF2Extended& extended)
{
  strcpy_s(extended.mStatusMessage, mpStatusMessage);
  if (strcmp(mPrevStatusMessage, extended.mStatusMessage) != 0) {
    DEBUG_MSG2(DebugLevel::DevInfo, "Status message updated: ", extended.mStatusMessage);

    strcpy_s(mPrevStatusMessage, extended.mStatusMessage);
    extended.mTicksStatusMessage = ::GetTickCount64();
  }
}