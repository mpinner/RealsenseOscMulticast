/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2014 Intel Corporation. All Rights Reserved.

*******************************************************************************/

using UnityEngine;
using System.Collections;
[ExecuteInEditMode]

public class RescaleGUI : MonoBehaviour {
  
    public float factor = 30;
 
    void OnGUI(){
		try {
	    	guiText.fontSize = (int)(Screen.width/factor);
		} finally {
		}
    }
}
 