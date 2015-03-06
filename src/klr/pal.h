void GetNativeBootstrapperDirectory(LPTSTR szPath);

#if PLATFORM_UNIX
typedef int BOOL;
typedef uint32_t DWORD;

typedef char TCHAR;
typedef char* LPTSTR;
typedef const char* LPTCSTR;

#define _T(x) x
#define MAX_PATH PATH_MAX

int _tcsnicmp(LPCTSTR string1, LPCTSTR string2, size_t count);
int _tcsnlen(LPCTSTR string, size_t count);
int _tprintf_s(LPCTSTR format, ...);

BOOL SetEnvironmentVarible(LPCTSTR lpName, LPCTSTR lpValue);
DWORD GetEnvironmentVarible(LPCTSTR lpName, LPTSTR lpBuffer, DWORD nSize);
DWORD GetFullPathName(LPCTSTR lpFileName, DWORD nBufferLength, LPTSTR lpBuffer, LPTSTR *lpFilePart);
VOID Sleep(DWORD dwMilliseconds);
BOOL IsDebuggerPresent();

#endif // PLATFORM_UNIX
