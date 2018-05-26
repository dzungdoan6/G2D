#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Vertex
{
	float x, y, z;
	std::vector<uint32_t> order;
};

// read vertex list
bool readVertexList(std::string vertex_file_text);

// parse each line within order file
void parseOrderLine(std::vector<uint32_t> &v, std::string str_line);

// read vertex order list
bool readVertexOrder(std::string vertex_order_file_text);

// display to see the vertex (including its order) that we read
void displayVertexInfo();

// create sparse trajectory
bool createTrajectory(std::string trajectory_file_text);