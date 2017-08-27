#include "UpnpPowerControl.h"
#include "PowerControlDevice_h.h"
#include <powrprof.h>

#define PCS_CAN_SUSPEND     0x01
#define PCS_CAN_HIBERNATE   0x02

typedef struct _UPnPPowerControlService {
    IUPnPPowerControlServiceVtbl *lpVtbl;
    IUPnPEventSourceVtbl *lpVtblEventSource;
    ULONG RefCount;
    ITypeInfo *TypeInfo;
} UPnPPowerControlService;

HRESULT STDMETHODCALLTYPE
UpnpEventSourceQueryInterface( 
    IUPnPEventSource * This,
    REFIID riid,
    void **ppvObject
    )
{
    UPnPPowerControlService *base;
    base = (UPnPPowerControlService*)((PBYTE)This - FIELD_OFFSET(UPnPPowerControlService, lpVtblEventSource));
    return base->lpVtbl->QueryInterface((IUPnPPowerControlService*)base, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE 
UpnpEventSourceAddRef(
    IUPnPEventSource * This
    )
{
    UPnPPowerControlService *base;
    base = (UPnPPowerControlService*)((PBYTE)This - FIELD_OFFSET(UPnPPowerControlService, lpVtblEventSource));
    return base->lpVtbl->AddRef((IUPnPPowerControlService*)base);
}
        
ULONG STDMETHODCALLTYPE
UpnpEventSourceRelease( 
    IUPnPEventSource * This
    )
{
    UPnPPowerControlService *base;
    base = (UPnPPowerControlService*)((PBYTE)This - FIELD_OFFSET(UPnPPowerControlService, lpVtblEventSource));
    return base->lpVtbl->Release((IUPnPPowerControlService*)base);
}

HRESULT STDMETHODCALLTYPE
UpnpEventSourceAdvise(
    IUPnPEventSource* This,
    IUPnPEventSink* pSink
    )
{
    UNREFERENCED_PARAMETER(This);
    UNREFERENCED_PARAMETER(pSink);

    // We don't support eventing
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
UpnpEventSourceUndvise(
    IUPnPEventSource* This,
    IUPnPEventSink* pSink
    )
{
    UNREFERENCED_PARAMETER(This);
    UNREFERENCED_PARAMETER(pSink);

    // We don't support eventing
    return S_OK;
}

IUPnPEventSourceVtbl UPnPEventSource_Vtbl = {
    UpnpEventSourceQueryInterface,
    UpnpEventSourceAddRef,
    UpnpEventSourceRelease,
    UpnpEventSourceAdvise,
    UpnpEventSourceUndvise
};

HRESULT STDMETHODCALLTYPE
UPnPPowerControlServiceQueryInterface( 
    IUPnPPowerControlService * This,
    REFIID riid,
    void **ppvObject
    )
{
    HRESULT hr = NOERROR;

    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IUPnPPowerControlService) ||
        IsEqualIID(riid, &IID_IDispatch))
    {
        *ppvObject = This;
    } 
    else if (IsEqualIID(riid, &IID_IUPnPEventSource))
    {
        *ppvObject = (PBYTE)This + FIELD_OFFSET(UPnPPowerControlService, lpVtblEventSource);
    } 
    else
    {
        hr = E_NOINTERFACE;
        *ppvObject = NULL;
    }
    
    if (SUCCEEDED(hr))
    {
        This->lpVtbl->AddRef(This);
    }

    return hr;
}
        
ULONG STDMETHODCALLTYPE 
UPnPPowerControlServiceAddRef(
    IUPnPPowerControlService * This
    )
{
    UPnPPowerControlService *RealThis = (UPnPPowerControlService*)This;

    RealThis->RefCount++;

    return RealThis->RefCount;
}
        
ULONG STDMETHODCALLTYPE
UPnPPowerControlServiceRelease( 
    IUPnPPowerControlService * This
    )
{
    ULONG RefCount;

    UPnPPowerControlService *RealThis = (UPnPPowerControlService*)This;

    RealThis->RefCount--;
    RefCount = RealThis->RefCount;

    if (RefCount == 0)
    {
        TraceInfo("UPnPPowerControlService delete");

        if (RealThis->TypeInfo)
        {
            RealThis->TypeInfo->lpVtbl->Release(RealThis->TypeInfo);
        }

        FreeMemory(This);
        //DllRelease();
    }

    return RefCount;
}

HRESULT STDMETHODCALLTYPE 
UPnPPowerControlServiceGetTypeInfoCount( 
    IUPnPPowerControlService * This,
    UINT *pctinfo
    )
{
    UNREFERENCED_PARAMETER(This);

    *pctinfo = 1;
    return NOERROR;
}
        
HRESULT STDMETHODCALLTYPE 
UPnPPowerControlServiceGetTypeInfo( 
    IUPnPPowerControlService * This,
    UINT iTInfo,
    LCID lcid,
    ITypeInfo **ppTInfo
    )
{
    UPnPPowerControlService *RealThis = (UPnPPowerControlService*)This;

    UNREFERENCED_PARAMETER(lcid);
    UNREFERENCED_PARAMETER(iTInfo);

    *ppTInfo = RealThis->TypeInfo;
    RealThis->TypeInfo->lpVtbl->AddRef(RealThis->TypeInfo);

    return NOERROR;
}
        
HRESULT STDMETHODCALLTYPE
UPnPPowerControlServiceGetIDsOfNames(
    IUPnPPowerControlService * This,
    REFIID riid,
    LPOLESTR *rgszNames,
    UINT cNames,
    LCID lcid,
    DISPID *rgDispId
    )
{
    HRESULT hr = DispGetIDsOfNames(((UPnPPowerControlService*)This)->TypeInfo, rgszNames, cNames, rgDispId);
    
    UNREFERENCED_PARAMETER(lcid);
    UNREFERENCED_PARAMETER(riid);

    return hr;
}
        
HRESULT STDMETHODCALLTYPE 
UPnPPowerControlServiceInvoke(
    IUPnPPowerControlService * This,
    DISPID dispIdMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS *pDispParams,
    VARIANT *pVarResult,
    EXCEPINFO *pExcepInfo,
    UINT *puArgErr)
{
    HRESULT hr;

    UNREFERENCED_PARAMETER(riid);
    UNREFERENCED_PARAMETER(lcid);

    hr = DispInvoke(This, ((UPnPPowerControlService*)This)->TypeInfo, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
    return hr;
}
        
HRESULT STDMETHODCALLTYPE
UPnPPowerControlServiceget_Hostname( 
    IUPnPPowerControlService * This,
    BSTR *pHostname)
{
    WCHAR pComputerName[MAX_COMPUTERNAME_LENGTH + 1];
    HRESULT hr = NOERROR;
    DWORD Size = ARRAYSIZE(pComputerName);

    UNREFERENCED_PARAMETER(This);

    TraceInfo("Get Hostname");

    if (!GetComputerName(pComputerName, &Size))
    {
        hr = LastErrorToHRESULT();
    }

    if (SUCCEEDED(hr))
    {
        *pHostname = SysAllocString(pComputerName);
        if (!*pHostname)
        {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE
UPnPPowerControlServiceget_AdapterAddresses( 
    IUPnPPowerControlService * This,
    BSTR *pAddresses)
{
    HRESULT hr;

    UNREFERENCED_PARAMETER(This);

    TraceInfo("Get AdapterAddresses");

    hr = GetAdaptersInformation(pAddresses);

    return hr;
}

DWORD WINAPI SuspendOrHibernate(
    LPVOID pParameter
    )
{
    BOOLEAN Result;

    //
    // Sleep for a while to give the UPnP stack time to send a reponse.
    //

    Sleep(500);

    Result = SetSuspendState((BOOLEAN)(ULONG_PTR)pParameter, TRUE, FALSE);
    if (!Result)
    {
        TraceError("SetSuspendState failed with %u", GetLastError());
    }

    return 0;
}

HRESULT ScheduleSuspendOrHibernate(BOOLEAN Hibernate)
{
    HRESULT hr = NOERROR;

    HANDLE hThread = CreateThread(NULL, 0, &SuspendOrHibernate, (LPVOID)(ULONG_PTR)Hibernate, 0, NULL);
    if (hThread == NULL)
    {
        hr = LastErrorToHRESULT();
    }
    else
    {
        CloseHandle(hThread);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE 
UPnPPowerControlServiceSuspend(
    IUPnPPowerControlService * This
    )
{
    HRESULT hr;

    UNREFERENCED_PARAMETER(This);

    TraceInfo("UPnP Suspend");

    //
    // We don't suspend immediately because the client wouldn't get a reponse back
    //

    hr = ScheduleSuspendOrHibernate(FALSE);

    return hr;
}

HRESULT STDMETHODCALLTYPE 
UPnPPowerControlServiceHibernate(
    IUPnPPowerControlService * This
    )
{
    HRESULT hr;

    UNREFERENCED_PARAMETER(This);

    TraceInfo("UPnP Hibernate");

    //
    // We don't hibernate immediately because the client wouldn't get a reponse back
    //

    hr = ScheduleSuspendOrHibernate(TRUE);

    return hr;
}

HRESULT STDMETHODCALLTYPE
UPnPPowerControlServiceget_Capabilities(
    IUPnPPowerControlService * This,
    LONG *pCapabilities
    )
{
    SYSTEM_POWER_CAPABILITIES PowerCapabilities;
    LONG Capabilities = 0;

    UNREFERENCED_PARAMETER(This);

    TraceInfo("UPnP Get Capabilities");

    if (GetPwrCapabilities(&PowerCapabilities))
    {
        if (PowerCapabilities.SystemS1 || 
            PowerCapabilities.SystemS2 ||
            PowerCapabilities.SystemS3)
        {
            Capabilities |= PCS_CAN_SUSPEND;
        }

        if (PowerCapabilities.SystemS4 &&
            PowerCapabilities.HiberFilePresent)
        {
            Capabilities |= PCS_CAN_HIBERNATE;
        }
    }

    *pCapabilities = Capabilities;
    return NOERROR;
}

IUPnPPowerControlServiceVtbl UPnPPowerControlService_Vtbl = {
    UPnPPowerControlServiceQueryInterface,
    UPnPPowerControlServiceAddRef,
    UPnPPowerControlServiceRelease,
    UPnPPowerControlServiceGetTypeInfoCount,
    UPnPPowerControlServiceGetTypeInfo,
    UPnPPowerControlServiceGetIDsOfNames,
    UPnPPowerControlServiceInvoke,
    UPnPPowerControlServiceget_Hostname,
    UPnPPowerControlServiceget_AdapterAddresses,
    UPnPPowerControlServiceget_Capabilities,
    UPnPPowerControlServiceSuspend,
    UPnPPowerControlServiceHibernate
};

HRESULT
LoadTypeInfo(
    ITypeInfo **ppvObject
    )
{
    HRESULT hr = NOERROR;
    LPTYPELIB pTypeLib = NULL;
    WCHAR ModuleFileName[MAX_PATH + 1];
    DWORD ModuleFileNameLength;

    *ppvObject = NULL;

    ModuleFileNameLength = GetModuleFileName(
        (HMODULE)&__ImageBase,
        ModuleFileName,
        ARRAYSIZE(ModuleFileName));
    if (ModuleFileNameLength == 0)
    {
        hr = LastErrorToHRESULT();
    }
    else if (ModuleFileNameLength > (ARRAYSIZE(ModuleFileName) - 1))
    {
        hr = CO_E_PATHTOOLONG;
    }

    IFC(hr);

    IFC(LoadTypeLibEx(ModuleFileName, REGKIND_NONE, &pTypeLib));

    IFC(pTypeLib->lpVtbl->GetTypeInfoOfGuid(pTypeLib, &IID_IUPnPPowerControlService, ppvObject));

Cleanup:
    if (pTypeLib)
    {
        pTypeLib->lpVtbl->Release(pTypeLib);
    }

    return hr;
}

HRESULT
UPnPPowerControlServiceCreateInstance(
    REFIID riid,
    void **ppvObject
    )
{
    HRESULT hr = S_OK;
    UPnPPowerControlService *thisObj = NULL;
    *ppvObject = NULL;

    TraceInfo("UPnPPowerControlService create instance");

    thisObj = (UPnPPowerControlService*)AllocateMemory(sizeof(UPnPPowerControlService));
    if (!thisObj)
    {
        hr = E_OUTOFMEMORY;
    }
    IFC(hr);

    RtlZeroMemory(thisObj, sizeof(UPnPPowerControlService));
    thisObj->lpVtbl = &UPnPPowerControlService_Vtbl;
    thisObj->lpVtblEventSource = &UPnPEventSource_Vtbl;
    thisObj->RefCount = 1;
    
    IFC(LoadTypeInfo(&thisObj->TypeInfo));

    IFC(UPnPPowerControlServiceQueryInterface((IUPnPPowerControlService*)thisObj, riid, ppvObject));

    //DllAddRef();

Cleanup:
    if (thisObj)
    {
        UPnPPowerControlServiceRelease((IUPnPPowerControlService*)thisObj);
    }

    return hr;
}
