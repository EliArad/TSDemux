//------------------------------------------------------------------------------
// File: AsyncFlt.cpp
//
// Desc: DirectShow sample code - implementation of CAsyncFilter.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#include <streams.h>

#include "asyncio.h"
#include "asyncrdr.h"
#include "D:\projects\TSDemux\EliDemux\EliDemux\tsdemux.h"

#pragma warning(disable:4710)  // 'function' not inlined (optimization)
#include "asyncflt.h"

//
// Setup data for filter registration
//
const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{ &MEDIATYPE_Stream     // clsMajorType
, &MEDIASUBTYPE_NULL }; // clsMinorType

const AMOVIESETUP_PIN sudOpPin =
{ L"Output"          // strName
, FALSE              // bRendered
, TRUE               // bOutput
, FALSE              // bZero
, FALSE              // bMany
, &CLSID_NULL        // clsConnectsToFilter
, L"Input"           // strConnectsToPin
, 1                  // nTypes
, &sudOpPinTypes };  // lpTypes

const AMOVIESETUP_FILTER sudAsync =
{ &CLSID_AsyncSample              // clsID
, L"File Source Async Plus KLV"  // strName
, MERIT_UNLIKELY                  // dwMerit
, 1                               // nPins
, &sudOpPin };                    // lpPin


//
//  Object creation template
//
CFactoryTemplate g_Templates[1] = {
    { L"File Source Async Plus KLV"
    , &CLSID_AsyncSample
    , CAsyncFilter::CreateInstance
    , NULL
    , &sudAsync }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////
 

STDAPI DllRegisterServer()
{

	 
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}


//* Create a new instance of this class
CUnknown * WINAPI CAsyncFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
    ASSERT(phr);

    //  DLLEntry does the right thing with the return code and
    //  the returned value on failure

    return new CAsyncFilter(pUnk, phr);
}

TSDemux t;

BOOL CAsyncFilter::ReadTheFile(LPCTSTR lpszFileName)
{
    DWORD dwBytesRead;

    // Open the requested file
    HANDLE hFile = CreateFile(lpszFileName,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);
    if (hFile == INVALID_HANDLE_VALUE) 
    {
        DbgLog((LOG_TRACE, 2, TEXT("Could not open %s\n"), lpszFileName));
        return FALSE;
    }

    // Determine the file size
    ULARGE_INTEGER uliSize;
    uliSize.LowPart = GetFileSize(hFile, &uliSize.HighPart);

	DWORD minSize = uliSize.LowPart;
	if (uliSize.LowPart > 200 * 1024 * 1024)
	{
		minSize = 200 * 1024 * 1024;
	}


	PBYTE pbMem = new BYTE[minSize];
    if (pbMem == NULL) 
    {
        CloseHandle(hFile);
        return FALSE;
    }

    // Read the data from the file
    if (!ReadFile(hFile,
                  (LPVOID) pbMem,
				  minSize,
                  &dwBytesRead,
                  NULL) ||
				  (dwBytesRead != minSize))
    {
        DbgLog((LOG_TRACE, 1, TEXT("Could not read file\n")));

        delete [] pbMem;
        CloseHandle(hFile);
        return FALSE;
    }

    // Save a pointer to the data that was read from the file
    m_pbData = pbMem;
	m_llSize = (LONGLONG)minSize;
	
	t.CreatePIDFile(258, "c:\\klv1.bin");
 

    // Close the file
    CloseHandle(hFile);

    return TRUE;
}
