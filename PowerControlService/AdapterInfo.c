#include "UpnpPowerControl.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <xmllite.h>
#include <strsafe.h>

HRESULT
GetAdaptersInformation(
    BSTR *bstrOutput
    )
{
    HRESULT hr = NOERROR;
    ULONG BufferSize = 0;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    PIP_ADAPTER_ADDRESSES pCurrentAddress;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast;
    ULONG ErrorCode;
    IXmlWriter *pXmlWriter = NULL;
    IStream *pOutputStream = NULL;
    IXmlWriterOutput *pXmlWriterOutput = NULL;
    HGLOBAL outputHGlobal = NULL;
    PWSTR pOutput = NULL;
    ULARGE_INTEGER outputLengthBytes;
    LARGE_INTEGER zero = {0};
    WCHAR Buffer[255];
    PSOCKADDR_IN pSockAddrIn;

    //
    // Start with getting the list of IP_ADAPTER_ADDRESSES structures. 
    //

    do
    {
        ErrorCode = GetAdaptersAddresses(
            AF_INET,
            GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
            NULL,
            pAddresses,
            &BufferSize
            );

        if (ErrorCode == ERROR_BUFFER_OVERFLOW)
        {
            if (pAddresses)
            {
                FreeMemory(pAddresses);
            }

            pAddresses = (PIP_ADAPTER_ADDRESSES)AllocateMemory(BufferSize);
            if (pAddresses == NULL)
            {
                ErrorCode = ERROR_OUTOFMEMORY;
            }
        }
    } while (ErrorCode == ERROR_BUFFER_OVERFLOW);

    IFC(HRESULT_FROM_WIN32(ErrorCode));

    //
    // Create output stream & the XML writer
    //

    IFC(CreateStreamOnHGlobal(NULL, TRUE, &pOutputStream));

    IFC(CreateXmlWriter(&_IID_IXmlWriter, &pXmlWriter, NULL));

    IFC(CreateXmlWriterOutputWithEncodingCodePage((IUnknown*)pOutputStream, NULL, 1200, &pXmlWriterOutput));

    IFC(pXmlWriter->lpVtbl->SetOutput(pXmlWriter, pXmlWriterOutput));

    IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"Adapters", NULL));

    //
    // Enumerate addresses
    //

    for (pCurrentAddress = pAddresses; pCurrentAddress; pCurrentAddress = pCurrentAddress->Next)
    {
        if (pCurrentAddress->PhysicalAddressLength != 6)
        {
            continue;
        }

        //
        // Format & write the MAC address
        //

        IFC(StringCchPrintf(
            Buffer,
            ARRAYSIZE(Buffer),
            L"%02X-%02X-%02X-%02X-%02X-%02X",
            pCurrentAddress->PhysicalAddress[0],
            pCurrentAddress->PhysicalAddress[1],
            pCurrentAddress->PhysicalAddress[2],
            pCurrentAddress->PhysicalAddress[3],
            pCurrentAddress->PhysicalAddress[4],
            pCurrentAddress->PhysicalAddress[5]));

        IFC(pXmlWriter->lpVtbl->WriteStartElement(pXmlWriter, NULL, L"Adapter", NULL));

        IFC(pXmlWriter->lpVtbl->WriteAttributeString(pXmlWriter, NULL, L"PhysicalAddress", NULL, Buffer));

        //
        // Enumarate IP addresses
        //

        for (pUnicast = pCurrentAddress->FirstUnicastAddress; pUnicast; pUnicast = pUnicast->Next)
        {
            switch (pUnicast->Address.lpSockaddr->sa_family)
            {
            case AF_INET:
                pSockAddrIn = (PSOCKADDR_IN)pUnicast->Address.lpSockaddr;
                
                IFC(StringCchPrintf(
                    Buffer,
                    ARRAYSIZE(Buffer),
                    L"%u.%u.%u.%u",
                    (UINT)pSockAddrIn->sin_addr.S_un.S_un_b.s_b1,
                    (UINT)pSockAddrIn->sin_addr.S_un.S_un_b.s_b2,
                    (UINT)pSockAddrIn->sin_addr.S_un.S_un_b.s_b3,
                    (UINT)pSockAddrIn->sin_addr.S_un.S_un_b.s_b4));

                IFC(pXmlWriter->lpVtbl->WriteElementString(pXmlWriter, NULL, L"IpAddress", NULL, Buffer));
                break;
            }
        }

        IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter));
    }


    IFC(pXmlWriter->lpVtbl->WriteEndElement(pXmlWriter));

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

    if (pAddresses)
    {
        FreeMemory(pAddresses);
    }

    return hr;
}