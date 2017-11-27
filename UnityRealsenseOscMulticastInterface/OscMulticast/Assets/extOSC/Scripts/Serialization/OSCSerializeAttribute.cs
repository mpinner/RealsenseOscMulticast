/* Copyright (c) 2017 ExT (V.Sigalkin) */

using System;

namespace extOSC.Serialization
{
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public class OSCSerializeAttribute : Attribute
    { }
}