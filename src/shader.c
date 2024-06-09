#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "shader.h"

GLuint GXCompileShader(GLuint type, const char* src)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, 0);
	glCompileShader(shader);

	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE) {
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(!len) {
			printf("Failed to retrieve shader compilation error log\n");
			exit(1);
		}

		char* log = (char*) malloc(len);
		glGetShaderInfoLog(shader, len, &len, log);
		glDeleteShader(shader);

		printf("Failed to compile shader:\n%s\n", log);
		free(log);

		exit(1);
	} else {
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(len) {
			char* log = (char*) malloc(len);
			glGetShaderInfoLog(shader, len, &len, log);
			printf("shader compilation log:\n%s\n", log);
			free(log);
		}
	}

	return shader;
}

GLuint GXCreateShader(const char* vs_src, const char* fs_src)
{
	GLuint vs = GXCompileShader(GL_VERTEX_SHADER, vs_src);
	GLuint fs = GXCompileShader(GL_FRAGMENT_SHADER, fs_src);

	GLuint shader = glCreateProgram();

	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	GLint linked = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &linked);
	if(linked == GL_FALSE) {
		GLint len = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(!len) {
			printf("Faild to retrieve shader linking error log\n");
			exit(1);
		}

		char* log = (char*) malloc(len);
		glGetProgramInfoLog(shader, len, &len, log);

		glDeleteProgram(shader);
		glDeleteShader(vs);
		glDeleteShader(fs);

		printf("Failed to link shader:\n%s\n", log);

		free(log);
		exit(1);
	} else {
		GLint len = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(len) {
			char* log = (char*) malloc(len);
			glGetProgramInfoLog(shader, len, &len, log);
			printf("Shader linking error log:\n%s\n", log);
			free(log);
		}
	}

	glDetachShader(shader, vs);
	glDetachShader(shader, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return shader;
}
