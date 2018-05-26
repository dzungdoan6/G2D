/*
	Author: Anh-Dzung Doan
*/

#include "script.h"

/*********************************************************************/
/******************************** VARIABLES **************************/
/*********************************************************************/

// Variables for processing menu
float _MENU_LINE_WIDTH = 350; // menu width size
int _current_main_menu_index = 0; // current selection index for main menu
int _current_weather_menu_index = 0; // current selection index for weather menu
int _current_traffic_density_menu_index = 0; // current selection index for traffic density menu

// notification text
std::string _notification_text;
DWORD _notification_text_draw_ticks_max;
bool _notification_text_gxt_entry;

// variables for traffic density
float _VEHICLE_DENSITY = 0.0;	// current vehicle density (from 0 to 1)
float _VEHICLE_DENSITY_CHANGE_STEP = 0.1; // the update amount of vehicle density (increase or decrease)
float _PED_DENSITY = 0.0; // current pedestrian density (from 0 to 1)
float _PED_DENSITY_CHANGE_STEP = 0.1; // the update amount of pedestrian density (increase or decrease)

// variables for function of pausing time
bool _TIME_PAUSED = false; // flag that tells if time is paused

/****************************************************************************************/
/******************************** IMPLEMENTATIONS OF FUNCTIONS **************************/
/****************************************************************************************/

void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

void drawMenuLine(std::string caption, float line_width, float line_height, 
	float line_top, float line_left, float text_left, bool active, 
	bool title, bool rescale_text /*= true*/)
{
	// default values
	int text_col[4] = { 255, 255, 255, 255 },
		rect_col[4] = { 70, 95, 95, 255 };
	float text_scale = 0.35;
	int font = 0;

	// correcting values for active line
	if (active)
	{
		text_col[0] = 0;
		text_col[1] = 0;
		text_col[2] = 0;

		rect_col[0] = 218;
		rect_col[1] = 242;
		rect_col[2] = 216;

		if (rescale_text) text_scale = 0.40;
	}

	if (title)
	{
		rect_col[0] = 0;
		rect_col[1] = 0;
		rect_col[2] = 0;

		if (rescale_text) text_scale = 0.50;
		font = 1;
	}

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	text_left += line_left;

	float line_width_scaled = line_width / (float)screen_w; // line width
	float line_top_scaled = line_top / (float)screen_h; // line top offset
	float text_left_scaled = text_left / (float)screen_w; // text left offset
	float line_height_scaled = line_height / (float)screen_h; // line height

	float line_left_scaled = line_left / (float)screen_w;

	// this is how it's done in original scripts

	// text upper part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	UI::_DRAW_TEXT(text_left_scaled, (((line_top_scaled + 0.00278f) + line_height_scaled) - 0.005f));

	// text lower part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_GXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	int num25 = UI::_0x9040DFB09BE75706(text_left_scaled, (((line_top_scaled + 0.00278f) + line_height_scaled) - 0.005f));

	// rect
	drawRect(line_left_scaled, line_top_scaled + (0.00278f),
		line_width_scaled, ((((float)(num25)* UI::_0xDB88A37483346780(text_scale, 0)) + (line_height_scaled * 2.0f)) + 0.005f),
		rect_col[0], rect_col[1], rect_col[2], rect_col[3]);
}

void updateNotificationText()
{
	if (GetTickCount() < _notification_text_draw_ticks_max)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.55, 0.55);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
		if (_notification_text_gxt_entry)
		{
			UI::_SET_TEXT_ENTRY((char *)_notification_text.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)_notification_text.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.5);
	}
}

void updateFeatures()
{
	updateNotificationText();

	// this setting density action should be called every frame
	PED::SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(_PED_DENSITY); 
	VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(_VEHICLE_DENSITY);
}

bool switchPressed()
{
	return IsKeyJustUp(VK_F4);
}

void getButtonState(bool *select, bool *back, bool *up, bool *down)
{
	if (select) *select = IsKeyDown(VK_NUMPAD5);
	if (back) *back = IsKeyDown(VK_NUMPAD0) || switchPressed() || IsKeyDown(VK_BACK);
	if (up) *up = IsKeyDown(VK_NUMPAD8);
	if (down) *down = IsKeyDown(VK_NUMPAD2);
}

void setNotificationText(std::string str, DWORD time /*= 1500*/, bool isGxtEntry /*= false*/)
{
	_notification_text = str;
	_notification_text_draw_ticks_max = GetTickCount() + time;
	_notification_text_gxt_entry = isGxtEntry;
}

void handleWeatherMenu(std::string menu_name)
{
	const int menu_item_number = 6;

	std::string menu_list[menu_item_number] = { "DAY", "NIGHT", "CLEAR",  "RAIN", "SNOW", "" };

	DWORD wait_time = 150;

	while (true)
	{
		if (_TIME_PAUSED == true)
			menu_list[5] = "RESUME CLOCK";
		else
			menu_list[5] = "PAUSE CLOCK";

		// timed menu draw, used for pause after active line switch
		DWORD max_tick_count = GetTickCount() + wait_time;
		do
		{
			// draw menu
			drawMenuLine(menu_name, _MENU_LINE_WIDTH, 15.0, 18.0, 0.0, 5.0, false, true);
			for (int i = 0; i < menu_item_number; i++)
				if (i != _current_weather_menu_index)
					drawMenuLine(menu_list[i], _MENU_LINE_WIDTH, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			drawMenuLine(menu_list[_current_weather_menu_index], _MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_weather_menu_index * 36.0, 0.0, 7.0, true, false);
			updateFeatures();
			WAIT(0);
		} while (GetTickCount() < max_tick_count);
		wait_time = 0;

		// listen if users press any buttons
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // if select button is pressed
		{
			switch (_current_weather_menu_index)
			{
				// DAY
			case 0:
				TIME::SET_CLOCK_TIME(12, 0, 0); // set time = 12:00
				break;

				// NIGHT
			case 1:
				TIME::SET_CLOCK_TIME(23, 0, 0); // set time = 23:00
				
				break;

				// CLEAR
			case 2:
				GAMEPLAY::CLEAR_OVERRIDE_WEATHER();
				GAMEPLAY::SET_OVERRIDE_WEATHER("CLEAR");
				break;

				// RAIN
			case 3:
				GAMEPLAY::CLEAR_OVERRIDE_WEATHER();
				GAMEPLAY::SET_OVERRIDE_WEATHER("THUNDER"); // set "THUNDER" to make heavy rain
				break;

				// SNOW
			case 4:
				GAMEPLAY::CLEAR_OVERRIDE_WEATHER();
				GAMEPLAY::SET_OVERRIDE_WEATHER("BLIZZARD"); // set "BLIZZARD" to make heavy snow
				break;
			case 5:
				if (_TIME_PAUSED == false)
				{
					_TIME_PAUSED = true;
					setNotificationText("Clock is paused");
				}
				else
				{
					_TIME_PAUSED = false;
					setNotificationText("Clock is resumed");
				}
				TIME::PAUSE_CLOCK(_TIME_PAUSED);

				break;
			}
			wait_time = 200;
		}
		else
			if (button_back || switchPressed()) // if back button is pressed
			{
				break;
			}
			else
				if (button_up) // if up/down button is pressed
				{

					if (_current_weather_menu_index == 0)
						_current_weather_menu_index = menu_item_number;
					_current_weather_menu_index--;
					wait_time = 150;
				}
				else
					if (button_down)
					{

						_current_weather_menu_index++;
						if (_current_weather_menu_index == menu_item_number)
							_current_weather_menu_index = 0;
						wait_time = 150;
					}
	}
}

void updateTrafficDensity(int CHANGE_TYPE, int TRAFFIC_TYPE)
{
	float multiplier;

	if (CHANGE_TYPE == INCREASE) multiplier = 1.0; // if "INCREASE", mean sign = +
	else if (CHANGE_TYPE == DECREASE) multiplier = -1.0; // if "DECREASE", mean sign = -

	switch (TRAFFIC_TYPE)
	{
	case VEHICLE_TYPE:
		_VEHICLE_DENSITY += multiplier * _VEHICLE_DENSITY_CHANGE_STEP; // add sign
		break;
	case PED_TYPE:
		_PED_DENSITY += multiplier * _PED_DENSITY_CHANGE_STEP; // add sign
		break;
	}

	// make density vary from 0 to 1
	if (_VEHICLE_DENSITY > 1.0) _VEHICLE_DENSITY = 1.0;
	else if (_VEHICLE_DENSITY < 0) _VEHICLE_DENSITY = 0.0;

	if (_PED_DENSITY > 1.0) _PED_DENSITY = 1.0;
	else if (_PED_DENSITY < 0) _PED_DENSITY = 0.0;

}

void handleTrafficDensityMenu(std::string menu_name)
{
	const int menu_item_number = 4;

	std::string menu_list[menu_item_number] = { "INCREASE VEHICLE DENSITY", "DECREASE VEHICLE DENSITY",
		"INCREASE PEDESTRIAN DENSITY", "DECREASE PEDESTRIAN DENSITY" };

	DWORD wait_time = 150;
	while (true)
	{
		// timed menu draw, used for pause after active line switch
		DWORD max_tick_count = GetTickCount() + wait_time;
		do
		{
			// draw menu
			drawMenuLine(menu_name, _MENU_LINE_WIDTH, 15.0, 18.0, 0.0, 5.0, false, true);
			for (int i = 0; i < menu_item_number; i++)
				if (i != _current_traffic_density_menu_index)
					drawMenuLine(menu_list[i], _MENU_LINE_WIDTH, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			drawMenuLine(menu_list[_current_traffic_density_menu_index], _MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_traffic_density_menu_index * 36.0, 0.0, 7.0, true, false);
			updateFeatures();
			WAIT(0);
		} while (GetTickCount() < max_tick_count);
		wait_time = 0;

		// listen if users press any buttonss
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // if select button is pressed
		{
			switch (_current_traffic_density_menu_index)
			{
			case 0:
				updateTrafficDensity(INCREASE, VEHICLE_TYPE); // change density
				setNotificationText("Vehicle density = " + std::to_string(_VEHICLE_DENSITY));
				break;
			case 1:
				updateTrafficDensity(DECREASE, VEHICLE_TYPE); // change density
				setNotificationText("Vehicle density = " + std::to_string(_VEHICLE_DENSITY));
				break;
			case 2:
				updateTrafficDensity(INCREASE, PED_TYPE); // change density
				setNotificationText("Pedestrian density = " + std::to_string(_PED_DENSITY));
				break;
			case 3:
				updateTrafficDensity(DECREASE, PED_TYPE); // change density
				setNotificationText("Pedestrian density = " + std::to_string(_PED_DENSITY));
				break;
			}
			wait_time = 200;
		}
		else
			if (button_back || switchPressed()) // if back button is pressed
			{
				break;
			}
			else
				if (button_up) // if up/down button is pressed
				{

					if (_current_traffic_density_menu_index == 0)
						_current_traffic_density_menu_index = menu_item_number;
					_current_traffic_density_menu_index--;
					wait_time = 150;
				}
				else
					if (button_down)
					{

						_current_traffic_density_menu_index++;
						if (_current_traffic_density_menu_index == menu_item_number)
							_current_traffic_density_menu_index = 0;
						wait_time = 150;
					}
	}

}

void teleportToMarker()
{
	// get entity to teleport
	Entity player_ped = PLAYER::PLAYER_PED_ID();

	bool blipFound = false;
	Vector3 coords;
	// search for marker
	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	{
		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4) // because marker id is 4
		{
			coords = UI::GET_BLIP_INFO_ID_COORD(i);
			blipFound = true;
			break;
		}
	}
	if (blipFound) // if marker is set already
	{
		// find Z coordinate
		bool groundFound = false;
		static float groundCheckHeight[] = {
			100.0, 150.0, 50.0, 0.0, 200.0, 250.0, 300.0, 350.0, 400.0,
			450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
		};
		for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
		{
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player_ped, coords.x, coords.y, groundCheckHeight[i], 0, 0, 1);
			WAIT(100);
			if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, groundCheckHeight[i], &coords.z, FALSE))
			{
				groundFound = true;
				coords.z += 3.0;
				break;
			}
		}

		// if ground not found then set Z in air and give player a parachute
		if (!groundFound)
		{
			coords.z = 1000.0;
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 0);
		}

		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player_ped, coords.x, coords.y, coords.z, 0, 0, 1);
		WAIT(0);
		setNotificationText("teleported");
	}
	else
	{
		setNotificationText("map marker isn't set");
	}
}

void handleMainMenu()
{
	const int menu_item_number = 3;
	std::string menu_name = "CONDITION TOOL";
	std::string menu_list[menu_item_number] = { "WEATHER & TIME", "VEHICLE & PEDESTRIAN DENSITY", "TELEPORT TO MARKER"};

	DWORD wait_time = 150;
	while (true)
	{
		
		// timed menu draw, used for pause after active line switch
		DWORD max_tick_count = GetTickCount() + wait_time;
		do
		{
			// draw menu
			drawMenuLine(menu_name, _MENU_LINE_WIDTH, 15.0, 18.0, 0.0, 5.0, false, true);
			for (int i = 0; i < menu_item_number; i++)
				if (i != _current_main_menu_index)
					drawMenuLine(menu_list[i], _MENU_LINE_WIDTH, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			drawMenuLine(menu_list[_current_main_menu_index], _MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_main_menu_index * 36.0, 0.0, 7.0, true, false);
			updateFeatures();
			WAIT(0);
		} while (GetTickCount() < max_tick_count);
		wait_time = 0;

		// listen if users press any buttons
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // select button is pressed
		{
			switch (_current_main_menu_index)
			{
			case 0:
				handleWeatherMenu(menu_list[0]); // display weather menu
				break;
			case 1:
				handleTrafficDensityMenu(menu_list[1]); // display traffic density menu
				break;
			case 2:
				teleportToMarker();
				break;
			}
			wait_time = 200;
		}
		else
			if (button_back || switchPressed()) // back button is pressed
			{
				break;
			}
			else
				if (button_up) // up/down button is pressed
				{

					if (_current_main_menu_index == 0)
						_current_main_menu_index = menu_item_number;
					_current_main_menu_index--;
					wait_time = 150;
				}
				else
					if (button_down)
					{

						_current_main_menu_index++;
						if (_current_main_menu_index == menu_item_number)
							_current_main_menu_index = 0;
						wait_time = 150;
					}
	}
}

void main()
{	
	std::ifstream fid;
	
	while (true)
	{
		
		if (switchPressed())
		{
			handleMainMenu();
		}

		updateFeatures();
		
		WAIT(0);
	}
}

void ScriptMain()
{
	main();
}
