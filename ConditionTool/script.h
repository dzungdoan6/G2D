/*
	Author: Anh-Dzung Doan
*/

#pragma once

#include "..\dependencies\inc\natives.h"
#include "..\dependencies\inc\types.h"
#include "..\dependencies\inc\enums.h"

#include "..\dependencies\inc\main.h"
#include <fstream>
#include <vector>
#include <string>
#include "keyboard.h"


/*********************************************************************/
/******************************** DEFINITIONS **************************/
/*********************************************************************/
#define VEHICLE_TYPE 1 // traffic type = vehicle
#define PED_TYPE 2 // traffic type = pedestrian

#define INCREASE 1 // increase traffic density
#define DECREASE 2 // decrease traffic density


/*********************************************************************/
/******************************** FUNCTIONS **************************/
/*********************************************************************/

/******************************** Functions for drawing menu ********************************/
void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7);
void drawMenuLine(std::string caption, float line_width, float line_height,
	float line_top, float line_left, float text_left, bool active,
	bool title, bool rescale_text = true);

/******************************** Functions that process menu ********************************/
void handleMainMenu(); // display main menu
void handleTrafficDensityMenu(std::string menu_name); // display menu of traffic density
void handleWeatherMenu(std::string menu_name); // display menu of weather

/******************************** Functions that manipulate conditions ********************************/
void updateTrafficDensity(int CHANGE_TYPE, int TRAFFIC_TYPE); // update traffic density
void teleportToMarker(); // teleport player to the marker on the map

/******************************** Functions that display notifications ********************************/
void setNotificationText(std::string str, DWORD time = 1500, bool isGxtEntry = false); // set notification text
void updateNotificationText(); // set text style, position, font

/******************************** Functions for listening keyboard ********************************/
bool switchPressed(); // listen if users open main menu
void getButtonState(bool *select, bool *back, bool *up, bool *down); // listen keyboard buttons

/******************************** Other functions ****************************************************/
void updateFeatures(); // Update neccessary features within game, should be called in every frames
void ScriptMain(); // Main functions