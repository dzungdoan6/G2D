/*
	Author: Anh-Dzung Doan
*/
#pragma once
#include "..\dependencies\inc\natives.h"
#include "..\dependencies\inc\types.h"
#include "..\dependencies\inc\enums.h"
#include "..\dependencies\inc\main.h"
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#pragma comment(lib, "gdiplus.lib")
#include <atlimage.h>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);goto cleanup;}}
#define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

#define PI 3.14159265359
#define DIRECTX 1
#define GDI 2
#define GTA_MAP 0
#define PROTAGONIST 1


/*********************************************************************/
/******************************** STRUCTS **************************/
/*********************************************************************/

struct Quartenion {
	float x = -1, y = -1, z = -1, w = -1;
};

// an element in trajectory
struct Point {
	Vector3 player_coord;
	Vector3 cam_rot;
	Vector3 cam_coord;
};

// struct stores all necessary variables for GDI method
struct GDIScreenCaptureWorker
{
	int nScreenWidth;
	int nScreenHeight;
	HWND hDesktopWnd;
	HDC hDesktopDC;
	HDC hCaptureDC;
	HBITMAP hCaptureBitmap;

};

/*********************************************************************/
/******************************** FUNCTIONS **************************/
/*********************************************************************/

/******************************** Functions for screen capture ********************************/
void GDIInitScreenCapture(); // initialize screen capture object
void GDIReleaseScreenCapture(); // release screen capture object
bool GDITakeScreenshots(std::string file_name); // capture screen

/******************************** Functions for drawing menu ********************************/
void drawMenuLine(std::string caption, float lineWidth, float lineHeight, float lineTop,
	float lineLeft, float textLeft, bool active, bool title, bool rescaleText = true);
void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7);

/******************************** Functions for listening keyboard ********************************/
bool switchPressed(); // listen if users open main menu
void getButtonState(bool *select, bool *back, bool *up, bool *down); // listen keyboard buttons

/******************************** Functions that process menu ********************************/
void handleMainMenu(); // display main menu
void handleCreateTrajectoryMenu(std::string menu_name); // display menu of trajectory construction
void handleExecuteTrajectoryMenu(std::string menu_name); // display menu of trajectory execution

/******************************** Functions that create trajectory ********************************/
void addVertex(); // add vertex to file
bool getCoordsFromMarker(Vector3 &coords); // get coordinate (x,y,z=1) from the marker on the built-in map

/******************************** Functions that execute trajectory ********************************/
bool readyExecuteSparseTrajectory(); // check if we are ready to execute sparse trajectory
bool readyExecuteDenseTrajectory(); // check if we are ready to execute dense trajectory
void moveToStartingPoint(); // move the player to the first point within the sparse trajectory
void resetExecuteTrajectory(); // reset all parameters relating to functions of execute sparse/dense trajectory
void executeDenseTrajectory(); // simultaneously execute dense trajectory and collect images
void executeSparseTrajectory(); // execute sparse trajectory with/without creating dense trajectory
void setParametersOfExecuteSparseTrajectory(bool create_dense_trajectory); // set parameters for functions of execute sparse trajectory
void setParametersOfExecuteDenseTrajectory(); // set parameters for functions of execute dense trajectory

/******************************** Functions that deal with camera ********************************/
void createCamera(); // create our own camera
void updateCamera(float coord_x, float coord_y, float coord_z
	, float rot_x, float rot_y, float rot_z); // update 6D-pose to our own camera
void activateCamera(); // set our own camera rendering
void backToGameplayCamera(); // set default gameplay camera rendering

/******************************** Functions that display notifications ********************************/
void setNotificationText(std::string str, DWORD time = 1500, bool isGxtEntry = false); // set notification text
void updateNotificationText(); // set text style, position, font

/******************************** Functions to manipulate file stream ********************************/
bool isFileExist(std::string file_text); // check if file exists
bool readSparseTrajectory();
bool readDenseTrajectory();
bool readFirstPointInTrajectory();

/******************************** Other functions ****************************************************/
int updateFeatures(); // Update neccessary features within game, should be called in every frames
float computeDistanceXY(Vector3 a, Vector3 b); // Compute Euclidean distance, only take into account X and Y values
void ScriptMain(); // Main functions