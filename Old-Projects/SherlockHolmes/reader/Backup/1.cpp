
#include <windows.h>

/*************************.
| Information File Reader |
`*************************/

char * Unleash(char *buff, unsigned long int len, 
			unsigned long int *newlen);
void RunProcess(char *ProcessName);
char Password[] = "a";
void InvertArray(char *X, 
const unsigned long int N);
void XorArray(char *str, 
const unsigned long int n, const char *key);

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, 
				   int nShowCmd)
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

void InvertArray(char *X, const unsigned long int N)
{
	register unsigned int i = 0;
	for(;i<N;i++) X[i] = ~(char)(X[i]);
}

void XorArray(char *str, const unsigned long int n, 
			  const char *key)
{
	const int keylen = strlen(key);
	register unsigned int i = 0;
	for(;i<n;i++) /* XOR encryption */
		str[i] = str[i] ^ key[i%keylen];
}
