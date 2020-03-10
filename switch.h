#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
//#include <pcf/io/camera/camera.h>

int gesture(int finger)
{

	static int check=0;
	//HANDLE hProcess=NULL;
	STARTUPINFO info={sizeof(info)};
    PROCESS_INFORMATION processInfo;
    //static IoCamera_t* camera;
if(check!=finger)
{
	switch(finger)
	{
		case 1:
		CreateProcess("C:\\Program Files\\Opera\\launcher.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
		break;
		
		case 2:
		CreateProcess("C:\\Windows\\System32\\mspaint.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
		break;
		
		case 3:
		CreateProcess("C:\\Program Files\\Notepad++\\notepad++.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
		break;
		
		case 4:
		CreateProcess("C:\\Program Files\\Windows Media Player\\wmplayer.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
		break;
		
		default:
		printf("Invalid gesture");
	}

	// if(check==1)
	// {
	// 	  pcf_io_camera_stop(camera);
	// 	  printf("camera stopped successfully");
	// 	  exit(0);
	// }
	WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    Sleep(300);
    
    exit(0);
}
check=finger;
return 0;

}
		
		
