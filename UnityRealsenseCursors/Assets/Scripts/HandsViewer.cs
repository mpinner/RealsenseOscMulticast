/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2014 Intel Corporation. All Rights Reserved.

*******************************************************************************/

using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;

public class HandsViewer : MonoBehaviour
{
    public GameObject JointPrefab; //Prefab for Joints
    public GameObject TipPrefab; //Prefab for Finger Tips
    public GameObject BonePrefab; //Prafab for Bones
    public GameObject PalmCenterPrefab;//Prefab for Palm Center

    public GUIText myTextLeft;//GuiText for Left hand
    public GUIText myTextRight;//Pointer for Right hand

    private GameObject[][] myJoints; //Array of Joint GameObjects
    private GameObject[][] myBones; //Array of Bone GameObjects

    private PXCMHandData.JointData[][] jointData; //non-smooth joint values
    private PXCMSmoother smoother = null; //Smoothing module instance
    private PXCMSmoother.Smoother3D[][] smoother3D = null; //smooth joint values
    private int weightsNum = 4; //smoothing factor

    private PXCMSenseManager sm = null; //SenseManager Instance
    private pxcmStatus sts; //StatusType Instance
    private PXCMHandModule handAnalyzer; //Hand Module Instance
    private int MaxHands = 2; //Max Hands
    private int MaxJoints = PXCMHandData.NUMBER_OF_JOINTS; //Max Joints

    private Hashtable handList;//keep track of bodyside and hands for GUItext

    // Use this for initialization
    void Start()
    {
        handList = new Hashtable();

        /* Initialize a PXCMSenseManager instance */
        sm = PXCMSenseManager.CreateInstance();
        if (sm == null)
            Debug.LogError("SenseManager Initialization Failed");

        /* Enable hand tracking and retrieve an hand module instance to configure */
        sts = sm.EnableHand();
        handAnalyzer = sm.QueryHand();
        if (sts != pxcmStatus.PXCM_STATUS_NO_ERROR)
            Debug.LogError("PXCSenseManager.EnableHand: " + sts);

        /* Initialize the execution pipeline */
        sts = sm.Init();
        if (sts != pxcmStatus.PXCM_STATUS_NO_ERROR)
            Debug.LogError("PXCSenseManager.Init: " + sts);

        /* Retrieve the the DataSmoothing instance */
        sm.QuerySession().CreateImpl<PXCMSmoother>(out smoother);

        /* Create a 3D Weighted algorithm */
        smoother3D = new PXCMSmoother.Smoother3D[MaxHands][];

        /* Configure a hand - Enable Gestures and Alerts */
        PXCMHandConfiguration hcfg = handAnalyzer.CreateActiveConfiguration();
        if (hcfg != null)
        {
            hcfg.EnableAllGestures();
            hcfg.EnableAlert(PXCMHandData.AlertType.ALERT_HAND_NOT_DETECTED);
            hcfg.ApplyChanges();
            hcfg.Dispose();
        }

        InitializeGameobjects();

    }

    // Update is called once per frame
    void Update()
    {

        /* Make sure SenseManager Instance is valid */
        if (sm == null)
            return;

        /* Wait until any frame data is available */
        if (sm.AcquireFrame(false) != pxcmStatus.PXCM_STATUS_NO_ERROR)
            return;

        /* Retrieve hand tracking Module Instance */
        handAnalyzer = sm.QueryHand();

        if (handAnalyzer != null)
        {
            /* Retrieve hand tracking Data */
            PXCMHandData _handData = handAnalyzer.CreateOutput();
            if (_handData != null)
            {
                _handData.Update();

                /* Retrieve Gesture Data to manipulate GUIText */
                PXCMHandData.GestureData gestureData;
                for (int i = 0; i < _handData.QueryFiredGesturesNumber(); i++)
                    if (_handData.QueryFiredGestureData(i, out gestureData) == pxcmStatus.PXCM_STATUS_NO_ERROR)
                        DisplayGestures(gestureData);


                /* Retrieve Alert Data to manipulate GUIText */
                PXCMHandData.AlertData alertData;
                for (int i = 0; i < _handData.QueryFiredAlertsNumber(); i++)
                    if (_handData.QueryFiredAlertData(i, out alertData) == pxcmStatus.PXCM_STATUS_NO_ERROR)
                        ProcessAlerts(alertData);

                /* Retrieve all joint Data */
                for (int i = 0; i < _handData.QueryNumberOfHands(); i++)
                {
                    PXCMHandData.IHand _iHand;
                    if (_handData.QueryHandData(PXCMHandData.AccessOrderType.ACCESS_ORDER_FIXED, i, out _iHand) == pxcmStatus.PXCM_STATUS_NO_ERROR)
                    {
                        for (int j = 0; j < MaxJoints; j++)
                        {
                            if (_iHand.QueryTrackedJoint((PXCMHandData.JointType)j, out jointData[i][j]) != pxcmStatus.PXCM_STATUS_NO_ERROR)
                                jointData[i][j] = null;
                        }
                        if (!handList.ContainsKey(_iHand.QueryUniqueId()))
                            handList.Add(_iHand.QueryUniqueId(), _iHand.QueryBodySide());
                    }
                }

                /* Smoothen and Display the Data - Joints and Bones*/
                DisplayJoints();
            }
            handAnalyzer.Dispose();
        }


        sm.ReleaseFrame();

        RotateCam();

    }

    //Close any ongoing Session
    void OnDisable()
    {
        if (smoother3D != null)
        {
            for (int i = 0; i < MaxHands; i++)
            {
                if (smoother3D[i] != null)
                {
                    for (int j = 0; j < MaxJoints; j++)
                    {
                        smoother3D[i][j].Dispose();
                        smoother3D[i][j] = null;
                    }
                }
            }
            smoother3D = null;
        }

        if (smoother != null)
        {
            smoother.Dispose();
            smoother = null;
        }

        if (sm != null)
        {
            sm.Close();
            sm.Dispose();
            sm = null;
        }
    }

    //Smoothen and Display the Joint Data
    void DisplayJoints()
    {

        for (int i = 0; i < MaxHands; i++)
        {
            for (int j = 0; j < MaxJoints; j++)
            {
                if (jointData[i][j] != null && jointData[i][j].confidence == 100)
                {
                    PXCMPoint3DF32 smoothedPoint = smoother3D[i][j].SmoothValue(jointData[i][j].positionWorld);
                    myJoints[i][j].SetActive(true);
                    myJoints[i][j].transform.position = new Vector3(-1 * smoothedPoint.x, smoothedPoint.y, smoothedPoint.z) * 100f;
                    jointData[i][j] = null;
                }
                else
                {
                    myJoints[i][j].SetActive(false);
                }
            }
        }

        for (int i = 0; i < MaxHands; i++)
            for (int j = 0; j < MaxJoints; j++)
            {

                if (j != 21 && j != 0 && j != 1 && j != 5 && j != 9 && j != 13 && j != 17)
                    UpdateBoneTransform(myBones[i][j], myJoints[i][j], myJoints[i][j + 1]);

                UpdateBoneTransform(myBones[i][21], myJoints[i][0], myJoints[i][2]);
                UpdateBoneTransform(myBones[i][17], myJoints[i][0], myJoints[i][18]);

                UpdateBoneTransform(myBones[i][5], myJoints[i][14], myJoints[i][18]);
                UpdateBoneTransform(myBones[i][9], myJoints[i][10], myJoints[i][14]);
                UpdateBoneTransform(myBones[i][13], myJoints[i][6], myJoints[i][10]);
                UpdateBoneTransform(myBones[i][0], myJoints[i][2], myJoints[i][6]);
            }
    }

    //Update Bones
    void UpdateBoneTransform(GameObject _bone, GameObject _prevJoint, GameObject _nextJoint)
    {

        if (_prevJoint.activeSelf == false || _nextJoint.activeSelf == false)
            _bone.SetActive(false);
        else
        {
            _bone.SetActive(true);

            // Update Position
            _bone.transform.position = ((_nextJoint.transform.position - _prevJoint.transform.position) / 2f) + _prevJoint.transform.position;

            // Update Scale
            _bone.transform.localScale = new Vector3(0.8f, (_nextJoint.transform.position - _prevJoint.transform.position).magnitude - (_prevJoint.transform.position - _nextJoint.transform.position).magnitude / 2f, 0.8f);

            // Update Rotation
            _bone.transform.rotation = Quaternion.FromToRotation(Vector3.up, _nextJoint.transform.position - _prevJoint.transform.position);
        }

    }

    //Key inputs to rotate camera and restart
    void RotateCam()
    {
        Vector3 _RotateAround = new Vector3(1, 1f, 30f);

        if (_RotateAround != Vector3.zero)
        {
            if (Input.GetKey(KeyCode.RightArrow))
                transform.RotateAround(_RotateAround, Vector3.up, 200 * Time.deltaTime);

            if (Input.GetKey(KeyCode.LeftArrow))
                transform.RotateAround(_RotateAround, Vector3.up, -200 * Time.deltaTime);
        }

        /* Restart the Level/Refresh Scene */
        if (Input.GetKeyDown(KeyCode.R))
            SceneManager.LoadScene(0);

        /* Quit the Application */
        if (Input.GetKeyDown(KeyCode.Q))
            Application.Quit();

    }

    //Display Gestures
    void DisplayGestures(PXCMHandData.GestureData gestureData)
    {
        if (handList.ContainsKey(gestureData.handId))
        {
            switch ((PXCMHandData.BodySideType)handList[gestureData.handId])
            {
                case PXCMHandData.BodySideType.BODY_SIDE_LEFT:
                    myTextLeft.text = gestureData.name.ToString();
                    break;
                case PXCMHandData.BodySideType.BODY_SIDE_RIGHT:
                    myTextRight.text = gestureData.name.ToString();
                    break;
            }
        }
    }

    //Process Alerts to keep track of hands for Gesture Display
    void ProcessAlerts(PXCMHandData.AlertData alertData)
    {

        if (handList.ContainsKey(alertData.handId))
        {
            switch ((PXCMHandData.BodySideType)handList[alertData.handId])
            {
                case PXCMHandData.BodySideType.BODY_SIDE_LEFT:
                    myTextLeft.text = "";
                    break;
                case PXCMHandData.BodySideType.BODY_SIDE_RIGHT:
                    myTextRight.text = "";
                    break;
            }
        }

    }

    //Populate bones and joints gameobjects
    void InitializeGameobjects()
    {
        myJoints = new GameObject[MaxHands][];
        myBones = new GameObject[MaxHands][];
        jointData = new PXCMHandData.JointData[MaxHands][];
        for (int i = 0; i < MaxHands; i++)
        {
            myJoints[i] = new GameObject[MaxJoints];
            myBones[i] = new GameObject[MaxJoints];
            smoother3D[i] = new PXCMSmoother.Smoother3D[MaxJoints];
            jointData[i] = new PXCMHandData.JointData[MaxJoints];
        }

        for (int i = 0; i < MaxHands; i++)
            for (int j = 0; j < MaxJoints; j++)
            {

                smoother3D[i][j] = smoother.Create3DWeighted(weightsNum);
                jointData[i][j] = new PXCMHandData.JointData();

                if (j == 1)
                    myJoints[i][j] = (GameObject)Instantiate(PalmCenterPrefab, Vector3.zero, Quaternion.identity);
                else if (j == 21 || j == 17 || j == 13 || j == 9 || j == 5)
                    myJoints[i][j] = (GameObject)Instantiate(TipPrefab, Vector3.zero, Quaternion.identity);
                else
                    myJoints[i][j] = (GameObject)Instantiate(JointPrefab, Vector3.zero, Quaternion.identity);

                if (j != 1)
                    myBones[i][j] = (GameObject)Instantiate(BonePrefab, Vector3.zero, Quaternion.identity);
            }

    }

}
