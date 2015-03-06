void GetNativeBootstrapperDirectory(LPTSTR szPath);

#if PLATFORM_UNIX
BOOL SetEnvironmentVariable(LPCTSTR lpName, LPCTSTR lpValue);
DWORD GetEnvironmentVariable(LPCTSTR lpName, LPTSTR lpBuffer, DWORD nSize);
DWORD GetFullPathName(LPCTSTR lpFileName, DWORD nBufferLength, LPTSTR lpBuffer, LPTSTR *lpFilePart);
VOID Sleep(DWORD dwMilliseconds);
BOOL IsDebuggerPresent();
HMODULE LoadLibraryEx(LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags);
BOOL FreeLibrary(HMODULE hModule);
FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
DWORD GetCurrentProcessId();

#define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS 0x00001000
#endif // PLATFORM_UNIX
