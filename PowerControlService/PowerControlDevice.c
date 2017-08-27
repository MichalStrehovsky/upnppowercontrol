#include "UpnpPowerControl.h"
#include "PowerControlDevice_h.h"
#include "PowerControlDevice_i.c"

typedef struct _PowerControlDevice {
    IUPnPDeviceControlVtbl *lpVtbl;
    ULONG RefCount;
    IDispatch *Service;
} PowerControlDevice;

HRESULT STDMETHODCALLTYPE
PowerControlDeviceQueryInterface( 
    IUPnPDeviceControl * This,
    REFIID riid,
    void **ppvObject
    )
{
    HRESULT hr = NOERROR;

    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IUPnPDeviceControl))
    {
        *ppvObject = This;
    }
    else
    {
        *ppvObject = NULL;
        hr = E_NOINTERFACE;
    }
    
    if (SUCCEEDED(hr))
    {
        This->lpVtbl->AddRef(This);
    }

    return hr;
}
        
ULONG STDMETHODCALLTYPE 
PowerControlDeviceAddRef(
    IUPnPDeviceControl * This
    )
{
    PowerControlDevice *RealThis = (PowerControlDevice*)This;

    RealThis->RefCount++;

    return RealThis->RefCount;
}
        
ULONG STDMETHODCALLTYPE
PowerControlDeviceRelease( 
    IUPnPDeviceControl * This
    )
{
    ULONG RefCount;

    PowerControlDevice *RealThis = (PowerControlDevice*)This;
    
    RealThis->RefCount--;
    RefCount = RealThis->RefCount;

    if (RefCount == 0)
    {
        TraceInfo("PowerControlDevice delete");

        if (RealThis->Service != NULL)
        {
            RealThis->Service->lpVtbl->Release(RealThis->Service);
        }

        FreeMemory(This);
        //DllRelease();
    }

    return RefCount;
}
        
HRESULT STDMETHODCALLTYPE 
PowerControlDeviceInitialize(
    IUPnPDeviceControl *This,
    BSTR bstrXMLDesc,
    BSTR bstrDeviceIdentifier,
    BSTR bstrInitString)
{
    UNREFERENCED_PARAMETER(This);
    UNREFERENCED_PARAMETER(bstrXMLDesc);
    UNREFERENCED_PARAMETER(bstrDeviceIdentifier);
    UNREFERENCED_PARAMETER(bstrInitString);

    return NOERROR;
}
        
HRESULT STDMETHODCALLTYPE
PowerControlDeviceGetServiceObject(
    IUPnPDeviceControl * This,
    BSTR bstrUDN,
    BSTR bstrServiceId,
    IDispatch **ppdispService
    )
{
    PowerControlDevice *realThis;

    UNREFERENCED_PARAMETER(bstrServiceId);
    UNREFERENCED_PARAMETER(bstrUDN);

    realThis = (PowerControlDevice*)This;

    *ppdispService = realThis->Service;
    realThis->Service->lpVtbl->AddRef(realThis->Service);

    return NOERROR;
}

IUPnPDeviceControlVtbl PowerControlDevice_Vtbl = {
    PowerControlDeviceQueryInterface,
    PowerControlDeviceAddRef,
    PowerControlDeviceRelease,
    PowerControlDeviceInitialize,
    PowerControlDeviceGetServiceObject
};

HRESULT
PowerControlDeviceCreateInstance(
    REFIID riid,
    void **ppvObject
    )
{
    HRESULT hr = S_OK;
    PowerControlDevice* thisObj = NULL;
    *ppvObject = NULL;

    TraceInfo("PowerControlDevice create instance");

    thisObj = (PowerControlDevice*)AllocateMemory(sizeof(PowerControlDevice));
    if (!thisObj)
    {
        hr = E_OUTOFMEMORY;
    }
    IFC(hr);

    RtlZeroMemory(thisObj, sizeof(PowerControlDevice));
    thisObj->lpVtbl = &PowerControlDevice_Vtbl;
    thisObj->RefCount = 1;
    
    IFC(UPnPPowerControlServiceCreateInstance(&IID_IDispatch, (void**)&thisObj->Service));

    IFC(PowerControlDeviceQueryInterface((IUPnPDeviceControl*)thisObj, riid, ppvObject));

    //DllAddRef();

Cleanup:
    if (thisObj != NULL)
    {
        PowerControlDeviceRelease((IUPnPDeviceControl*)thisObj);
    }

    return hr;
}
