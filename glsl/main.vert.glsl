#version 330

layout(location = 0) in vec3 position;

uniform float depth = 1.0;
uniform mat4 view;

out vec3 pos;

void main(void)
{
	gl_Position = vec4(position.xyz, 1.0);

	vec2 screen = (position.xy + vec2(1.0, 1.0)) / 2.0;

	vec3 texcoords = vec3(position.xy, depth * 2.0 - 1.0);
	vec4 point = view * vec4(texcoords, 1.0);

	pos = point.xyz / point.w;
}
