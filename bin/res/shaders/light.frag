#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

const float lightRadius = 5;
const vec2 screenSize = vec2(800, 1000);

void main() {

	// Texel color fetching from texture sampler
	vec4 texelColor = texture(texture0, fragTexCoord);

	float dist = length(fragPosition.xy - gl_FragCoord.xy);

	float brightness = clamp(1.0 - (dist / lightRadius), 0.5, 1.0);

	// NOTE: Implement here your fragment shader code
	finalColor = texelColor * colDiffuse * brightness;
}
