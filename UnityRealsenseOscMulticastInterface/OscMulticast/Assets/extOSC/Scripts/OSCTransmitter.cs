/* Copyright (c) 2017 ExT (V.Sigalkin) */

using UnityEngine;

using System.Collections.Generic;

using extOSC.Core;
using extOSC.Core.Console;
using extOSC.Core.Network;

namespace extOSC
{
    [AddComponentMenu("extOSC/OSC Transmitter")]
    public class OSCTransmitter : OSCBase
    {
        #region Public Vars

        public override bool IsAvaible
        {
            get
            {
                if (transmitterBackend != null)
                    return transmitterBackend.IsAvaible;

                return false;
            }
        }

        public string RemoteHost
        {
            get { return remoteHost; }
            set
            {
                if (remoteHost == value)
                    return;

                transmitterBackend.RefreshConnection(remoteHost, remotePort);

                remoteHost = value;
            }
        }

        public int RemotePort
        {
            get { return remotePort; }
            set
            {
                value = OSCUtilities.ClampPort(value);

                if (remotePort == value)
                    return;

                transmitterBackend.RefreshConnection(remoteHost, remotePort);

                remotePort = value;
            }
        }

        public bool UseBundle
        {
            get { return useBundle; }
            set { useBundle = value; }
        }

        #endregion

        #region Protected Vars

        [SerializeField]
        protected string remoteHost = "127.0.0.1";

        [SerializeField]
        protected int remotePort = 7000;

        [SerializeField]
        protected bool useBundle;

        protected OSCTransmitterBackend transmitterBackend
        {
            get
            {
                if (_transmitterBackend == null)
                    _transmitterBackend = OSCTransmitterBackend.Create();

                return _transmitterBackend;
            }
        }

        #endregion

        #region Private Vars

        private readonly List<OSCPacket> _packetPool = new List<OSCPacket>();

        private OSCTransmitterBackend _transmitterBackend;

        #endregion

        #region Unity Methods

        protected virtual void Update()
        {
            if (_packetPool.Count > 0)
            {
                var bundle = new OSCBundle();

                foreach (var packet in _packetPool)
                {
                    bundle.AddPacket(packet);
                }

                Send(bundle);

                _packetPool.Clear();
            }
        }

#if UNITY_EDITOR
        protected void OnValidate()
        {
            remotePort = OSCUtilities.ClampPort(remotePort);
            transmitterBackend.RefreshConnection(remoteHost, remotePort);
        }
#endif

        #endregion

        #region Public Methods

        public override void Connect()
        {
            transmitterBackend.Connect(remoteHost, remotePort);
        }

        public override void Close()
        {
            transmitterBackend.Close();
        }

        public override string ToString()
        {
            return string.Format("<{0} (Host: {1}, Port: {2})>", GetType(), remoteHost, remotePort);
        }

        public void Send(OSCPacket packet)
        {
            if (useBundle && packet != null && (packet is OSCMessage))
            {
                _packetPool.Add(packet);

                return;
            }

            if (mapBundle != null)
                mapBundle.Map(packet);

            var data = OSCConverter.Pack(packet);

            transmitterBackend.Send(data);

            OSCConsole.Transmitted(this, packet);
        }

        public virtual void Send(string address, OSCValue value)
        {
            var message = new OSCMessage(address);
            message.AddValue(value);

            Send(message);
        }

        #endregion
    }
}