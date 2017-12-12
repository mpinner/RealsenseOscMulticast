/* Copyright (c) 2017 ExT (V.Sigalkin) */

using UnityEngine;

namespace extOSC.Components.Informers
{
    [AddComponentMenu("extOSC/Components/Transmitter/Realsense Right Informer")]
    public class OSCRealsenseRightInformer : OSCTransmitterInformer<Vector2>
    {
        #region Protected Methods

        protected override void FillMessage(OSCMessage message, Vector2 value)
        {
            message.AddValue(OSCValue.Float(value.x * -1f));
            message.AddValue(OSCValue.Float(0.0f));
            message.AddValue(OSCValue.Float(value.y));

        }

        #endregion
    }
}