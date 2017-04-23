/*----------------------------------------------------------------------------
GetWindowText API EXE command
    
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

TCHAR	szText[MAXBUFFSIZE];

typedef struct tagPARAMS {
	HWND	hWnd;
} PARAMS;
PARAMS	prm;

/*----------------------------------------------------------------------------
Check and Read arguments
----------------------------------------------------------------------------*/
LRESULT ReadArgs(int argc, TCHAR *argv[])
{
	// wrong param count
	if (argc != 2) {
		return ERROR_INVALID_PARAMETER;
	}
	// set arguments
	int iCt = 0;
	prm.hWnd = (HWND)_ttol(argv[iCt + 1]);
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
	}
	else {
		SetLastError(0);
		// call target API
		//---------------------------------------------------------
		ret = GetWindowText(prm.hWnd, szText, sizeof(szText));
		//---------------------------------------------------------
		dwErrCd = GetLastError();

		// if success
		if (ret != 0 && dwErrCd == 0) {
			// output result
			_tprintf(_T("%s\n"), szText);
			iExitCd = EXIT_SUCCESS;
		}
		else {
			// show error code
			_tfprintf(stderr, _T("\nError: %li"), (long)dwErrCd);
			fflush(stderr);
			iExitCd = EXIT_FAILURE;
		}
	}

	return iExitCd;
}