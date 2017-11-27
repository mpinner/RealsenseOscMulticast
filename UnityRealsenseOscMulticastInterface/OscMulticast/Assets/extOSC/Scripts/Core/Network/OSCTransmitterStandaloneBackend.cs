/* Copyright (c) 2017 ExT (V.Sigalkin) */

#if !NETFX_CORE
using UnityEngine;

using System.Net;
using System.Net.Sockets;

namespace extOSC.Core.Network
{
    public class OSCTransmitterStandaloneBackend : OSCTransmitterBackend
    {
        #region Public Vars

        public override bool IsAvaible
        {
            get { return _client != null; }
        }

        #endregion

        #region Private Vars

        private IPEndPoint _ipEndPoint;

        private UdpClient _client;

        #endregion

        #region Public Methods

        public override void Connect(string remoteHost, int remotePort)
        {
            if (_client != null)
                Close();

            _client = new UdpClient();
            _client.DontFragment = true;

            _ipEndPoint = CreateEndPoint(remoteHost, remotePort);
        }

        public override void RefreshConnection(string remoteHost, int remotePort)
        {
            _ipEndPoint = CreateEndPoint(remoteHost, remotePort);
        }

        public override void Close()
        {
            if (_client != null)
                _client.Close();

            _client = null;
        }

        public override void Send(byte[] data)
        {
            if (_client == null) return;

            try
            {
                _client.Send(data, data.Length, _ipEndPoint);
            }
            catch (SocketException exception)
            {
                Debug.LogWarningFormat("[OSCTranmitter] Error: {0}", exception);
            }

        }

        #endregion

        #region Private Methods

        private IPEndPoint CreateEndPoint(string remoteHost, int remotePort, bool showWarning = true)
        {
            IPAddress ipAddress;

            if (!IPAddress.TryParse(remoteHost, out ipAddress))
            {
                if (showWarning) Debug.LogWarningFormat("[OSCTranmitter] Invalid IP address: {0}.", remoteHost);
                return null;
            }

            return new IPEndPoint(ipAddress, remotePort);
        }

        #endregion
    }
}
#endif