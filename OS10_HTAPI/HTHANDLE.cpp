#include "pch.h" 
#include "HT.h"
using namespace HT;

HTHANDLE::HTHANDLE() :
    Capacity(0),
    SecSnapshotInterval(0),
    MaxKeyLength(0),
    MaxPayloadLength(0),
    File(INVALID_HANDLE_VALUE),
    FileMapping(NULL),
    Addr(NULL),
    lastsnaptime(0)
{
    FileName[0] = '\0';
    LastErrorMessage[0] = '\0';
}

HTHANDLE::HTHANDLE(int capacity, int secSnapshotInterval, int maxKeyLength,
    int maxPayloadLength, const char fileName[SIZE]) :
    Capacity(capacity),
    SecSnapshotInterval(secSnapshotInterval),
    MaxKeyLength(maxKeyLength),
    MaxPayloadLength(maxPayloadLength),
    File(INVALID_HANDLE_VALUE),
    FileMapping(NULL),
    Addr(NULL),
    lastsnaptime(0)
{
    strncpy_s(FileName, fileName, SIZE);
    LastErrorMessage[0] = '\0';
}