/* Copyright (c) 2017 ExT (V.Sigalkin) */

using UnityEngine;
using UnityEditor;

using extOSC.Components;

namespace extOSC.Editor.Components
{
    [CustomEditor(typeof(OSCTransmitterComponent), true)]
    public class OSCTransmitterComponentEditor : UnityEditor.Editor
    {
        #region Static Private Vars

        private static readonly GUIContent _transmitterComponentSettingsContent = new GUIContent("Transmitter Component Settings:");

        private static readonly GUIContent _otherSettingsContent = new GUIContent("Other Settings:");

        #endregion

        #region Private Vars

        private SerializedProperty _transmitterProperty;

        private SerializedProperty _addressProperty;

        #endregion

        #region Unity Methods

        protected virtual void OnEnable()
        {
            _transmitterProperty = serializedObject.FindProperty("transmitter");
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
            EditorGUILayout.LabelField(_transmitterComponentSettingsContent, EditorStyles.boldLabel);
            OSCEditorLayout.TransmitterSettings(_transmitterProperty, _addressProperty);

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

            OSCEditorLayout.DrawProperties(serializedObject, _addressProperty.name, _transmitterProperty.name);

            EditorGUILayout.EndVertical();
        }

        #endregion

    }
}