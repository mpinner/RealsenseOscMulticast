/* Copyright (c) 2017 ExT (V.Sigalkin) */

using UnityEngine;
using UnityEditor;

using extOSC.Components;

namespace extOSC.Editor.Components
{
    [CustomEditor(typeof(OSCReceiverComponent), true)]
    public class OSCReceiverComponentEditor : UnityEditor.Editor
    {
        #region Static Private Vars

        private static readonly GUIContent _receiverComponentSettingsContent = new GUIContent("Receiver Component Settings:");

        private static readonly GUIContent _otherSettingsContent = new GUIContent("Other Settings:");

        #endregion

        #region Private Vars

        private SerializedProperty _receiverProperty;

        private SerializedProperty _addressProperty;

        #endregion

        #region Unity Methods

        protected virtual void OnEnable()
        {
            _receiverProperty = serializedObject.FindProperty("receiver");
            _addressProperty = serializedObject.FindProperty("address");
        }

        protected virtual void OnDisable()
        { }

        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            EditorGUI.BeginChangeCheck();

            // LOGO
            GUILayout.Space(10);
            OSCEditorLayout.DrawLogo();
            GUILayout.Space(5);

            GUILayout.BeginVertical("box");

            // SETTINGS BLOCK
            EditorGUILayout.LabelField(_receiverComponentSettingsContent, EditorStyles.boldLabel);
            OSCEditorLayout.ReceiverSettings(_receiverProperty, _addressProperty);

            DrawSettings();

            EditorGUILayout.EndVertical();

            if (EditorGUI.EndChangeCheck())
                serializedObject.ApplyModifiedProperties();
        }

        #endregion

        #region Protected Methods

        protected virtual void DrawSettings()
        {
            // CUSTOM SETTINGS
            EditorGUILayout.LabelField(_otherSettingsContent, EditorStyles.boldLabel);
            EditorGUILayout.BeginVertical("box");

            OSCEditorLayout.DrawProperties(serializedObject, _addressProperty.name, _receiverProperty.name);

            EditorGUILayout.EndVertical();
        }

        #endregion
    }
}