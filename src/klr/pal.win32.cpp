#include "stdafx.h"

void GetNativeBootstrapperDirectory(LPTSTR szPath)
{
    DWORD dirLength = GetModuleFileName(NULL, szPath, MAX_PATH);
    for (dirLength--; dirLength >= 0 && szPath[dirLength] != _T('\\'); dirLength--);
    szPath[dirLength + 1] = _T('\0');
}
