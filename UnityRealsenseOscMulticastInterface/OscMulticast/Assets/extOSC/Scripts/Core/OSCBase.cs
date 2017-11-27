/* Copyright (c) 2017 ExT (V.Sigalkin) */

using UnityEngine;

using System;

using extOSC.Mapping;

namespace extOSC.Core
{
    [ExecuteInEditMode]
    public abstract class OSCBase : MonoBehaviour, IDisposable
    {
        #region Public Vars

        public bool AutoConnect
        {
            get { return autoConnect; }
            set { autoConnect = value; }
        }

        public abstract bool IsAvaible { get; }

        public OSCMapBundle MapBundle
        {
            get { return mapBundle; }
            set { mapBundle = value; }
        }

        public bool WorkInEditor
        {
            get { return workInEditor; }
            set { workInEditor = value; }
        }

        #endregion

        #region Protected Vars

        [SerializeField]
        protected bool autoConnect = true;

        [SerializeField]
        protected OSCMapBundle mapBundle;

        protected System.Object _lock = new System.Object();

        protected bool restoreOnEnable;

        [SerializeField]
        protected bool workInEditor;

        #endregion

        #region Unity Methods

        protected virtual void Start()
        {
            if (!Application.isPlaying) return;

            if (AutoConnect) Connect();
        }

        protected virtual void OnEnable()
        {
            if (Application.isPlaying && restoreOnEnable)
                Connect();
        }

        protected virtual void OnDisable()
        {
            restoreOnEnable = IsAvaible;

            if (Application.isPlaying && restoreOnEnable)
                Close();
        }

        protected virtual void OnDestroy()
        {
            Dispose();
        }

        #endregion

        #region Public Methods

        public abstract void Connect();

        public abstract void Close();

        public void Dispose()
        {
            Dispose(true);

            GC.SuppressFinalize(this);
        }

        #endregion

        #region Protected Methods

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                ///IDK, rly...
            }

            Close();
        }

        #endregion
    }
}