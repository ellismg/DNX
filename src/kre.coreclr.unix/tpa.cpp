// This file will be dynamically updated during build to generate a 
// minimal trusted platform assemblies list

#include "stdafx.h"
#include "tpa.h"

typedef char* LPSTR;

bool CreateTpaBase(char*** ppNames, size_t* pcNames, bool bNative)
{
    const size_t count = 31;
    char** pArray = (char**) calloc(count, sizeof(char*));

    if (bNative)
    {
        pArray[0] = strdup("Internal.Runtime.Uri.ni.dll");
        pArray[1] = strdup("Internal.Uri.ni.dll");
        pArray[2] = strdup("mscorlib.ni.dll");
        pArray[3] = strdup("System.ni.dll");
        pArray[4] = strdup("System.AppContext.ni.dll");
        pArray[5] = strdup("System.Collections.ni.dll");
        pArray[6] = strdup("System.Collections.Concurrent.ni.dll");
        pArray[7] = strdup("System.ComponentModel.ni.dll");
        pArray[8] = strdup("System.Console.ni.dll");
        pArray[9] = strdup("System.Core.ni.dll");
        pArray[10] = strdup("System.Diagnostics.Debug.ni.dll");
        pArray[11] = strdup("System.Globalization.ni.dll");
        pArray[12] = strdup("System.IO.ni.dll");
        pArray[13] = strdup("System.IO.FileSystem.ni.dll");
        pArray[14] = strdup("System.IO.FileSystem.Primitives.ni.dll");
        pArray[15] = strdup("System.Linq.ni.dll");
        pArray[16] = strdup("System.Reflection.ni.dll");
        pArray[17] = strdup("System.Reflection.Extensions.ni.dll");
        pArray[18] = strdup("System.Reflection.Primitives.ni.dll");
        pArray[19] = strdup("System.Resources.ResourceManager.ni.dll");
        pArray[20] = strdup("System.Runtime.ni.dll");
        pArray[21] = strdup("System.Runtime.Extensions.ni.dll");
        pArray[22] = strdup("System.Runtime.Handles.ni.dll");
        pArray[23] = strdup("System.Runtime.InteropServices.ni.dll");
        pArray[24] = strdup("System.Runtime.Loader.ni.dll");
        pArray[25] = strdup("System.Text.Encoding.ni.dll");
        pArray[26] = strdup("System.Text.Encoding.Extensions.ni.dll");
        pArray[27] = strdup("System.Threading.ni.dll");
        pArray[28] = strdup("System.Threading.Overlapped.ni.dll");
        pArray[29] = strdup("System.Threading.Tasks.ni.dll");
        pArray[30] = strdup("System.Threading.ThreadPool.ni.dll");
    }
    else
    {
        pArray[0] = strdup("Internal.Runtime.Uri.dll");
        pArray[1] = strdup("Internal.Uri.dll");
        pArray[2] = strdup("mscorlib.dll");
        pArray[3] = strdup("System.dll");
        pArray[4] = strdup("System.AppContext.dll");
        pArray[5] = strdup("System.Collections.dll");
        pArray[6] = strdup("System.Collections.Concurrent.dll");
        pArray[7] = strdup("System.ComponentModel.dll");
        pArray[8] = strdup("System.Console.dll");
        pArray[9] = strdup("System.Core.dll");
        pArray[10] = strdup("System.Diagnostics.Debug.dll");
        pArray[11] = strdup("System.Globalization.dll");
        pArray[12] = strdup("System.IO.dll");
        pArray[13] = strdup("System.IO.FileSystem.dll");
        pArray[14] = strdup("System.IO.FileSystem.Primitives.dll");
        pArray[15] = strdup("System.Linq.dll");
        pArray[16] = strdup("System.Reflection.dll");
        pArray[17] = strdup("System.Reflection.Extensions.dll");
        pArray[18] = strdup("System.Reflection.Primitives.dll");
        pArray[19] = strdup("System.Resources.ResourceManager.dll");
        pArray[20] = strdup("System.Runtime.dll");
        pArray[21] = strdup("System.Runtime.Extensions.dll");
        pArray[22] = strdup("System.Runtime.Handles.dll");
        pArray[23] = strdup("System.Runtime.InteropServices.dll");
        pArray[24] = strdup("System.Runtime.Loader.dll");
        pArray[25] = strdup("System.Text.Encoding.dll");
        pArray[26] = strdup("System.Text.Encoding.Extensions.dll");
        pArray[27] = strdup("System.Threading.dll");
        pArray[28] = strdup("System.Threading.Overlapped.dll");
        pArray[29] = strdup("System.Threading.Tasks.dll");
        pArray[30] = strdup("System.Threading.ThreadPool.dll");
    }

    *ppNames = pArray;
    *pcNames = count;

    return true;
}

bool FreeTpaBase(char** values, const size_t count)
{
    for (size_t idx = 0; idx < count; ++idx)
    {
        free(values[idx]);
    }

    free(values);

    return true;
}
