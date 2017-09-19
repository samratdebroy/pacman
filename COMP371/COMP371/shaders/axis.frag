#version 330

out vec4 FragColor;

in vec3 Position;
void main()
{

 FragColor = vec4(ceil(Position.x),ceil(Position.y),ceil(Position.z),1.0);
	
}