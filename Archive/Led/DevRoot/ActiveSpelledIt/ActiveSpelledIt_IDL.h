

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Wed Apr 09 23:22:00 2008
 */
/* Compiler settings for .\Sources\ActiveSpelledIt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ActiveSpelledIt_IDL_h__
#define __ActiveSpelledIt_IDL_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IActiveSpelledItCtl_FWD_DEFINED__
#define __IActiveSpelledItCtl_FWD_DEFINED__
typedef interface IActiveSpelledItCtl IActiveSpelledItCtl;
#endif 	/* __IActiveSpelledItCtl_FWD_DEFINED__ */


#ifndef __IActiveSpelledIt_WordInfo_FWD_DEFINED__
#define __IActiveSpelledIt_WordInfo_FWD_DEFINED__
typedef interface IActiveSpelledIt_WordInfo IActiveSpelledIt_WordInfo;
#endif 	/* __IActiveSpelledIt_WordInfo_FWD_DEFINED__ */


#ifndef __IActiveSpelledIt_ScanContext_FWD_DEFINED__
#define __IActiveSpelledIt_ScanContext_FWD_DEFINED__
typedef interface IActiveSpelledIt_ScanContext IActiveSpelledIt_ScanContext;
#endif 	/* __IActiveSpelledIt_ScanContext_FWD_DEFINED__ */


#ifndef __ActiveSpelledItCtl_FWD_DEFINED__
#define __ActiveSpelledItCtl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ActiveSpelledItCtl ActiveSpelledItCtl;
#else
typedef struct ActiveSpelledItCtl ActiveSpelledItCtl;
#endif /* __cplusplus */

#endif 	/* __ActiveSpelledItCtl_FWD_DEFINED__ */


#ifndef __BOGUS_TO_GET_INTERFACES_EXPORTED_FWD_DEFINED__
#define __BOGUS_TO_GET_INTERFACES_EXPORTED_FWD_DEFINED__

#ifdef __cplusplus
typedef class BOGUS_TO_GET_INTERFACES_EXPORTED BOGUS_TO_GET_INTERFACES_EXPORTED;
#else
typedef struct BOGUS_TO_GET_INTERFACES_EXPORTED BOGUS_TO_GET_INTERFACES_EXPORTED;
#endif /* __cplusplus */

#endif 	/* __BOGUS_TO_GET_INTERFACES_EXPORTED_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IActiveSpelledItCtl_INTERFACE_DEFINED__
#define __IActiveSpelledItCtl_INTERFACE_DEFINED__

/* interface IActiveSpelledItCtl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IActiveSpelledItCtl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D3241A98-5959-4414-998F-EF1CFBEDF1A5")
    IActiveSpelledItCtl : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GenerateSuggestions( 
            /* [in] */ BSTR missingWord,
            /* [retval][out] */ VARIANT *results) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateScanContext( 
            BSTR text,
            /* [defaultvalue][in] */ UINT cursor,
            /* [retval][out] */ IDispatch **scanContext) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VersionNumber( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShortVersionString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LookupWord( 
            BSTR word,
            /* [defaultvalue][out] */ BSTR *matchedWord,
            /* [retval][out] */ VARIANT_BOOL *found) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindWordBreaks( 
            /* [in] */ BSTR srcText,
            /* [defaultvalue][in] */ UINT textOffsetToStartLookingForWord,
            /* [out][retval] */ IDispatch **wordInfo) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AvailableSystemDictionaries( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SystemDictionaryList( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SystemDictionaryList( 
            /* [in] */ VARIANT val) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DictionaryList( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_UserDictionary( 
            /* [retval][out] */ BSTR *pUDName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_UserDictionary( 
            /* [in] */ BSTR UDName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultUserDictionaryName( 
            /* [retval][out] */ BSTR *pUDName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddWordToUserDictionary( 
            /* [in] */ BSTR word) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AddWordToUserDictionarySupported( 
            /* [retval][out] */ VARIANT_BOOL *supported) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveSpelledItCtlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveSpelledItCtl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveSpelledItCtl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveSpelledItCtl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActiveSpelledItCtl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActiveSpelledItCtl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActiveSpelledItCtl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActiveSpelledItCtl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GenerateSuggestions )( 
            IActiveSpelledItCtl * This,
            /* [in] */ BSTR missingWord,
            /* [retval][out] */ VARIANT *results);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateScanContext )( 
            IActiveSpelledItCtl * This,
            BSTR text,
            /* [defaultvalue][in] */ UINT cursor,
            /* [retval][out] */ IDispatch **scanContext);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VersionNumber )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShortVersionString )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LookupWord )( 
            IActiveSpelledItCtl * This,
            BSTR word,
            /* [defaultvalue][out] */ BSTR *matchedWord,
            /* [retval][out] */ VARIANT_BOOL *found);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindWordBreaks )( 
            IActiveSpelledItCtl * This,
            /* [in] */ BSTR srcText,
            /* [defaultvalue][in] */ UINT textOffsetToStartLookingForWord,
            /* [out][retval] */ IDispatch **wordInfo);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AvailableSystemDictionaries )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SystemDictionaryList )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SystemDictionaryList )( 
            IActiveSpelledItCtl * This,
            /* [in] */ VARIANT val);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DictionaryList )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserDictionary )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ BSTR *pUDName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserDictionary )( 
            IActiveSpelledItCtl * This,
            /* [in] */ BSTR UDName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultUserDictionaryName )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ BSTR *pUDName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddWordToUserDictionary )( 
            IActiveSpelledItCtl * This,
            /* [in] */ BSTR word);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddWordToUserDictionarySupported )( 
            IActiveSpelledItCtl * This,
            /* [retval][out] */ VARIANT_BOOL *supported);
        
        END_INTERFACE
    } IActiveSpelledItCtlVtbl;

    interface IActiveSpelledItCtl
    {
        CONST_VTBL struct IActiveSpelledItCtlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveSpelledItCtl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveSpelledItCtl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveSpelledItCtl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveSpelledItCtl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveSpelledItCtl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveSpelledItCtl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveSpelledItCtl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveSpelledItCtl_GenerateSuggestions(This,missingWord,results)	\
    (This)->lpVtbl -> GenerateSuggestions(This,missingWord,results)

#define IActiveSpelledItCtl_CreateScanContext(This,text,cursor,scanContext)	\
    (This)->lpVtbl -> CreateScanContext(This,text,cursor,scanContext)

#define IActiveSpelledItCtl_get_VersionNumber(This,pVal)	\
    (This)->lpVtbl -> get_VersionNumber(This,pVal)

#define IActiveSpelledItCtl_get_ShortVersionString(This,pVal)	\
    (This)->lpVtbl -> get_ShortVersionString(This,pVal)

#define IActiveSpelledItCtl_LookupWord(This,word,matchedWord,found)	\
    (This)->lpVtbl -> LookupWord(This,word,matchedWord,found)

#define IActiveSpelledItCtl_FindWordBreaks(This,srcText,textOffsetToStartLookingForWord,wordInfo)	\
    (This)->lpVtbl -> FindWordBreaks(This,srcText,textOffsetToStartLookingForWord,wordInfo)

#define IActiveSpelledItCtl_get_AvailableSystemDictionaries(This,pVal)	\
    (This)->lpVtbl -> get_AvailableSystemDictionaries(This,pVal)

#define IActiveSpelledItCtl_get_SystemDictionaryList(This,pVal)	\
    (This)->lpVtbl -> get_SystemDictionaryList(This,pVal)

#define IActiveSpelledItCtl_put_SystemDictionaryList(This,val)	\
    (This)->lpVtbl -> put_SystemDictionaryList(This,val)

#define IActiveSpelledItCtl_get_DictionaryList(This,pVal)	\
    (This)->lpVtbl -> get_DictionaryList(This,pVal)

#define IActiveSpelledItCtl_get_UserDictionary(This,pUDName)	\
    (This)->lpVtbl -> get_UserDictionary(This,pUDName)

#define IActiveSpelledItCtl_put_UserDictionary(This,UDName)	\
    (This)->lpVtbl -> put_UserDictionary(This,UDName)

#define IActiveSpelledItCtl_get_DefaultUserDictionaryName(This,pUDName)	\
    (This)->lpVtbl -> get_DefaultUserDictionaryName(This,pUDName)

#define IActiveSpelledItCtl_AddWordToUserDictionary(This,word)	\
    (This)->lpVtbl -> AddWordToUserDictionary(This,word)

#define IActiveSpelledItCtl_get_AddWordToUserDictionarySupported(This,supported)	\
    (This)->lpVtbl -> get_AddWordToUserDictionarySupported(This,supported)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_GenerateSuggestions_Proxy( 
    IActiveSpelledItCtl * This,
    /* [in] */ BSTR missingWord,
    /* [retval][out] */ VARIANT *results);


void __RPC_STUB IActiveSpelledItCtl_GenerateSuggestions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_CreateScanContext_Proxy( 
    IActiveSpelledItCtl * This,
    BSTR text,
    /* [defaultvalue][in] */ UINT cursor,
    /* [retval][out] */ IDispatch **scanContext);


void __RPC_STUB IActiveSpelledItCtl_CreateScanContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_VersionNumber_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IActiveSpelledItCtl_get_VersionNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_ShortVersionString_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IActiveSpelledItCtl_get_ShortVersionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_LookupWord_Proxy( 
    IActiveSpelledItCtl * This,
    BSTR word,
    /* [defaultvalue][out] */ BSTR *matchedWord,
    /* [retval][out] */ VARIANT_BOOL *found);


void __RPC_STUB IActiveSpelledItCtl_LookupWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_FindWordBreaks_Proxy( 
    IActiveSpelledItCtl * This,
    /* [in] */ BSTR srcText,
    /* [defaultvalue][in] */ UINT textOffsetToStartLookingForWord,
    /* [out][retval] */ IDispatch **wordInfo);


void __RPC_STUB IActiveSpelledItCtl_FindWordBreaks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_AvailableSystemDictionaries_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IActiveSpelledItCtl_get_AvailableSystemDictionaries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_SystemDictionaryList_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IActiveSpelledItCtl_get_SystemDictionaryList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_put_SystemDictionaryList_Proxy( 
    IActiveSpelledItCtl * This,
    /* [in] */ VARIANT val);


void __RPC_STUB IActiveSpelledItCtl_put_SystemDictionaryList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_DictionaryList_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IActiveSpelledItCtl_get_DictionaryList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_UserDictionary_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ BSTR *pUDName);


void __RPC_STUB IActiveSpelledItCtl_get_UserDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_put_UserDictionary_Proxy( 
    IActiveSpelledItCtl * This,
    /* [in] */ BSTR UDName);


void __RPC_STUB IActiveSpelledItCtl_put_UserDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_DefaultUserDictionaryName_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ BSTR *pUDName);


void __RPC_STUB IActiveSpelledItCtl_get_DefaultUserDictionaryName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_AddWordToUserDictionary_Proxy( 
    IActiveSpelledItCtl * This,
    /* [in] */ BSTR word);


void __RPC_STUB IActiveSpelledItCtl_AddWordToUserDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledItCtl_get_AddWordToUserDictionarySupported_Proxy( 
    IActiveSpelledItCtl * This,
    /* [retval][out] */ VARIANT_BOOL *supported);


void __RPC_STUB IActiveSpelledItCtl_get_AddWordToUserDictionarySupported_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveSpelledItCtl_INTERFACE_DEFINED__ */


#ifndef __IActiveSpelledIt_WordInfo_INTERFACE_DEFINED__
#define __IActiveSpelledIt_WordInfo_INTERFACE_DEFINED__

/* interface IActiveSpelledIt_WordInfo */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IActiveSpelledIt_WordInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B86F5832-5874-4952-B88C-A27CC820B976")
    IActiveSpelledIt_WordInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WordStart( 
            /* [retval][out] */ UINT *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WordEnd( 
            /* [retval][out] */ UINT *pVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WordReal( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveSpelledIt_WordInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveSpelledIt_WordInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveSpelledIt_WordInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveSpelledIt_WordInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActiveSpelledIt_WordInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActiveSpelledIt_WordInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActiveSpelledIt_WordInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActiveSpelledIt_WordInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordStart )( 
            IActiveSpelledIt_WordInfo * This,
            /* [retval][out] */ UINT *pVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordEnd )( 
            IActiveSpelledIt_WordInfo * This,
            /* [retval][out] */ UINT *pVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordReal )( 
            IActiveSpelledIt_WordInfo * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        END_INTERFACE
    } IActiveSpelledIt_WordInfoVtbl;

    interface IActiveSpelledIt_WordInfo
    {
        CONST_VTBL struct IActiveSpelledIt_WordInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveSpelledIt_WordInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveSpelledIt_WordInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveSpelledIt_WordInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveSpelledIt_WordInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveSpelledIt_WordInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveSpelledIt_WordInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveSpelledIt_WordInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveSpelledIt_WordInfo_get_WordStart(This,pVal)	\
    (This)->lpVtbl -> get_WordStart(This,pVal)

#define IActiveSpelledIt_WordInfo_get_WordEnd(This,pVal)	\
    (This)->lpVtbl -> get_WordEnd(This,pVal)

#define IActiveSpelledIt_WordInfo_get_WordReal(This,pVal)	\
    (This)->lpVtbl -> get_WordReal(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_WordInfo_get_WordStart_Proxy( 
    IActiveSpelledIt_WordInfo * This,
    /* [retval][out] */ UINT *pVal);


void __RPC_STUB IActiveSpelledIt_WordInfo_get_WordStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_WordInfo_get_WordEnd_Proxy( 
    IActiveSpelledIt_WordInfo * This,
    /* [retval][out] */ UINT *pVal);


void __RPC_STUB IActiveSpelledIt_WordInfo_get_WordEnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_WordInfo_get_WordReal_Proxy( 
    IActiveSpelledIt_WordInfo * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IActiveSpelledIt_WordInfo_get_WordReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveSpelledIt_WordInfo_INTERFACE_DEFINED__ */


#ifndef __IActiveSpelledIt_ScanContext_INTERFACE_DEFINED__
#define __IActiveSpelledIt_ScanContext_INTERFACE_DEFINED__

/* interface IActiveSpelledIt_ScanContext */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IActiveSpelledIt_ScanContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8CBD536B-9816-4A2E-98BD-5761DAE6D379")
    IActiveSpelledIt_ScanContext : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_WordFound( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_WordStart( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_WordEnd( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Word( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Next( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Suggestions( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Cursor( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveSpelledIt_ScanContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveSpelledIt_ScanContext * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveSpelledIt_ScanContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveSpelledIt_ScanContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActiveSpelledIt_ScanContext * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActiveSpelledIt_ScanContext * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActiveSpelledIt_ScanContext * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActiveSpelledIt_ScanContext * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordFound )( 
            IActiveSpelledIt_ScanContext * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordStart )( 
            IActiveSpelledIt_ScanContext * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordEnd )( 
            IActiveSpelledIt_ScanContext * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Word )( 
            IActiveSpelledIt_ScanContext * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IActiveSpelledIt_ScanContext * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Suggestions )( 
            IActiveSpelledIt_ScanContext * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Cursor )( 
            IActiveSpelledIt_ScanContext * This,
            /* [retval][out] */ ULONG *pVal);
        
        END_INTERFACE
    } IActiveSpelledIt_ScanContextVtbl;

    interface IActiveSpelledIt_ScanContext
    {
        CONST_VTBL struct IActiveSpelledIt_ScanContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveSpelledIt_ScanContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveSpelledIt_ScanContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveSpelledIt_ScanContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveSpelledIt_ScanContext_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveSpelledIt_ScanContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveSpelledIt_ScanContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveSpelledIt_ScanContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveSpelledIt_ScanContext_get_WordFound(This,pVal)	\
    (This)->lpVtbl -> get_WordFound(This,pVal)

#define IActiveSpelledIt_ScanContext_get_WordStart(This,pVal)	\
    (This)->lpVtbl -> get_WordStart(This,pVal)

#define IActiveSpelledIt_ScanContext_get_WordEnd(This,pVal)	\
    (This)->lpVtbl -> get_WordEnd(This,pVal)

#define IActiveSpelledIt_ScanContext_get_Word(This,pVal)	\
    (This)->lpVtbl -> get_Word(This,pVal)

#define IActiveSpelledIt_ScanContext_Next(This)	\
    (This)->lpVtbl -> Next(This)

#define IActiveSpelledIt_ScanContext_get_Suggestions(This,pVal)	\
    (This)->lpVtbl -> get_Suggestions(This,pVal)

#define IActiveSpelledIt_ScanContext_get_Cursor(This,pVal)	\
    (This)->lpVtbl -> get_Cursor(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_get_WordFound_Proxy( 
    IActiveSpelledIt_ScanContext * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IActiveSpelledIt_ScanContext_get_WordFound_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_get_WordStart_Proxy( 
    IActiveSpelledIt_ScanContext * This,
    /* [retval][out] */ ULONG *pVal);


void __RPC_STUB IActiveSpelledIt_ScanContext_get_WordStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_get_WordEnd_Proxy( 
    IActiveSpelledIt_ScanContext * This,
    /* [retval][out] */ ULONG *pVal);


void __RPC_STUB IActiveSpelledIt_ScanContext_get_WordEnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_get_Word_Proxy( 
    IActiveSpelledIt_ScanContext * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IActiveSpelledIt_ScanContext_get_Word_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_Next_Proxy( 
    IActiveSpelledIt_ScanContext * This);


void __RPC_STUB IActiveSpelledIt_ScanContext_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_get_Suggestions_Proxy( 
    IActiveSpelledIt_ScanContext * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IActiveSpelledIt_ScanContext_get_Suggestions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IActiveSpelledIt_ScanContext_get_Cursor_Proxy( 
    IActiveSpelledIt_ScanContext * This,
    /* [retval][out] */ ULONG *pVal);


void __RPC_STUB IActiveSpelledIt_ScanContext_get_Cursor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveSpelledIt_ScanContext_INTERFACE_DEFINED__ */



#ifndef __ActiveSpelledItLib_LIBRARY_DEFINED__
#define __ActiveSpelledItLib_LIBRARY_DEFINED__

/* library ActiveSpelledItLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ActiveSpelledItLib;

EXTERN_C const CLSID CLSID_ActiveSpelledItCtl;

#ifdef __cplusplus

class DECLSPEC_UUID("2D8F976C-2869-49D4-BCC7-B1555EA76C2C")
ActiveSpelledItCtl;
#endif

EXTERN_C const CLSID CLSID_BOGUS_TO_GET_INTERFACES_EXPORTED;

#ifdef __cplusplus

class DECLSPEC_UUID("93890BC2-F754-404E-A1DA-12AA7E3AB89A")
BOGUS_TO_GET_INTERFACES_EXPORTED;
#endif
#endif /* __ActiveSpelledItLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


