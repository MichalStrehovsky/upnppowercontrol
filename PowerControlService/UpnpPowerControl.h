#pragma once

// Workaround for Visual Studio - the IDE always defines __cplusplus, so for IntelliSense
// to work on the MIDL generated files, let's define this:
#define CINTERFACE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ObjBase.h>
#include <upnphost.h>
#include <strsafe.h>
#include "EventMessages.h"

extern IMAGE_DOS_HEADER __ImageBase;

extern HANDLE g_hEventSource;

typedef struct _SERVICE_CONTEXT {
    HANDLE hExitEvent;
    SERVICE_STATUS_HANDLE StatusHandle;
    SERVICE_STATUS Status;
} SERVICE_CONTEXT, *PSERVICE_CONTEXT;

HRESULT
LastErrorToHRESULT();

HRESULT
UPnPPowerControlServiceCreateInstance(
    REFIID riid,
    void **ppvObject
    );

HRESULT
PowerControlDeviceCreateInstance(
    REFIID riid,
    void **ppvObject
    );

HRESULT
GetAdaptersInformation(
    BSTR *bstrOutput
    );

HRESULT AcquireShutdownPrivilege();

#define AllocateMemory(Size) HeapAlloc(GetProcessHeap(), 0, (Size))
#define FreeMemory(Memory) HeapFree(GetProcessHeap(), 0, (Memory))

#define WIDEN2(X) L##X
#define WIDEN(X) WIDEN2(#X)
#define STRINGIFY(X) WIDEN(X)

#define LOGHR(hr) \
    do \
    { \
        WCHAR HResult[16]; \
        if (g_hEventSource && SUCCEEDED(StringCchPrintf(HResult, ARRAYSIZE(HResult), L"0x%X", hr))) \
        { \
            LPWSTR StringInserts[] = { NULL, TEXT(__FILE__), STRINGIFY(__LINE__) }; \
            StringInserts[0] = HResult; \
            ReportEvent(g_hEventSource, EVENTLOG_ERROR_TYPE, 0, MSG_FAILED_HR, NULL, ARRAYSIZE(StringInserts), 0, StringInserts, NULL); \
        } \
    } while (UNCONDITIONAL_EXPR(0))

#define LOGLE(error) \
    do \
    { \
        WCHAR ErrorCodeStr[16]; \
        if (g_hEventSource && SUCCEEDED(StringCchPrintf(ErrorCodeStr, ARRAYSIZE(ErrorCodeStr), L"%u", error))) \
        { \
            LPWSTR StringInserts[] = { NULL, TEXT(__FILE__), STRINGIFY(__LINE__) }; \
            StringInserts[0] = ErrorCodeStr; \
            ReportEvent(g_hEventSource, EVENTLOG_ERROR_TYPE, 0, MSG_FAILED_LASTERROR, NULL, ARRAYSIZE(StringInserts), 0, StringInserts, NULL); \
        } \
    } while (UNCONDITIONAL_EXPR(0))

#ifndef NODEBUGPRINT
#include <stdio.h>
#define TraceInfo(Format,...) printf("%s(%d): " ## Format ## "\n", __FILE__, __LINE__, __VA_ARGS__)
#define TraceError(Format,...) printf("%s(%d): " ## Format ## "\n", __FILE__, __LINE__, __VA_ARGS__)
#else
#define TraceInfo(Format,...) (void)0
#define TraceError(Format,...) (void)0
#endif

// This gets rid of "C4127: conditional expression is constant" in cases when we really want it to be constant
#define UNCONDITIONAL_EXPR(Exp) (0,Exp)

#define IFC(stmt) \
    do { hr = (stmt); if (FAILED(hr)) { LOGHR(hr); TraceError("Failed with HRESULT %#x", hr); goto Cleanup; } } while (UNCONDITIONAL_EXPR(0))

#define IFC_WIN32(condition) \
    do \
    { \
        if (condition) \
        { \
            DWORD ErrorCode = GetLastError(); \
            LOGLE(ErrorCode); \
            hr = HRESULT_FROM_WIN32(ErrorCode); \
            TraceError("Failed with LastError %u", ErrorCode); \
            goto Cleanup; \
        } \
    } while (UNCONDITIONAL_EXPR(0))

#define IFRELEASE(intfc) \
    do { if (intfc) { intfc->lpVtbl->Release(intfc); } } while (UNCONDITIONAL_EXPR(0))

#define IFFREESTRING(bstr) \
    do { if (bstr) { SysFreeString(bstr); } } while (UNCONDITIONAL_EXPR(0))
