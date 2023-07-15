#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform vec2 lightPos;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec2 light;

// NOTE: Add here your custom variables
const vec2 screenSize = vec2(800, 1000);

void main() {
	// Send vertex attributes to fragment shader
	fragPosition = vec3(mvp * vec4(vertexPosition, 1.0));
	fragTexCoord = vertexTexCoord;
	fragColor = vertexColor;

	light = lightPos / screenSize.xy;

	// Calculate final vertex position
	gl_Position = mvp * vec4(vertexPosition, 1.0);
}