#include "stdafx.h"

int _tprintf_s(LPCTSTR format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);

    return ret;
}

int _tcscpy_s(LPTSTR strDestination, size_t numberOfElements, LPCTSTR strSrc)
{
    if (numberOfElements == 0)
    {
        return -1;
    }

    strncpy(strDestination, strSrc, numberOfElements);
    
    if(strDestination[numberOfElements - 1] != '\0')
    {
        strDestination[0] = '\0';
    }

    return strDestination[numberOfElements - 1] != '\0';
}
