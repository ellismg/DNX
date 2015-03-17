#include "stdafx.h"
#include "tpa.h"
#include <assert.h>

// Windows types used by the ExecuteAssembly function
typedef int32_t HRESULT;
typedef const char* LPCSTR;
typedef uint32_t DWORD;

typedef struct CALL_APPLICATION_MAIN_DATA
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

const HRESULT S_OK = 0;
const HRESULT E_FAIL = -1;

bool GetTrustedPlatformAssembliesList(const std::string& tpaDirectory, bool isNative, std::string& trustedPlatformAssemblies)
{
    size_t cTpaAssemblyNames = 0;
    LPCTSTR* ppszTpaAssemblyNames = nullptr;

    CreateTpaBase(&ppszTpaAssemblyNames, &cTpaAssemblyNames, isNative);

    assert(ppszTpaAssemblyNames != nullptr || cTpaAssemblyNames == 0);

    //TODO: The Windows version of this actaully ensures the files are present.  We just fail for native and assume MSIL is present
    if (isNative)
    {
        return false;
    }

    for (size_t i = 0; i < cTpaAssemblyNames; i++)
    {
        trustedPlatformAssemblies.append(tpaDirectory);
        trustedPlatformAssemblies.append("/");
        trustedPlatformAssemblies.append(ppszTpaAssemblyNames[i]);
        trustedPlatformAssemblies.append(":");
    }

    if (ppszTpaAssemblyNames)
    {
        FreeTpaBase(ppszTpaAssemblyNames, cTpaAssemblyNames);
    }

    return true;
}

// TODO: Figure out if runtimeDirectory should have a trailing "/".  Need to look at what happens on Windows.
void* LoadCoreClr(std::string& runtimeDirectory)
{
    void* ret = nullptr;

    char* coreClrEnvVar = getenv("CORECLR_DIR");

    if (coreClrEnvVar)
    {
        runtimeDirectory = coreClrEnvVar;

        std::string coreClrDllPath = runtimeDirectory;
        coreClrDllPath.append("/");
        coreClrDllPath.append("libcoreclr.so");

        ret = dlopen(coreClrDllPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    }

    if (!ret)
    {
        // Try to load coreclr from application path.
        char pathToBootstrapper[PATH_MAX];
        ssize_t pathLen = readlink("/proc/self/exe", pathToBootstrapper, PATH_MAX - 1);

        if (pathLen != -1)
        {
            pathToBootstrapper[pathLen] = '\0';
            runtimeDirectory.assign(pathToBootstrapper);

            size_t lastSlash = runtimeDirectory.rfind('/');

            if (lastSlash != std::string::npos)
            {
                runtimeDirectory.erase(lastSlash);

                std::string coreClrDllPath = runtimeDirectory;
                coreClrDllPath.append("/");
                coreClrDllPath.append("libcoreclr.so");

                ret = dlopen(coreClrDllPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
            }
        }
    }

    return ret;
}

extern "C" HRESULT CallApplicationMain(PCALL_APPLICATION_MAIN_DATA data)
{
    HRESULT hr = S_OK;
    size_t cchTrustedPlatformAssemblies = 0;
    std::string runtimeDirectory;

    if (data->runtimeDirectory)
    {
        runtimeDirectory = data->runtimeDirectory;
    }
    else
    {
        // TODO: This should get the directory that this library is in, not the CWD.
        char szCurrentDirectory[PATH_MAX];

        if (!getcwd(szCurrentDirectory, PATH_MAX))
        {
            return E_FAIL;
        }

        runtimeDirectory = std::string(szCurrentDirectory);
    }

    std::string coreClrDirectory;
    void* coreClr = LoadCoreClr(coreClrDirectory);

    if (!coreClr)
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
        }
    }

    // Add the assembly containing the app domain manager to the trusted list
    trustedPlatformAssemblies.append(runtimeDirectory);
    trustedPlatformAssemblies.append("dnx.coreclr.managed.dll");

    std::string appPaths(runtimeDirectory);

    appPaths.append(":");
    appPaths.append(coreClrDirectory);
    appPaths.append(":");

    const char* property_values[] = {
        // APPBASE
        data->applicationBase,
        // TRUESTED_PLATFORM_ASSEMBLIES
        trustedPlatformAssemblies.c_str(),
        // APP_PATHS
        appPaths.c_str(),
    };

    ExecuteAssemblyFunction executeAssembly = (ExecuteAssemblyFunction)dlsym(coreClr, "ExecuteAssembly");

    if (!executeAssembly)
    {
        fprintf(stderr, "Could not find ExecuteAssembly entrypoint in coreclr.\n");
        return E_FAIL;
    }

    std::string coreClrDllPath(coreClrDirectory);
    coreClrDllPath.append("/");
    coreClrDllPath.append("libcoreclr.so");

    char pathToBootstrapper[PATH_MAX];
    ssize_t pathLen = readlink("/proc/self/exe", pathToBootstrapper, PATH_MAX - 1);

    if (pathLen == -1)
    {
        fprintf(stderr, "Could not locate full bootstrapper path.\n");
        return E_FAIL;
    }

    pathToBootstrapper[pathLen] = '\0';

    hr = executeAssembly(pathToBootstrapper,
                         coreClrDllPath.c_str(),
                         "dnx.coreclr.managed",
                         sizeof(property_keys) / sizeof(property_keys[0]),
                         property_keys,
                         property_values,
                         data->argc,
                         (const char**)data->argv,
                         nullptr,
                         "dnx.coreclr.managed, Version=0.1.0.0",
                         "DomainManager",
                         "Execute",
                         (DWORD*)&(data->exitcode));


    dlclose(coreClr);

    return hr;
}
