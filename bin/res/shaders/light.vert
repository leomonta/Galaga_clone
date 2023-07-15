#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec2 lightPos;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec2 light;

// NOTE: Add here your custom variables
const vec2 screenSize = vec2(800, 1000);

void main() {
	// Send vertex attributes to fragment shader
	fragTexCoord = vertexTexCoord;
	fragColor = vertexColor;

	light = vec2(vec4(lightPos, 1.0, 1.0) * mvp);

	// Calculate final vertex position
	gl_Position = mvp * vec4(vertexPosition, 1.0);
}