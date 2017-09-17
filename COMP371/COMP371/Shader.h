#pragma once
#include <glew.h>
#include <string>
#include <fstream>
#include <iostream>

class Shader
{
public:
	// The program ID
	unsigned int ID;

	Shader(const std::string vertex_shader_path, const std::string fragment_shader_path);
	void UseProgram();
};

