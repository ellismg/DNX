// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.InteropServices;
using kre.hosting;
using Microsoft.Framework.Runtime;

public class DomainManager : AppDomainManager
{
    private ApplicationMainInfo _info;

    public override void InitializeNewDomain(AppDomainSetup appDomainInfo)
    {
        _info.Main = Main;
        BindApplicationMain(ref _info);

        if (!string.IsNullOrEmpty(_info.ApplicationBase))
        {
            Environment.SetEnvironmentVariable(EnvironmentNames.AppBase, _info.ApplicationBase);
        }

        appDomainInfo.ApplicationBase = Environment.GetEnvironmentVariable(EnvironmentNames.DefaultLib);
    }

    private int Main(int argc, string[] argv)
    {
        return RuntimeBootstrapper.Execute(argv);
    }

    [DllImport(Constants.BootstrapperClrName + ".dll")]
    private extern static void BindApplicationMain(ref ApplicationMainInfo info);

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 1)]
    public struct ApplicationMainInfo
    {
        [MarshalAs(UnmanagedType.FunctionPtr)]
        public MainDelegate Main;

        [MarshalAs(UnmanagedType.BStr)]
        public String ApplicationBase;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate int MainDelegate(
        int argc,
        [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr, SizeParamIndex = 0)] String[] argv);
}
