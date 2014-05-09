// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

using NuGet;
using System;
using System.Collections.Generic;
using System.Web;

namespace Microsoft.Framework.PackageManager
{
    public class PackageInfo
    {
        public string Id { get; set; }
        public SemanticVersion Version { get; set; }
        public string ContentUri { get; set; }
    }
}