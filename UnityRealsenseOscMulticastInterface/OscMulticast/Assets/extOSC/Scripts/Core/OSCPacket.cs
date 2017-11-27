/* Copyright (c) 2017 ExT (V.Sigalkin) */

using System;
using System.Net;

#if NETFX_CORE
using System.Reflection;
#endif

namespace extOSC.Core
{
    abstract public class OSCPacket
    {
        #region Static Public Methods

        public static bool IsBundle(OSCPacket packet)
        {
            var type = packet.GetType();
#if !NETFX_CORE
            return (type.IsSubclassOf(typeof(OSCBundle)) || type == typeof(OSCBundle));
#else
            return (type.GetTypeInfo().IsSubclassOf(typeof(OSCBundle)) || type == typeof(OSCBundle));
#endif
        }

        public static string ToBase64String(OSCPacket packet)
        {
            return Convert.ToBase64String(OSCConverter.Pack(packet));
        }

        public static OSCPacket FromBase64String(string base64String)
        {
            return OSCConverter.Unpack(Convert.FromBase64String(base64String));
        }

        #endregion

        #region Public Vars

        public virtual int Port
        {
            get { return port; }
            set { port = value; }
        }

        public virtual IPAddress Ip
        {
            get { return ip; }
            set { ip = value; }
        }

        public virtual string Address
        {
            get { return address; }
            set { address = value; }
        }

        #endregion

        #region Protected Vars

        protected int port;

        protected IPAddress ip;

        protected string address;

        #endregion

        #region Public Methods

        public bool IsBundle() { return IsBundle(this); }

        #endregion
    }
}