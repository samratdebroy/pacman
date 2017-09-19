#version 330

out vec4 FragColor;

in vec3 Position;
void main()
{
	// Create a grid using the vertex positions
	if((Position.x - floor(Position.x) < 0.05 || Position.z - floor(Position.z) < 0.05 
	|| ceil(Position.z) - Position.z < 0.05 || ceil(Position.x) - Position.x < 0.05) ){
			FragColor = vec4(1.0);
	}else{
		FragColor = vec4(0.1);
	}
}