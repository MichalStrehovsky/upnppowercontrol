#include "UpnpPowerControl.h"
#include <stdio.h>
#include <conio.h>
#include <xmllite.h>
#include <strsafe.h>

static const PCWSTR ServiceName = L"UpnpPowerControl";
static const PCWSTR EventLogName = L"MichalStrehovsky-UPnPPowerControl";

HANDLE g_hEventSource = NULL;

HRESULT
LastErrorToHRESULT()
{
    HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}

HRESULT
AcquireShutdownPrivilege()
{
    HRESULT hr = NOERROR;
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    BOOL Result;

    Result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
    IFC_WIN32(!Result);

    Result = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid);
    IFC_WIN32(!Result);

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    Result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    IFC_WIN32(!Result);

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_ALL_ASSIGNED);
        goto Cleanup;
    }

Cleanup:
    if (hToken != NULL)
    {
        CloseHandle(hToken);
    }

    return hr;
}

HRESULT
GetDeviceDescriptionDocument(
    BSTR *bstrOutput
    )
{
    HRESULT hr = NOERROR;
    IXmlWriter *pXmlWriter = NULL;
    IStream *pOutputStream = NULL;
    IXmlWriterOutput *pXmlWriterOutput = NULL;
    HGLOBAL outputHGlobal = NULL;
    PWSTR pOutput = NULL;
    ULARGE_INTEGER outputLengthBytes;
    LARGE_INTEGER zero = {0};
    WCHAR pComputerName[MAX_COMPUTERNAME_LENGTH + 1];
    WCHAR pFriendlyName[MAX_COMPUTERNAME_LENGTH + 100 + 1];
    DWORD ComputerNameSize = ARRAYSIZE(pComputerName);
    extern const IID _IID_IXmlWriter;

    IFC_WIN32(!GetComputerName(pComputerName, &ComputerNameSize));

    IFC(StringCchPrintf(
        pFriendlyName,
        ARRAYSIZE(pFriendlyName),
        L"%s: Power State Manager",
        pComputerName
        ));
    
    //
    // Create output stream & the XML writer
    //

    IFC(CreateStreamOnHGlobal(NULL, TRUE, &pOutputStream));

    IFC(CreateXmlWriter(&_IID_IXmlWriter, &pXmlWriter, NULL));

    IFC(CreateXmlWriterOutputWithEncodingCodePage((IUnknown*)pOutputStream, NULL, 1200, &pXmlWriterOutput));

    IFC(pXmlWriter->lpVtbl->SetOutput(pXmlWriter, pXmlWriterOutput));

    IFC(pXmlWriter->lpVtbl->WriteProcessingInstruction(pXmlWriter, L"xml", L"version=\"1.0\""));

    IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"root", L"urn:schemas-upnp-org:device-1-0"));

    IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"specVersion", NULL));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"major", NULL, L"1"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"minor", NULL, L"0"));
    IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter)); // specVersion

    IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"device", NULL));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"deviceType", NULL, L"urn:migeel-sk:device:PowerStateDevice:1"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"friendlyName", NULL, pFriendlyName));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"manufacturer", NULL, L"Michal Strehovsky"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"manufacturerURL", NULL, L"http://migeel.sk/"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"modelDescription", NULL, L""));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"modelName", NULL, L"Power State Management Service"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"modelNumber", NULL, L"1-00-00"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"modelURL", NULL, L"http://migeel.sk/"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"serialNumber", NULL, L"0000001"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"UDN", NULL, L"uuid:RootDevice"));

    IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"serviceList", NULL));
    IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"service", NULL));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"serviceType", NULL, L"urn:migeel-sk:service:PowerStateService:1"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"serviceId", NULL, L"urn:migeel-sk:serviceId:PowerStateService1.0"));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"controlURL", NULL, L""));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"eventSubURL", NULL, L""));
    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"SCPDURL", NULL, L"PowerStateService.xml"));
    IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter)); // service
    IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter)); // serviceList

    IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"presentationURL", NULL, L""));

    IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter)); // device

    IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter)); // root

    IFC(pXmlWriter->lpVtbl->Flush(pXmlWriter));

    //
    // Extract the XML string from the stream
    //

    IFC(pOutputStream->lpVtbl->Seek(pOutputStream, zero, STREAM_SEEK_CUR, &outputLengthBytes));

    IFC(GetHGlobalFromStream(pOutputStream, &outputHGlobal));

    pOutput = (PWSTR)GlobalLock(outputHGlobal);
    if (!pOutput)
    {
        IFC(LastErrorToHRESULT());
    }

    //
    // Output starts with the Unicode BOM mark - skip it
    //

    pOutput++;
    outputLengthBytes.QuadPart -= 2;

    *bstrOutput = SysAllocStringByteLen((LPCSTR)pOutput, (UINT)outputLengthBytes.QuadPart);
    if (!(*bstrOutput))
    {
        hr = E_OUTOFMEMORY;
    }

Cleanup:
    if (pOutput)
    {
        GlobalUnlock(outputHGlobal);
    }

    IFRELEASE(pXmlWriter);

    IFRELEASE(pXmlWriterOutput);

    IFRELEASE(pOutputStream);

    return hr;
}

HRESULT
GetResourceDirectory(
    BSTR *pOutput
    )
{
    HRESULT hr = NOERROR;
    WCHAR ResourceDirectory[MAX_PATH + 1];
    DWORD ResourceDirectoryLength;
    DWORD Index;

    ResourceDirectoryLength = GetModuleFileName((HMODULE)&__ImageBase, ResourceDirectory, ARRAYSIZE(ResourceDirectory));
    if (ResourceDirectoryLength == 0)
    {
        hr = LastErrorToHRESULT();
    }
    else if (ResourceDirectoryLength > (ARRAYSIZE(ResourceDirectory) - 1))
    {
        hr = CO_E_PATHTOOLONG;
    }

    IFC(hr);

    for (Index = ResourceDirectoryLength - 1;
        (Index >= 0) && (ResourceDirectory[Index] != '\\');
        Index--)
    {
        ResourceDirectory[Index] = 0;
    }

    *pOutput = SysAllocString(ResourceDirectory);
    if (!(*pOutput))
    {
        hr = E_OUTOFMEMORY;
    }

Cleanup:
    return hr;
}

DWORD WINAPI
Run(
    LPVOID pParameter
    )
{
    HRESULT hr;
    IUnknown *pDevice = NULL;
    BSTR pResourceDirectory = NULL;
    BSTR pDeviceDescriptionDoc = NULL;
    BSTR pDeviceId = NULL;
    IUPnPRegistrar *pReg = NULL;
    DWORD WaitResult;
    PSERVICE_CONTEXT Context = (PSERVICE_CONTEXT)pParameter;

    struct {
        DWORD Length;
        WCHAR String;
    } EmptyBstr = { 0, 0 };

    IFC(GetDeviceDescriptionDocument(&pDeviceDescriptionDoc));

    IFC(GetResourceDirectory(&pResourceDirectory));

    IFC(PowerControlDeviceCreateInstance(&IID_IUnknown, (LPVOID*)&pDevice));

    IFC(CoCreateInstance(&CLSID_UPnPRegistrar, NULL, CLSCTX_LOCAL_SERVER, &IID_IUPnPRegistrar, (LPVOID*)&pReg));

    IFC(pReg->lpVtbl->RegisterRunningDevice(
        pReg,
        pDeviceDescriptionDoc,
        pDevice,
        &EmptyBstr.String,
        pResourceDirectory,
        3200,
        &pDeviceId));

    if (Context->StatusHandle != NULL)
    {
        Context->Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
        Context->Status.dwCurrentState = SERVICE_RUNNING;
        IFC_WIN32(!SetServiceStatus(Context->StatusHandle, &Context->Status));
    }

    WaitResult = WaitForSingleObject(Context->hExitEvent, INFINITE);
    if (WaitResult == WAIT_OBJECT_0)
    {
        // Expected
    }
    else if (WaitResult == WAIT_FAILED)
    {
        hr = LastErrorToHRESULT();
    }
    else
    {
        hr = E_FAIL;
    }
    IFC(hr);

Cleanup:
    if (pDeviceId)
    {
        HRESULT hr2 = pReg->lpVtbl->UnregisterDevice(pReg, pDeviceId, TRUE);
        if (FAILED(hr2))
        {
            TraceError("UnregisterDevice failed with HRESULT %#x", hr2);
        }
    }

    IFFREESTRING(pDeviceId);
    IFRELEASE(pReg);
    IFRELEASE(pDevice);
    IFFREESTRING(pResourceDirectory);
    IFFREESTRING(pDeviceDescriptionDoc);

    return hr;
}

HRESULT
ConsoleMain()
{
    HRESULT hr = NOERROR;
    HANDLE hThread = NULL;
    SERVICE_CONTEXT Context = { 0 };

    Context.hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    IFC_WIN32(Context.hExitEvent == NULL);

    printf("Service started in console mode\n");
    printf("Hit Enter to unregister the device\n");

    hThread = CreateThread(NULL, 0, Run, &Context, 0, NULL);
    IFC_WIN32(hThread == NULL);

    (CHAR)_getche();

    SetEvent(Context.hExitEvent);

    WaitForSingleObject(hThread, INFINITE);

Cleanup:
    if (Context.hExitEvent)
    {
        CloseHandle(Context.hExitEvent);
    }

    if (hThread)
    {
        CloseHandle(hThread);
    }

    return hr;
}

DWORD WINAPI
ServiceHandler(
  __in  DWORD dwControl,
  __in  DWORD dwEventType,
  __in  LPVOID lpEventData,
  __in  LPVOID lpContext
)
{
    PSERVICE_CONTEXT Context = (PSERVICE_CONTEXT)lpContext;
    DWORD Result = NO_ERROR;

    UNREFERENCED_PARAMETER(lpEventData);
    UNREFERENCED_PARAMETER(dwEventType);

    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:
        SetEvent(Context->hExitEvent);
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        Result = ERROR_CALL_NOT_IMPLEMENTED;
        break;
    }

    return Result;
}

VOID WINAPI
ServiceMain(
  DWORD dwArgc,
  LPTSTR *lpszArgv
)
{
    HRESULT hr = NOERROR;
    SERVICE_CONTEXT Context = { 0 };

    UNREFERENCED_PARAMETER(dwArgc);
    UNREFERENCED_PARAMETER(lpszArgv);

    Context.hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    IFC_WIN32(Context.hExitEvent == NULL);

    Context.StatusHandle = RegisterServiceCtrlHandlerEx(ServiceName, ServiceHandler, &Context);
    IFC_WIN32(Context.StatusHandle == NULL);

    Context.Status.dwCurrentState = SERVICE_START_PENDING;
    Context.Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    IFC_WIN32(!SetServiceStatus(Context.StatusHandle, &Context.Status));

    hr = Run(&Context);

Cleanup:
    if (Context.hExitEvent)
    {
        CloseHandle(Context.hExitEvent);
    }

    if (Context.StatusHandle)
    {
        Context.Status.dwCurrentState = SERVICE_STOPPED;
        if (FAILED(hr))
        {
            Context.Status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
            Context.Status.dwServiceSpecificExitCode = hr;
        }
        SetServiceStatus(Context.StatusHandle, &Context.Status);
    }
}

int wmain(int argc, wchar_t* argv[])
{
    HRESULT hr;
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        { L"", ServiceMain },
        { 0, 0 }
    };

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    g_hEventSource = RegisterEventSource(NULL, EventLogName);

    IFC(CoInitializeEx(NULL, COINIT_MULTITHREADED));

    IFC(AcquireShutdownPrivilege());

    IFC(CoInitializeSecurity(
            NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_SECURE_REFS,
            NULL));

    // Try to start the service dispatched
    if (!StartServiceCtrlDispatcher(ServiceTable))
    {
        if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
        {
            // This is a command line invocation
            hr = ConsoleMain();
        }
        else
        {
            IFC_WIN32(UNCONDITIONAL_EXPR(1));
        }
    }

    if (g_hEventSource)
    {
        DeregisterEventSource(g_hEventSource);
    }

Cleanup:

    CoUninitialize();
}
