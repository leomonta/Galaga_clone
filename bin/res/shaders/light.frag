#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec2 light;

// Input uniform values
uniform sampler2D texture0; // star atlas
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

const float lightRadius = 6.0;

void main() {

	// Texel color fetching from texture sampler
	vec4 texelColor = texture(texture0, fragTexCoord);

	float dist = length(gl_FragCoord.xy - light);

	float brightness = clamp(1.0 - (dist / lightRadius), 0.0, 1.0);

	vec4 col = brightness * 2 * fragColor;

	// NOTE: Implement here your fragment shader code
	finalColor = colDiffuse * col * texelColor;
}
