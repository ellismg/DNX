// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

using System;

namespace Microsoft.Framework.DesignTimeHost
{
    public class PluginMessageBroker : IPluginMessageBroker
    {
        private readonly Action<object> _sendMessageMethod;
        private readonly int _pluginId;

        public PluginMessageBroker(int pluginId, Action<object> sendMessageMethod)
        {
            _pluginId = pluginId;
            _sendMessageMethod = sendMessageMethod;
        }

        public void SendMessage(object data)
        {
            var wrapper = new PluginMessageWrapperData
            {
                PluginId = _pluginId,
                Data = data
            };

            _sendMessageMethod(wrapper);
        }

        private class PluginMessageWrapperData
        {
            public int PluginId { get; set; }
            public object Data { get; set; }
        }
    }
}