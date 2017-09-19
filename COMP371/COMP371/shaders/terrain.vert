#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Function Prototype
float noise(vec3 position);

out vec3 Position;
void main()
{
	Position = aPos;
	gl_Position = projection * view * model * vec4(aPos,1.0);
}

float noise(vec3 position){
	return sin(position.x * position.z);
	//return abs(cos(position.x * position.z + curr_time*5))*3;
}