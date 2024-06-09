#version 330

uniform sampler3D tex_intensity;
uniform sampler3D tex_pos;
uniform float gain = 1.0;
uniform float power = 1.0;
uniform bool palette = false;

in  vec3 pos;
out vec4 color;

vec3 get_color_a(float loc)
{
	// vec3 point0 = vec3(1.0, 1.0, 0.0);
	// vec3 point1 = vec3(1.0, 1.0, 1.0);
	// vec3 point2 = vec3(0.0, 0.0, 1.0);

	if(loc < 0.5) {
		return vec3(1.0, 1.0, loc * 2.0);
	} else {
		float i = 1.0 - (loc * 2.0 - 1.0);
		return vec3(i, 1.0, 1.0);
	}
}

vec3 get_color_b(float loc)
{
	// vec3 point0 = vec3(1.0, 0.0, 0.0);
	// vec3 point1 = vec3(1.0, 1.0, 0.0);
	// vec3 point2 = vec3(1.0, 1.0, 1.0);
	// vec3 point3 = vec3(0.0, 1.0, 1.0);
	// vec3 point4 = vec3(0.0, 0.0, 1.0);

	if(loc < 0.25) {
		float i = loc * 4.0;
		return vec3(1.0, i, 0.0);
	} else if(loc < 0.5) {
		float i = loc * 4.0 - 1.0;
		return vec3(1.0, 1.0, i);
	} else if(loc < 0.75) {
		float i = loc * 4.0 - 2.0;
		return vec3(1.0 - i, 1.0, 1.0);
	} else {
		float i = loc * 4.0 - 3.0;
		return vec3(0.0, 1.0 - i, 1.0);
	}
}

void main(void)
{
	vec3 texcoord = pos + vec3(0.5);
	bool render = all(lessThanEqual(texcoord, vec3(1.0))) && all(greaterThanEqual(texcoord, vec3(0.0)));

	float intensity = texture(tex_intensity, texcoord).r;
	float fpos = texture(tex_pos, texcoord).r;
	vec3 fcolor = palette ? get_color_b(fpos) : get_color_a(fpos);

	intensity = clamp(pow(intensity, power) * pow(gain, 1.5), 0.0, 1.0);

	if(render) {
		color = vec4(fcolor, intensity);
	} else {
		color = vec4(0.0);
	}
}
