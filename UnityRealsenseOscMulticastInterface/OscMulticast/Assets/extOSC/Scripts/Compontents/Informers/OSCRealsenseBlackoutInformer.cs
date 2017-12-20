/* Copyright (c) 2017 ExT (V.Sigalkin) */

using UnityEngine;

namespace extOSC.Components.Informers
{
    [AddComponentMenu("extOSC/Components/Transmitter/Realsense Blackout Informer")]
    public class OSCRealsenseBlackoutInformer : OSCTransmitterInformer<bool>
    {
        #region Protected Methods

        protected override void FillMessage(OSCMessage message, bool value)
        {
            message.AddValue(OSCValue.Float(0.0f));
            message.AddValue(OSCValue.Float(0.0f));
            message.AddValue(OSCValue.Float(0.14f));

        }

        #endregion
    }
}