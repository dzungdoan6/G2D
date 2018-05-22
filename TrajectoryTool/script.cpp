/*
	Author: Anh-Dzung Doan
*/

#include "script.h"
#include "keyboard.h"

/*********************************************************************/
/******************************** VARIABLES **************************/
/*********************************************************************/

// notification text
std::string _notification_text;
DWORD _notification_text_draw_ticks_max;
bool _notification_text_gxt_entry;

// Variables for processing menu
float _MENU_LINE_WIDTH = 350; // menu width size
int _current_main_menu_index = 0; // current selection index for main menu
int _current_create_trajectory_menu_index = 0; // current selection index for menu of trajectory creator
int _current_execute_trajectory_menu_index = 0; // current selection index for menu of trajectory executor

// threshold for trajectory executor
float _DISTANCE_THRESHOLD = 1.5;
float _DISTANCE_THRESHOLD_SMALL = 0.5;

// flags for trajectory executor
bool _DO_FOLLOW_TRAJECTORY = false;
bool _DO_CREATE_DENSE_TRAJECTORY = false;
bool _DO_FOLLOW_DENSE_TRAJECTORY = false;

int _traj_idx = 0; // trajectory index used while executing trajectory
Cam _camera; // Our owned camera used for executing dense trajectory and collecting image data
int _order_rot = 2; // rotation order

// The output file names
std::string _sparse_trajectory_file_text = "trajectory_sparse.txt";
std::string _vertex_file_text = "vertex.txt";
std::string _dense_trajectory_file_text = "trajectory_dense.txt";
std::string _dataset_dir = "dataset";
std::string _dataset_image_dir = _dataset_dir + "/" + "images";
std::string _6dpose_im_file_text = "6dpose_list.txt";

std::ofstream _ofile; // stream to write output files

std::vector<Point> _trajectory; // trajectory vector used to store the trajectory (including sparse and dense trajectory)

GDIScreenCaptureWorker _screen_capture_worker; // variable for screen capture


/****************************************************************************************/
/******************************** IMPLEMENTATIONS OF FUNCTIONS **************************/
/****************************************************************************************/
void setNotificationText(std::string str, DWORD time /*= 1500*/, bool isGxtEntry /*= false*/)
{
	_notification_text = str;
	_notification_text_draw_ticks_max = GetTickCount() + time;
	_notification_text_gxt_entry = isGxtEntry;
}

void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

void drawMenuLine(std::string caption, float line_width, float line_height, float line_top,
	float line_left, float text_left, bool active, bool title, bool rescale_text /*= true*/)
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

void GDIInitScreenCapture()
{
	_screen_capture_worker.nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	_screen_capture_worker.nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	_screen_capture_worker.hDesktopWnd = GetDesktopWindow();
	_screen_capture_worker.hDesktopDC = GetDC(_screen_capture_worker.hDesktopWnd);
	_screen_capture_worker.hCaptureDC = CreateCompatibleDC(_screen_capture_worker.hDesktopDC);

	_screen_capture_worker.hCaptureBitmap = CreateCompatibleBitmap(_screen_capture_worker.hDesktopDC,
		_screen_capture_worker.nScreenWidth, _screen_capture_worker.nScreenHeight);

	SelectObject(_screen_capture_worker.hCaptureDC, _screen_capture_worker.hCaptureBitmap);
}

void GDIReleaseScreenCapture()
{
	ReleaseDC(_screen_capture_worker.hDesktopWnd, _screen_capture_worker.hDesktopDC);
	DeleteDC(_screen_capture_worker.hCaptureDC);
	DeleteObject(_screen_capture_worker.hCaptureBitmap);
}

bool GDITakeScreenshots(std::string file_name)
{
	BitBlt(_screen_capture_worker.hCaptureDC, 0, 0, 
		_screen_capture_worker.nScreenWidth, _screen_capture_worker.nScreenHeight,
		_screen_capture_worker.hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);
	CImage image;
	image.Attach(_screen_capture_worker.hCaptureBitmap);
	image.Save(file_name.c_str(), Gdiplus::ImageFormatJPEG);

	return true;
}

bool switchPressed()
{
	return IsKeyJustUp(VK_F5);
}

void getButtonState(bool *select, bool *back, bool *up, bool *down)
{
	if (select) *select = IsKeyDown(VK_NUMPAD5);
	if (back) *back = IsKeyDown(VK_NUMPAD0) || switchPressed() || IsKeyDown(VK_BACK);
	if (up) *up = IsKeyDown(VK_NUMPAD8);
	if (down) *down = IsKeyDown(VK_NUMPAD2);
}

void addVertex()
{

	Vector3 coord;
	bool success = getCoordsFromMarker(coord); // get XYZ location from the marker on the built-in map
	if (!success)
	{
		setNotificationText("Please put the marker on the map");
		return;
	}

	std::ofstream ofile;

	// if vertex file does not exist, create a new one
	if (!isFileExist(_vertex_file_text))
	{
		ofile.open(_vertex_file_text.c_str(), std::ios_base::out);
	}

	// if vertex file exists, append the content
	else
	{
		ofile.open(_vertex_file_text.c_str(), std::ios_base::app);
	}

	ofile << coord.x << " " << coord.y << " " << coord.z << std::endl;
	setNotificationText("Add vertex: " + std::to_string(coord.x) + "," + std::to_string(coord.y) + "," + std::to_string(coord.z));
	ofile.close();
}

void handleCreateTrajectoryMenu(std::string menu_name)
{
	const int menu_item_number = 2;
	std::string menu_list[menu_item_number] = { "ADD VERTEX", "..." };

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
				if (i != _current_create_trajectory_menu_index)
					drawMenuLine(menu_list[i], _MENU_LINE_WIDTH, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			drawMenuLine(menu_list[_current_create_trajectory_menu_index],
				_MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_create_trajectory_menu_index * 36.0, 0.0, 7.0, true, false);
			updateFeatures();
			WAIT(0);
		} while (GetTickCount() < max_tick_count);

		wait_time = 0;

		// listen if users press any buttons
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // if select button is pressed
		{
			switch (_current_create_trajectory_menu_index)
			{
			case 0:
				addVertex();
				break;
			case 1:
				setNotificationText("Need your help for more convenient ways to create trajectory");
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

					if (_current_create_trajectory_menu_index == 0)
						_current_create_trajectory_menu_index = menu_item_number;
					_current_create_trajectory_menu_index--;
					wait_time = 150;
				}
				else
					if (button_down)
					{
						_current_create_trajectory_menu_index++;
						if (_current_create_trajectory_menu_index == menu_item_number)
							_current_create_trajectory_menu_index = 0;
						wait_time = 150;
					}
	}

}

bool readFirstPointInTrajectory()
{
	// if unable to load sparse trajectory, return false
	if (!isFileExist(_sparse_trajectory_file_text))
	{
		return false;
	}

	// only read the first point
	std::ifstream file(_sparse_trajectory_file_text.c_str());
	_trajectory.clear();
	Point p;
	file >> p.player_coord.x;
	file >> p.player_coord.y;
	file >> p.player_coord.z;

	_trajectory.push_back(p);
	return true;
}
bool readSparseTrajectory()
{
	// if unable to load sparse trajectory, return false
	if (!isFileExist(_sparse_trajectory_file_text))
	{
		return false;
	}

	// read all points within sparse trajectory
	std::ifstream file(_sparse_trajectory_file_text.c_str());
	_trajectory.clear();
	while (!file.eof())
	{
		Point p;
		file >> p.player_coord.x;
		file >> p.player_coord.y;
		file >> p.player_coord.z;

		_trajectory.push_back(p);
	}
	_trajectory.pop_back(); // last and second last elements are the same

	if (_trajectory.size() < 1)
		return false;

	return true; 
}

bool readDenseTrajectory()
{
	// if unable to load dense trajectory, return false
	if (!isFileExist(_dense_trajectory_file_text))
	{
		return false;
	}

	// read all points in dense trajectory
	std::ifstream file(_dense_trajectory_file_text.c_str());
	_trajectory.clear();
	while (!file.eof())
	{
		Point p;
		int temp;
		
		// read player location
		file >> p.player_coord.x;
		file >> p.player_coord.y;
		file >> p.player_coord.z;

		// read 6d pose of camera
		file >> p.cam_coord.x;
		file >> p.cam_coord.y;
		file >> p.cam_coord.z;

		file >> p.cam_rot.x;
		file >> p.cam_rot.y;
		file >> p.cam_rot.z;

		file >> temp;
		file >> temp;
		_trajectory.push_back(p);
	}
	_trajectory.pop_back(); // last and second last elements are the same

	if (_trajectory.size() < 1)
		return false;

	return true;
}

void moveToStartingPoint()
{
	if (readFirstPointInTrajectory() == false)
	{
		setNotificationText("Unable to load sparse trajectory");
		return;
	}

	// if we do not know Z, we should find Z
	if (_trajectory[0].player_coord.z == 1)
	{
		Vector3 coords;
		coords.x = _trajectory[0].player_coord.x;
		coords.y = _trajectory[0].player_coord.y;
		coords.z = 1;
		bool groundFound = false;
		Entity e = PLAYER::PLAYER_PED_ID();
		static float groundCheckHeight[] = {
			100.0, 150.0, 50.0, 0.0, 200.0, 250.0, 300.0, 350.0, 400.0,
			450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
		};
		for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
		{
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, groundCheckHeight[i], 0, 0, 1);
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
			setNotificationText("Moved player to starting point with a parachute, it is not good, should find for other initial coordinate");
		}
		else
		{
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, coords.z, 0, 0, 1);
			WAIT(0);
			setNotificationText("Moved to starting point");
		}

	}
	else
	{
		Entity e = PLAYER::PLAYER_PED_ID();

		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, _trajectory[0].player_coord.x, _trajectory[0].player_coord.y, _trajectory[0].player_coord.z, 0, 0, 1);

		WAIT(0);
		setNotificationText("Moved to starting point");
	}
	
}

void createCamera()
{
	_camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1); // create our own camera
	CAM::SET_CAM_FOV(_camera, CAM::GET_GAMEPLAY_CAM_FOV()); // set its fov
}

void updateCamera(float coord_x, float coord_y, float coord_z
	, float rot_x, float rot_y, float rot_z)
{
	CAM::SET_CAM_COORD(_camera, coord_x, coord_y, coord_z); // update location of our own camera
	CAM::SET_CAM_ROT(_camera, rot_x, rot_y, rot_z, _order_rot); // update rotation of our own camera
}

void activateCamera()
{
	CAM::RENDER_SCRIPT_CAMS(true, 1, 1, 1, 0); // set our own camera rendering
}

void backToGameplayCamera()
{
	CAM::RENDER_SCRIPT_CAMS(false, 1, 1, 1, 0); // set gameplay camera rendering
}

bool readyExecuteSparseTrajectory()
{
	if (readSparseTrajectory() == false)
	{
		setNotificationText("Unable to load sparse trajectory");
		return false;
	}

	// get player location
	Entity player_ped = PLAYER::PLAYER_PED_ID();
	Vector3 player_coord = ENTITY::GET_ENTITY_COORDS(player_ped, true);

	bool traj_idx = 0;

	// check if player location is too far from the first point of sparse trajectory
	float dist = computeDistanceXY(player_coord, _trajectory[traj_idx].player_coord);
	if (dist > _DISTANCE_THRESHOLD_SMALL)
	{
		setNotificationText("Please move to starting point first");
		
		return false;
	}
	
	return true;
}

bool readyExecuteDenseTrajectory()
{
	if (readDenseTrajectory() == false)
	{
		setNotificationText("Unable to load dense trajectory");
		return false;
	}
	return true;
}

void executeSparseTrajectory()
{
	// We get player location to store in dense trajectory
	Entity player_ped = PLAYER::PLAYER_PED_ID();
	Vector3 player_coord = ENTITY::GET_ENTITY_COORDS(player_ped, true);
	
	// We get 6D pose of gameplay camera to store in dense trajectory
	Vector3 gameplay_cam_coord = CAM::GET_GAMEPLAY_CAM_COORD();
	Vector3 gameplay_cam_rot = CAM::GET_GAMEPLAY_CAM_ROT(_order_rot);

	// If trajectory is too small
	if (_trajectory.size() <= 1)
	{
		setNotificationText("Please add more one point in trajectory");
		resetExecuteTrajectory();
		return;
	}
	
	// if player reaches to the destination point, move to next point
	float dist = computeDistanceXY(player_coord, _trajectory[_traj_idx].player_coord);
	if (dist < _DISTANCE_THRESHOLD)
	{
		_traj_idx++;
		AI::TASK_GO_STRAIGHT_TO_COORD(player_ped, _trajectory[_traj_idx].player_coord.x, _trajectory[_traj_idx].player_coord.y, _trajectory[_traj_idx].player_coord.z,
			2, 60000, 1, 0);
		
		setNotificationText("Go to point " + std::to_string(_traj_idx+1) +
			": (x,y) = " + "(" + std::to_string(_trajectory[_traj_idx].player_coord.x) + "," +
			std::to_string(_trajectory[_traj_idx].player_coord.y) + ")");
	}

	// Save dense trajectory 
	if (!_ofile.is_open()) // if dense trajectory file has not been opened, open it
		_ofile.open(_dense_trajectory_file_text.c_str());

	_ofile << player_coord.x << " " << player_coord.y << " " << player_coord.z << " ";
	_ofile << gameplay_cam_coord.x << " " << gameplay_cam_coord.y << " " << gameplay_cam_coord.z << " ";
	_ofile << gameplay_cam_rot.x << " " << gameplay_cam_rot.y << " " << gameplay_cam_rot.z << " ";
	_ofile << _traj_idx << " " << _traj_idx + 1 << std::endl;
	
	// if we reach to last point of trajectory, stop function
	if (_traj_idx == _trajectory.size() - 1)
	{
		resetExecuteTrajectory(); // reset all parameters

		if (_ofile.is_open())
			_ofile.close();
		
		return;
	}
}

void executeDenseTrajectory()
{
	if (_trajectory.size() <= 1)
	{
		setNotificationText("Please add more one point in trajectory");
		resetExecuteTrajectory();
		return;
	}

	// if player reaches to the final point of dense trajectory, we set gameplay camera rendering
	if (_traj_idx == _trajectory.size())
	{
		resetExecuteTrajectory();

		if (CAM::IS_CAM_RENDERING(_camera) == TRUE)
			backToGameplayCamera();

		if (_ofile.is_open())
			_ofile.close();
		return;
	}
	else
		// if first point of dense trajectory is the point destination
		// we teleport the player to that first point
		// because it needs time for game to render all game objects, we let our program wait a little bit
		if (_traj_idx == 0) 
		{
			// teleport the player
			Entity e = PLAYER::PLAYER_PED_ID();
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, _trajectory[0].player_coord.x, _trajectory[0].player_coord.y, _trajectory[0].player_coord.z, 0, 0, 1);
		
			WAIT(1000);

			// set 6D pose for our own camera
			updateCamera(_trajectory[_traj_idx].cam_coord.x, _trajectory[_traj_idx].cam_coord.y, _trajectory[_traj_idx].cam_coord.z,
				_trajectory[_traj_idx].cam_rot.x, _trajectory[_traj_idx].cam_rot.y, _trajectory[_traj_idx].cam_rot.z);
		
			_ofile.open(_dataset_dir + "/" + _6dpose_im_file_text);
			
			// if our own camera is not the rendering camera, we set it rendering
			if (CAM::IS_CAM_RENDERING(_camera) == false)
				activateCamera();
			WAIT(1000); // wait for the game to update its objects

			std::string im_name = _dataset_image_dir + "/" + std::to_string(_traj_idx) + ".jpg";

			GDITakeScreenshots(im_name); // capture screen

			// store 6D pose
			_ofile << im_name << " " << std::to_string(_trajectory[_traj_idx].cam_coord.x) << " " <<
				std::to_string(_trajectory[_traj_idx].cam_coord.y) << " " << std::to_string(_trajectory[_traj_idx].cam_coord.z) << " " <<
				std::to_string(_trajectory[_traj_idx].cam_rot.x) << " " << std::to_string(_trajectory[_traj_idx].cam_rot.y) << " " <<
				std::to_string(_trajectory[_traj_idx].cam_rot.z) << std::endl;
			
			WAIT(100);
		}
		else
		{
			// set 6D pose for our own camera
			updateCamera(_trajectory[_traj_idx].cam_coord.x, _trajectory[_traj_idx].cam_coord.y, _trajectory[_traj_idx].cam_coord.z,
				_trajectory[_traj_idx].cam_rot.x, _trajectory[_traj_idx].cam_rot.y, _trajectory[_traj_idx].cam_rot.z);

			WAIT(100);

			std::string im_name = _dataset_image_dir + "/" + std::to_string(_traj_idx) + ".jpg";
			
			// capture screen
			GDITakeScreenshots(im_name); 

			// store 6D pose
			_ofile << im_name << " " << std::to_string(_trajectory[_traj_idx].cam_coord.x) << " " <<
				std::to_string(_trajectory[_traj_idx].cam_coord.y) << " " << std::to_string(_trajectory[_traj_idx].cam_coord.z) << " " <<
				std::to_string(_trajectory[_traj_idx].cam_rot.x) << " " << std::to_string(_trajectory[_traj_idx].cam_rot.y) << " " <<
				std::to_string(_trajectory[_traj_idx].cam_rot.z) << std::endl;

			WAIT(100);
		}

	_traj_idx++;
}

void resetExecuteTrajectory()
{
	_traj_idx = 0;
	_DO_FOLLOW_TRAJECTORY = false;
	_DO_CREATE_DENSE_TRAJECTORY = false;

	if (_ofile.is_open())
		_ofile.close();

	if (_DO_FOLLOW_DENSE_TRAJECTORY)
	{
		GDIReleaseScreenCapture();

		_DO_FOLLOW_DENSE_TRAJECTORY = false;
	}
}

void setParametersOfExecuteSparseTrajectory(bool create_dense_trajectory)
{
	_traj_idx = 0;
	_DO_FOLLOW_TRAJECTORY = true;
	if (create_dense_trajectory)
		_DO_CREATE_DENSE_TRAJECTORY = true;
	_DO_FOLLOW_DENSE_TRAJECTORY = false;
}

void setParametersOfExecuteDenseTrajectory()
{
	_traj_idx = 0;
	_DO_FOLLOW_TRAJECTORY = false;
	_DO_CREATE_DENSE_TRAJECTORY = false;
	_DO_FOLLOW_DENSE_TRAJECTORY = true;

	GDIInitScreenCapture(); // initialize screen capture object
			
}
float computeDistanceXY(Vector3 a, Vector3 b)
{
	return sqrt( (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) );
}

void handleExecuteTrajectoryMenu(std::string menu_name)
{
	const int menu_item_number = 3;
	
	std::string menu_list[menu_item_number] = { "MOVE TO STARTING POINT", "EXECUTE SPARSE TRAJECTORY", 
												"EXECUTE DENSE TRAJECTORY" };

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
				if (i != _current_execute_trajectory_menu_index)
					drawMenuLine(menu_list[i], _MENU_LINE_WIDTH, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			drawMenuLine(menu_list[_current_execute_trajectory_menu_index],
				_MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_execute_trajectory_menu_index * 36.0, 0.0, 7.0, true, false);
			updateFeatures();
			WAIT(0);
		} while (GetTickCount() < max_tick_count);

		wait_time = 0;

		// listen if users press any buttons
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // if select button is pressed
		{
			switch (_current_execute_trajectory_menu_index)
			{
			case 0:
				moveToStartingPoint();
				break;
			case 1:
				if (readyExecuteSparseTrajectory())	
				{
					setParametersOfExecuteSparseTrajectory(true);
				}
				break;
			case 2:
				if (readyExecuteDenseTrajectory())
				{
					setParametersOfExecuteDenseTrajectory();
				}
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

					if (_current_execute_trajectory_menu_index == 0)
						_current_execute_trajectory_menu_index = menu_item_number;
					_current_execute_trajectory_menu_index--;
					wait_time = 150;
				}
				else
					if (button_down)
					{

						_current_execute_trajectory_menu_index++;
						if (_current_execute_trajectory_menu_index == menu_item_number)
							_current_execute_trajectory_menu_index = 0;
						wait_time = 150;
					}

		if (_DO_FOLLOW_TRAJECTORY || _DO_FOLLOW_DENSE_TRAJECTORY)
			break;
	}
}


void handleMainMenu()
{
	const int menu_item_number = 2;
	std::string menu_name = "TRAJECTORY TOOL";
	std::string menu_list[menu_item_number] = { "CREATE TRAJECTORY", "EXECUTE TRAJECTORY" };

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
			drawMenuLine(menu_list[_current_main_menu_index],
				_MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_main_menu_index * 36.0, 0.0, 7.0, true, false);
			updateFeatures();
			WAIT(0);
		} while (GetTickCount() < max_tick_count);

		wait_time = 0;

		// listen if users press any buttons
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // if users select one item within menu, check what the item is
		{
			switch (_current_main_menu_index)
			{
			case 0:
				handleCreateTrajectoryMenu(menu_list[0]);
				break;
			case 1:
				handleExecuteTrajectoryMenu(menu_list[1]);
				break;
			}
			wait_time = 200;
		}
		else
			if (button_back || switchPressed()) // if users press back
			{
				break;
			}
			else
				if (button_up) // if users press up/down
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
		if (_DO_FOLLOW_TRAJECTORY || _DO_FOLLOW_DENSE_TRAJECTORY)
			break;
	}
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


int updateFeatures()
{
	updateNotificationText();
	return 0;
}

bool isFileExist(std::string file_text)
{
	std::ifstream ifile((file_text.c_str()));
	return ifile.is_open();
}

bool getCoordsFromMarker(Vector3 &coords)
{
	Entity e = PLAYER::PLAYER_PED_ID();
	bool success = false;
	bool blipFound = false;

	// search for marker blip
	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	{
		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4) // number 4 is the ID of marker on the built-in map
		{
			coords = UI::GET_BLIP_INFO_ID_COORD(i);
			success = true;;
			break;
		}
	}

	return success;
}


void main()
{

	createCamera();
	
	while (true)
	{

		if (switchPressed())
		{
			resetExecuteTrajectory();
			handleMainMenu();
		}

		if (_DO_FOLLOW_TRAJECTORY)
		{
			executeSparseTrajectory();
		}
		else
			if (_DO_FOLLOW_DENSE_TRAJECTORY)
			{
				executeDenseTrajectory();
			}

		updateFeatures();
		WAIT(0);

	}

}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}
