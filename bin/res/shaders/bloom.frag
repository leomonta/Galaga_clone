#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

const vec2  size = vec2(800, 1000); // Framebuffer size
const int   samples = 5;            // Pixels per axis; higher = bigger glow, worse performance
const float quality = 2.0;          // Defines size factor: Lower = smaller glow, better quality
const vec3  colFilter = vec3(0.299, 0.587, 0.114);

void main() {
	vec4 sum = vec4(0);
	vec2 sizeFactor = vec2(1) / size * quality;

	// Texel color fetching from texture sampler
	vec4 source = texture(texture0, fragTexCoord);

	int range = (samples - 1) / 2;

	for (int x = -range; x <= range; x++) {
		for (int y = -range; y <= range; y++) {
			vec4 samplePoint = texture(texture0, fragTexCoord + vec2(x, y) * sizeFactor);
			if (length(samplePoint.rgb - colFilter.rgb) < 0.3) {
				samplePoint.rgb = vec3(0, 0, 0);
			}
			sum += samplePoint;
		}
	}

	// Calculate final fragment color
	finalColor = ((sum / (samples * samples)) + source) * colDiffuse;
}