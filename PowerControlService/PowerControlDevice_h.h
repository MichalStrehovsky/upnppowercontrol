

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Sun Dec 08 16:06:27 2013
 */
/* Compiler settings for PowerControlDevice.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __PowerControlDevice_h_h__
#define __PowerControlDevice_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUPnPPowerControlService_FWD_DEFINED__
#define __IUPnPPowerControlService_FWD_DEFINED__
typedef interface IUPnPPowerControlService IUPnPPowerControlService;

#endif 	/* __IUPnPPowerControlService_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PowerControlServiceLib_LIBRARY_DEFINED__
#define __PowerControlServiceLib_LIBRARY_DEFINED__

/* library PowerControlServiceLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_PowerControlServiceLib;

#ifndef __IUPnPPowerControlService_INTERFACE_DEFINED__
#define __IUPnPPowerControlService_INTERFACE_DEFINED__

/* interface IUPnPPowerControlService */
/* [unique][dual][object][uuid] */ 


EXTERN_C const IID IID_IUPnPPowerControlService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("42C090EC-2086-436A-AE61-6DB2FC8F20E8")
    IUPnPPowerControlService : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Hostname( 
            /* [retval][out] */ BSTR *pHostname) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AdapterAddresses( 
            /* [retval][out] */ BSTR *pAdapterAddresses) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Capabilities( 
            /* [retval][out] */ long *pCapabilities) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Suspend( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Hibernate( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUPnPPowerControlServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUPnPPowerControlService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUPnPPowerControlService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUPnPPowerControlService * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUPnPPowerControlService * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUPnPPowerControlService * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUPnPPowerControlService * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUPnPPowerControlService * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Hostname )( 
            IUPnPPowerControlService * This,
            /* [retval][out] */ BSTR *pHostname);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_AdapterAddresses )( 
            IUPnPPowerControlService * This,
            /* [retval][out] */ BSTR *pAdapterAddresses);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Capabilities )( 
            IUPnPPowerControlService * This,
            /* [retval][out] */ long *pCapabilities);
        
        HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IUPnPPowerControlService * This);
        
        HRESULT ( STDMETHODCALLTYPE *Hibernate )( 
            IUPnPPowerControlService * This);
        
        END_INTERFACE
    } IUPnPPowerControlServiceVtbl;

    interface IUPnPPowerControlService
    {
        CONST_VTBL struct IUPnPPowerControlServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUPnPPowerControlService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUPnPPowerControlService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUPnPPowerControlService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUPnPPowerControlService_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUPnPPowerControlService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUPnPPowerControlService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUPnPPowerControlService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUPnPPowerControlService_get_Hostname(This,pHostname)	\
    ( (This)->lpVtbl -> get_Hostname(This,pHostname) ) 

#define IUPnPPowerControlService_get_AdapterAddresses(This,pAdapterAddresses)	\
    ( (This)->lpVtbl -> get_AdapterAddresses(This,pAdapterAddresses) ) 

#define IUPnPPowerControlService_get_Capabilities(This,pCapabilities)	\
    ( (This)->lpVtbl -> get_Capabilities(This,pCapabilities) ) 

#define IUPnPPowerControlService_Suspend(This)	\
    ( (This)->lpVtbl -> Suspend(This) ) 

#define IUPnPPowerControlService_Hibernate(This)	\
    ( (This)->lpVtbl -> Hibernate(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUPnPPowerControlService_INTERFACE_DEFINED__ */

#endif /* __PowerControlServiceLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


