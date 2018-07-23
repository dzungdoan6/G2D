About
============
G2D is a software that enables capturing videos from Grand Theft Auto V (GTA V), a popular role playing game set in an expansive virtual city. The target users of our software are computer vision researchers who wish to collect hyper-realistic computer-generated imagery of a city from the street level, under controlled 6DOF camera poses and varying environmental conditions (weather, time of day and traffic density). 

[WATCH THE VIDEO HERE TO GET THE IDEA WHAT G2D COULD DO](https://youtu.be/cA7G-4YVBeU)

Description
============

G2D contains three tools
1. G2D-Condition.asi    : the tool enables users to change game's condition, e.g. time, weather and traffic density
2. G2D-Trajectory.asi   : the tool facilitates users to construct sparse/dense trajectory
3. CreateSparseTrajectory.exe : the program creates sparse trajectory, given vertex and visitation order lists.

For more technical detail about the software, please refer to our technical document [HERE](https://arxiv.org/abs/1806.07381). If you find our software or our dataset is useful, please cite:

```
@article{Dzung18G2D,
  title = {{G2D}: from {GTA} to {Data}},
  author = {Anh-Dzung Doan and Abdul Mohsi Jawaid and Thanh-Toan Do and Tat-Jun Chin},
  journal={arXiv preprint arXiv:1806.07381},
  pages = {1--9},
  year = {2018}
}
```

The dataset in our "Sample Application" section could be downloaded [HERE](https://goo.gl/DNzxHx)

Installation
============
+ Our software only works for Windows OS.

+ You can either use the pre-built binaries for Window or manually build the source code
 
+ [Recommended] Invisibility Cloak (https://www.gta5-mods.com/scripts/invisibility-cloak): A mod could make protagonist invisible

+ [Recommended] 100% game save (https://www.gta5-mods.com/misc/100-save-game): To open the whole map of the game

------------------
Pre-built Binaries
------------------

1. Download Scripthook V library (http://www.dev-c.com/gtav/scripthookv/). Copy Scripthook.dll and dinput8.dll to GTA V directory
2. Copy our build/G2D-Trajectory.asi and build/G2D-Condition.asi to GTA V directory
3. Launch the game and enjoy!

------------------
Build from Source
------------------

0. Make sure you have:
    + Grand Theft Auto V purchased on Steam.
    + Microsoft Visual Studio. The software is developed in Microsoft Visual Studio Community 2017, 64-bit Windows 10 Education, but the code should work with older version of those.
    + Scripthook SDK (http://www.dev-c.com/gtav/scripthookv/). Actually the SDK is included already within folder dependencies/
1. Open G2D.sln by Visual Studio.
2. Build the solution.

Usage
============

------------------
How to install and use condition tool
------------------

[CLICK HERE TO WATCH THE TUTORIAL VIDEO](https://youtu.be/TlI6C46e3_0)

1. Press F4 to open Condition Tool
    * Control: Numpad 8 = move up, Numpad 2 = move down, Numpad 5 = select, Numpad 0 / F4 = back
2. Place a marker on the map and try function "Teleport to Marker"
3. Change the traffic density through submenu "Vehicle & Pedestrian Density"
4. Change weather and time through submenu "Weather & Time"

------------------
How to create sparse/dense trajectory
------------------

[CLICK HERE TO WATCH THE TUTORIAL VIDEO](https://youtu.be/Tf9MXrZ2NFM)

1. Within GTA V directory, create folder dataset/images to store the collected dataset.
2. Press F4 to open Condition Tool ==> change weather & time, and make sure the clock is paused
3. Press F5 to open Trajectory Tool
4. Place markers on the built-in map ==> select "Create Trajectory => Add Vertex" to add vertex. 
      * The vertex list (vertex.txt) could be found within directory GTA V.
      * In vertex.txt, line 1, 2, 3, etc represent vertex number I, II, III, etc.
      * The format description for each line: 
      <X> <Y> <Z>
      where, <X> and <Y> are X and Y position within the game coordinate, <Z> is set to 1
5. Manually create vertex order (vertex_order.txt) with the following format:
      * Line 1, 2, 3, etc represent order of vertex number I, II, III, etc.
      * For each line
      <order index>,<order index>,...,<order index N>
      For more detail about how vertex and vertex order create sparse trajectory, please refer to our document.
6. Use CreateSparseTrajectory.exe to create sparse trajectory
      * Command-line usage: CreateSparseTrajectory <vertex list> <vertex order list> <sparse trajectory file>
        <sparse trajectory file> MUST be named trajectory_sparse.txt, if not G2D unable to load sparse trajectory.
      * Make sure trajectory_sparse.txt is put within GTA V directory.
7. Open Trajectory Tool, select "Execute Trajectory => Move to Starting Point" to move the player to the 1st point of sparse trajectory
8. Press "v" to change the view mode of gameplay camera, there are two view modes: third-person and first-person view modes.
9. Select "Execute Trajectory => Execute Sparse Trajectory", 
      * The function makes the player move automatically. While the player moves, the tool extracts the positions of gameplay camera that moves along with the player
      * Regarding the rotations of gameplay camera, users have two options:
          + Third-person view mode: Users could use the mouse to control the viewing direction of gameplay camera
          + Third-person view mode: Gameplay camera is attached to the eyes of player, so its viewing direction is always forward. Users do not have to do anything.
        
        In every rendered frame, the tool extracts the rotations of the gameplay camera.
10. When "Execute Sparse Trajectory" is finished, dense trajectory could be found within GTA V direction. The file name is "trajectory_dense.txt"
11. Select "Execute Trajectory => Execute Dense Trajectory" to collect the dataset. 

    When "Execute Dense Trajectory" is finished, users could find the collected dataset within dataset/images which is created in Step 1.
    
License
============

Our software and data are only for research and education purposes, any commercial uses are strictly prohibited.

As a courtesy towards Take-Two Interactive and Rockstar Games, please purchase Grand Theft Auto V, if you use the software and data.

Support
============

If you have any questions, feel free to contact me via email: dung.doan@adelaide.edu.au

Contributions (bug reports, bug fixes, improvements, etc.) are very welcome and should be submitted in the form of new issues and/or pull requests on GitHub.

Acknowledgement
============

We would like to thank Alexander Blade for creating Scripthook library and all anonymous people who have been making contribution to that library.
