rem :start
IF "%RS_CAM_ID%"=="" ECHO RS_CAM_ID is NOT defined
cd C:\Users\intel\Desktop\
rem RealsenseOscMulticast.exe %RS_CAM_ID% 239.255.0.85 10085
rem RealsenseOscMulticast.exe %RS_CAM_ID% 239.255.0.85 10085
RealsenseOscMulticast.exe %RS_CAM_ID% 239.255.0.85 10085
rem goto start