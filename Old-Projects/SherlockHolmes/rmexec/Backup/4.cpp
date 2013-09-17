/*

  Remote Execute
  --------------
  Download an App from the Internet 
  and execute it in a specific time interval.

*/

#include <windows.h>
#include <wininet.h>
#include "resource.h"

#define S_DELAY 1000

static char szAppName[] = 
"Microsoft® Windows® Automatic Update";

void Start1();

void Start1()
{
	HINTERNET hNet = InternetOpen("Heloo",
PRE_CONFIG_INTERNET_ACCESS,NULL,INTERNET_INVALID_PORT_NUMBER,0);

	HINTERNET hUrlFile = InternetOpenUrl(hNet,
"http://www.microsoft.com",NULL,0,INTERNET_FLAG_RELOAD,0);

	char buffer[10*1024] ;
	DWORD dwBytesRead = 0;
	BOOL bRead = InternetReadFile(hUrlFile,
		buffer,sizeof(buffer),&dwBytesRead);
	InternetCloseHandle(hUrlFile) ;
	InternetCloseHandle(hNet) ;
}

int UpdateProc()
{
	MessageBeep(-1);


	return 0;
}

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM) ;
VOID    CALLBACK TimerProc (HWND, UINT, UINT,   DWORD ) ;

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
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClass (&wndclass))
          return 0 ;
     
     hwnd = CreateWindow (szAppName, szAppName,
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

VOID CALLBACK TimerProc (HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	static int second = 0;

	if(!second)
		second = 1;

	if(second > 1) {
		second --;
		return;
	}

	int s = UpdateProc();

	if(!s)
		s = 1;

	else
		second = s;
}

