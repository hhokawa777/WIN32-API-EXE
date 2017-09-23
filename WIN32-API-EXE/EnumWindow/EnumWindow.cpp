/*----------------------------------------------------------------------------
EnumWindows API EXE command
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>

#ifdef UNICODE
#define _tfprintf fwprintf
#else
#define _tfprintf fprintf
#endif

#define MAXBUFFSIZE		1000
#define OPT_NODATA		(0x00000000L)
#define OPT_TEXT		(0x00000001L)
#define OPT_CLASS		(0x00000002L)
#define OPT_PARENT		(0x00000004L)

typedef struct tagPARAMS {
	DWORD	dwOptFlags;
	TCHAR	szSelfCmdName[MAXBUFFSIZE];
	TCHAR	szText[MAXBUFFSIZE];
	TCHAR	szClass[MAXBUFFSIZE];
	HWND	hParentWnd;
} PARAMS;

PARAMS	prm;

/*----------------------------------------------------------------------------
EnumWindows Callback
----------------------------------------------------------------------------*/
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR	szClass[MAXBUFFSIZE];
	TCHAR	szText[MAXBUFFSIZE];
	HWND	hParentWnd;

	// if class name filter specified
	if (0 != (prm.dwOptFlags & OPT_CLASS)) {
		// call win32 api
		GetClassName(hWnd, szClass, sizeof(szClass));
		// if not partly matched
		if (0 != _tcscmp(szClass, prm.szClass)) {
			return TRUE;	// continue enumeration
		}
	}
	// if window text filter specified
	if (0 != (prm.dwOptFlags & OPT_TEXT)) {
		// call win32 api
		GetWindowText(hWnd, szText, sizeof(szText));
		// if not partly matched
		if (NULL == _tcsstr(szText, prm.szText)) {
			return TRUE;	// continue enumeration
		}
		// ignore this command itself
		//if (NULL != _tcsstr(szText, prm.szSelfCmdName)) {
		//return TRUE;	// continue callback enumeration
		//}
	}
	// if finding parent window
	if (0 != (prm.dwOptFlags & OPT_PARENT)) {
		// call win32 api
		hParentWnd = GetParent(hWnd);
		// if not matched
		if (prm.hParentWnd != hParentWnd) {
			return TRUE;	// continue enumeration
		}
	}

	// print out window handle
	_tprintf(_T("%ld\n"), (long)hWnd);

	//GetClassName(hWnd, szClass, sizeof(szClass));
	//_tprintf(_T("%s\n"), szClass);
	//GetWindowText(hWnd, szText, sizeof(szText));
	//_tprintf(_T("%s\n"), szText);
	
	return TRUE;	// continue enumeration
}
/*----------------------------------------------------------------------------
Check and Read arguments
----------------------------------------------------------------------------*/
LRESULT ReadArgs(int argc, TCHAR *argv[])
{
	int 	iCt;
	LPTSTR	lpszOption;

	//	if(argc < 3){
	//		return ERROR_DISPHELP;
	//	}

	*prm.szText = '\0';
	*prm.szClass = '\0';
	prm.dwOptFlags = OPT_NODATA;
	for (iCt = 1; iCt < argc; iCt += 2) {
		if ((*argv[iCt] == '-') || (*argv[iCt] == '/')) {
			lpszOption = (argv[iCt] + 1);
			// if window text filter specified
			if (_tcscmp(lpszOption, _T("win")) == 0) {	// -win "any window text"
				if (iCt + 1 >= argc) { return ERROR_INVALID_PARAMETER; }
				_tcscpy_s(prm.szText, argv[iCt + 1]);
				prm.dwOptFlags |= OPT_TEXT;
			}
			// if class name filter specified
			else if (_tcscmp(lpszOption, _T("cls")) == 0) {	// -cls "any class name"
				if (iCt + 1 >= argc) { return ERROR_INVALID_PARAMETER; }
				_tcscpy_s(prm.szClass, argv[iCt + 1]);
				prm.dwOptFlags |= OPT_CLASS;
			}
			// if finding parent window handle
			else if (_tcscmp(lpszOption, _T("parent")) == 0) { // -parent "parent window handle"
				if (iCt + 1 >= argc) { return ERROR_INVALID_PARAMETER; }
				prm.hParentWnd = (HWND)_ttol(argv[iCt + 1]);
				prm.dwOptFlags |= OPT_PARENT;
			}
		}
	}
	_tcscpy_s(prm.szSelfCmdName, argv[0]);
	return ERROR_SUCCESS;
}
/*----------------------------------------------------------------------------
Main
----------------------------------------------------------------------------*/
int _tmain(int argc, TCHAR *argv[])
{
	LRESULT ret;
	DWORD dwErrCd;
	int iExitCd;

	int mode = _O_U8TEXT; //* _O_WTEXT, _O_U16TEXT, _O_U8TEXT 
	_setmode(_fileno(stdout), mode);
	_setmode(_fileno(stderr), mode);
	//_setmode(_fileno(stdin), mode);

	// check and read args
	if ((ret = ReadArgs(argc, argv)) != ERROR_SUCCESS) {
		// show error
		_tfprintf(stderr, _T("\nInvalid arguments."));
		fflush(stderr);
		iExitCd = EXIT_FAILURE;
	} else {
		SetLastError(0);
		// call target API
		//---------------------------------------------------------
		ret = EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)0);
		//---------------------------------------------------------
		dwErrCd = GetLastError();

		// if success
		if (ret != 0 && dwErrCd == 0) {
			iExitCd = EXIT_SUCCESS;
		} else {
			// show error code
			_tfprintf(stderr, _T("\nError: %li"), (long)dwErrCd);
			fflush(stderr);
			iExitCd = EXIT_FAILURE;
		}
	}

	return iExitCd;
}
