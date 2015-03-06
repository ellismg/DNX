#include "stdafx.h"
#include <assert.h>
#include <dlfcn.h>

void GetNativeBootstrapperDirectory(LPTSTR szPath)
{
    size_t ret = readlink("/proc/self/exe", szPath, PATH_MAX - 1);

    assert(ret != -1);

    szPath[ret] = '\0';
    
    size_t lastSlash = 0;

    for (size_t i = 0; szPath[i] != '\0'; i++)
    {
        if (szPath[i] == '/')
        {
            lastSlash = i;
        }
    }

    szPath[lastSlash] = '\0';
}

BOOL SetEnvironmentVariable(LPCTSTR lpName, LPCTSTR lpValue)
{
    int ret;

    if (lpValue == nullptr)
    {
        ret = setenv(lpName, lpValue, 1);
    }
    else
    {
        ret = unsetenv(lpName);
    }

    return ret == 0;
}

DWORD GetEnvironmentVariable(LPCTSTR lpName, LPTSTR lpBuffer, DWORD nSize)
{
    char* envValue = getenv(lpName);

    if (envValue == nullptr)
    {
        return 0;
    }

    size_t len = strlen(envValue);

    if (len < nSize)
    {
        memcpy(lpBuffer, envValue, len + 1);
        return len;
    }

    // not enough space
    return len + 1;
}

DWORD GetFullPathName(LPCTSTR lpFileName, DWORD nBufferLength, LPTSTR lpBuffer, LPTSTR *lpFilePart)
{
    if (nBufferLength < PATH_MAX)
    {
        return 0;
    }

    // NOTE: We don't need to emulate this behavior for klr today.
    if (lpFilePart != nullptr)
    {
        return 0;
    }

    if(realpath(lpFileName, lpBuffer) != nullptr)
    {
        return strlen(lpBuffer);
    }
    else
    {
        return 0;
    }
}

VOID Sleep(DWORD dwMilliseconds)
{
    usleep(dwMilliseconds * 1000);
}

BOOL IsDebuggerPresent()
{
    // TODO: Implement this.  procfs will be able to tell us this.
    return FALSE;
}

HMODULE LoadLibraryEx(LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    // NOTE: We ignore dwFlags and just search next to the app.
    char localPath[PATH_MAX];
    
    GetNativeBootstrapperDirectory(localPath);
    
    strcat(localPath, "/");
    strcat(localPath, lpFileName);

    return dlopen(localPath, RTLD_NOW | RTLD_GLOBAL);
}

BOOL FreeLibrary(HMODULE hModule)
{
    if (hModule != nullptr)
    {
        return dlclose(hModule);
    }

    return TRUE;
}

FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    return dlsym(hModule, lpProcName);
}

DWORD GetCurrentProcessId()
{
    return getpid();
}
