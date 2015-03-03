#include "stdafx.h"
#include "tpa.h"

// Windows types used by the ExecuteAssembly function
typedef int32_t HRESULT;
typedef const char* LPCSTR;
typedef unsigned int DWORD;
#define S_OK 0;
#define E_FAIL -1;

typedef struct CALL_APPLICTION_MAIN_DATA
{
    char* applicationBase;
    char* runtimeDirectory;
    int argc;
    char** argv;
    int exitcode;
} *PCALL_APPLICATION_MAIN_DATA;

// Prototype of the ExecuteAssembly function from the libcoreclr.do
typedef HRESULT (*ExecuteAssemblyFunction)(
                    LPCSTR exePath,
                    LPCSTR coreClrPath,
                    LPCSTR appDomainFriendlyName,
                    int propertyCount,
                    LPCSTR* propertyKeys,
                    LPCSTR* propertyValues,
                    int argc,
                    LPCSTR* argv,
                    LPCSTR managedAssemblyPath,
                    LPCSTR entryPointAssemblyName,
                    LPCSTR entryPointTypeName,
                    LPCSTR entryPointMethodsName,
                    DWORD* exitCode);

bool GetTrustedPlatformAssembliesList(const std::string& szDirectory, bool bNative, std::string& trustedPlatformAssemblies)
{
    size_t cTpaAssemblyNames = 0;
    char** ppszTpaAssemblyNames = NULL;
    
    CreateTpaBase(&ppszTpaAssemblyNames, &cTpaAssemblyNames, bNative);

    //TODO: The Windows version of this actaully ensures the files are present.  We just fail for native and assume MSIL is present

    if (bNative)
    {
        return false;
    }

    trustedPlatformAssemblies = std::string();

    for (size_t i = 0; i < cTpaAssemblyNames; i++)
    {
        trustedPlatformAssemblies += szDirectory;
        trustedPlatformAssemblies += "/";
        trustedPlatformAssemblies += ppszTpaAssemblyNames[i];
        trustedPlatformAssemblies += ":";
    }

    if (ppszTpaAssemblyNames != NULL)
    {
        FreeTpaBase(ppszTpaAssemblyNames, cTpaAssemblyNames);
    }

    return true;
}

void* LoadCoreClr(std::string& runtimeDirectory)
{
    runtimeDirectory = std::string(getenv("CORECLR_DIR"));

    std::string coreClrDllPath;
    coreClrDllPath += getenv("CORECLR_DIR");
    coreClrDllPath += "/";
    coreClrDllPath += "libcoreclr.so";

    return dlopen(coreClrDllPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
}

extern "C" HRESULT CallApplicationMain(PCALL_APPLICATION_MAIN_DATA data)
{
    HRESULT hr = S_OK;
    size_t cchTrustedPlatformAssemblies = 0;
    std::string runtimeDirectory;

    if (data->runtimeDirectory != NULL)
    {
        runtimeDirectory = std::string(data->runtimeDirectory);
    }
    else
    {
        // TODO: This should get the directory that this library is in.
        char szCurrentDirectory[PATH_MAX];

        if (getcwd(szCurrentDirectory, PATH_MAX) == NULL)
        {
            return E_FAIL;
        }

        runtimeDirectory = std::string(szCurrentDirectory);
    }

    std::string coreClrDirectory;
    void* coreClr = LoadCoreClr(coreClrDirectory);

    if (coreClr == nullptr)
    {
        char* error = dlerror();
        fprintf(stderr, "failed to locate coreclr.dll with error %s\n", error);
        return E_FAIL;
    }

    const char* property_keys[] =
    {
        "APPBASE",
        "TRUSTED_PLATFORM_ASSEMBLIES",
        "APP_PATHS",
    };

    std::string trustedPlatformAssemblies;

    // Try native images first
    if (!GetTrustedPlatformAssembliesList(coreClrDirectory.c_str(), true, trustedPlatformAssemblies))
    {
        if (!GetTrustedPlatformAssembliesList(coreClrDirectory.c_str(), false, trustedPlatformAssemblies))
        {
            fprintf(stderr, "Failed to find files in the coreclr directory\n");
            hr = E_FAIL;
            return hr;
            // goto Finished;
        }
    }

    // Add the assembly containing the app domain manager to the trusted list
    trustedPlatformAssemblies += runtimeDirectory;
    trustedPlatformAssemblies += "kre.coreclr.managed.dll";

    std::string appPaths(runtimeDirectory);

    appPaths += ":";
    appPaths += coreClrDirectory;
    appPaths += ":";

    const char* property_values[] = {
        // APPBASE
        data->applicationBase,
        // TRUESTED_PLATFORM_ASSEMBLIES
        trustedPlatformAssemblies.c_str(),
        // APP_PATHS
        appPaths.c_str(),
    };

    ExecuteAssemblyFunction executeAssembly = (ExecuteAssemblyFunction)dlsym(coreClr, "ExecuteAssembly");

    if (executeAssembly == nullptr)
    {
        fprintf(stderr, "Could not find ExecuteAssembly entrypoint in coreclr.\n");
        return E_FAIL;
    }

    std::string coreClrDllPath(coreClrDirectory);
    coreClrDllPath += "/";
    coreClrDllPath += "libcoreclr.so";

    hr = executeAssembly("/home/matell/.k/runtimes/kre-coreclr.1.0.0-dev/bin/klr",
                         coreClrDllPath.c_str(),
                         "kre.coreclr.managed",
                         sizeof(property_keys) / sizeof(property_keys[0]),
                         property_keys,
                         property_values,
                         data->argc,
                         (const char**)data->argv,
                         nullptr,
                         "kre.coreclr.managed, Version=0.1.0.0",
                         "DomainManager",
                         "Execute",
                         (DWORD*)&(data->exitcode));


    dlclose(coreClr);

    return hr;
}

int main(int argc, char** argv)
{
    CALL_APPLICTION_MAIN_DATA callData;
    callData.applicationBase = "/home/matell/helloworld/bin/Debug/aspnetcore50/";
    callData.runtimeDirectory = "/home/matell/.k/runtimes/kre-coreclr.1.0.0-dev/bin/";
    
    char** strippedArgv = (char**) calloc(argc - 1, sizeof(char*));

    for (int i = 0; i < argc - 1; i++)
    {
        strippedArgv[i] = argv[i + 1];
    }

    callData.argc = argc - 1;
    callData.argv = strippedArgv;

    CallApplicationMain(&callData);

    return callData.exitcode;
}
