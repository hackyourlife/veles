#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "types.h"
#include "shader.h"
#include "mtx.h"

#define	SIZE	256
#define LAYERS	(SIZE * 4)

extern const char main_vert[];
extern const char main_frag[];

static GLuint shader;
static GLuint shader_tex_intensity;
static GLuint shader_tex_pos;
static GLuint shader_depth;
static GLuint shader_gain;
static GLuint shader_power;
static GLuint shader_palette;
static GLuint shader_view;
static GLuint quad_vao;
static GLuint quad_vbo;
static GLuint tex_intensity;
static GLuint tex_position;

static float* positions;
static float* intensities;

static u64 maxfreq = 0;

static u8 buffer[256 * 1024];
static u8 oldbuf[2];
static unsigned int ptr = 0;
static BOOL initialized = FALSE;

static BOOL mouse_l = FALSE;
static BOOL mouse_r = FALSE;
static int mouse_x = 0;
static int mouse_y = 0;
static float rot_x = 0;
static float rot_y = 0;
static int gain = 100;
static int power = 1000;
static BOOL lowgain = FALSE;
static BOOL palette = TRUE;

static Mtx44 rotmtx;

static const float quad_vertices[] = {
	-1.0f, -1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,

	 1.0f,  1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f
};

#define	QUAD_VTX_CNT	(sizeof(quad_vertices) / (sizeof(*quad_vertices) * 3))

#ifdef NDEBUG
#define GL_ERROR()
#else
#define GL_ERROR()      check_error(__FILE__, __LINE__)

void check_error(const char* filename, unsigned int line)
{
	GLenum error = glGetError();
	switch(error) {
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
			printf("%s:%u: Error: GL_INVALID_ENUM\n", filename, line);
			break;
		case GL_INVALID_VALUE:
			printf("%s:%u: Error: GL_INVALID_VALUE\n", filename, line);
			break;
		case GL_INVALID_OPERATION:
			printf("%s:%u: Error: GL_INVALID_OPERATION\n", filename, line);
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			printf("%s:%u: Error: GL_INVALID_FRAMEBUFFER_OPERATION\n", filename, line);
			break;
		case GL_OUT_OF_MEMORY:
			printf("%s:%u: Error: GL_OUT_OF_MEMORY\n", filename, line);
			exit(1);
			break;
		case GL_STACK_UNDERFLOW:
			printf("%s:%u: Error: GL_STACK_UNDERFLOW\n", filename, line);
			break;
		case GL_STACK_OVERFLOW:
			printf("%s:%u: Error: GL_STACK_OVERFLOW\n", filename, line);
			break;
		default:
			printf("%s:%u: Unknown error 0x%X\n", filename, line, error);
	}
}
#endif

static void readvec(FILE* in, u8* vec)
{
	if(!initialized) {
		fread(buffer, sizeof(buffer), 1, in);
		initialized = TRUE;
	}

	unsigned int remaining = sizeof(buffer) - ptr;
	if(remaining >= 3) {
		vec[0] = buffer[ptr + 0];
		vec[1] = buffer[ptr + 1];
		vec[2] = buffer[ptr + 2];
		ptr++;
	} else if(remaining == 2) {
		oldbuf[0] = buffer[sizeof(buffer) - 2];
		oldbuf[1] = buffer[sizeof(buffer) - 1];
		fread(buffer, sizeof(buffer), 1, in);

		vec[0] = oldbuf[0];
		vec[1] = oldbuf[1];
		vec[2] = buffer[0];
		ptr++;
	} else {
		/* remaining == 1 */
		vec[0] = oldbuf[1];
		vec[1] = buffer[0];
		vec[2] = buffer[1];
		ptr = 0;
	}
}

void display_func(void)
{
	GL_ERROR();
	glClear(GL_COLOR_BUFFER_BIT);

	double div = lowgain ? 1000.0 : 100.0;

	glUseProgram(shader);
	glUniform1i(shader_tex_intensity, 0);
	glUniform1i(shader_tex_pos, 1);
	glUniform1f(shader_gain, gain / div);
	glUniform1f(shader_power, power / 1000.0);
	glUniform1i(shader_palette, palette);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, tex_intensity);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, tex_position);

	glUniformMatrix4fv(shader_view, 1, 0, rotmtx.a);

	for(unsigned int i = 0; i <= LAYERS; i++) {
		float z = i / (double) LAYERS;
		glUniform1f(shader_depth, z);

		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);
	}

	GL_ERROR();

	glutSwapBuffers();
}

void kb_func(unsigned char key, int x, int y)
{
	switch(key) {
		case 'a':
		case 'A':
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glutPostRedisplay();
			break;
		case 'b':
		case 'B':
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glutPostRedisplay();
			break;
		case 'g':
		case 'G':
			lowgain = FALSE;
			glutPostRedisplay();
			break;
		case 'l':
		case 'L':
			lowgain = TRUE;
			glutPostRedisplay();
			break;
		case 'c':
		case 'C':
			palette = !palette;
			glutPostRedisplay();
			break;
		case 'p':
		case 'P':
			glBindTexture(GL_TEXTURE_3D, tex_intensity);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glutPostRedisplay();
			break;
		case 's':
		case 'S':
			glBindTexture(GL_TEXTURE_3D, tex_intensity);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glutPostRedisplay();
			break;
		case 'n':
		case 'N':
			glBindTexture(GL_TEXTURE_3D, tex_position);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glutPostRedisplay();
			break;
		case 'f':
		case 'F':
			glBindTexture(GL_TEXTURE_3D, tex_position);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glutPostRedisplay();
			break;
		case 'r':
		case 'R':
			gain = 100;
			power = 1000;
			lowgain = FALSE;
			palette = FALSE;
			MTX44Identity(&rotmtx);
			glutPostRedisplay();
			break;
	}
}

void special_func(int key, int x, int y)
{
	switch(key) {
		case GLUT_KEY_F5:
			exit(0);
			break;
	}
}

void mouse_func(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;

	if(button == GLUT_LEFT_BUTTON) {
		mouse_l = state == GLUT_DOWN;
	} else if(button == GLUT_RIGHT_BUTTON) {
		mouse_r = state == GLUT_DOWN;
	}
}

void motion_func(int x, int y)
{
	int dx = x - mouse_x;
	int dy = y - mouse_y;

	if(mouse_l && (dx || dy)) {
		rot_x -= dy / 5.0f;
		while(rot_x >= 360) {
			rot_x -= 360;
		}
		while(rot_x < 0) {
			rot_x += 360;
		}

		rot_y -= dx / 5.0f;
		while(rot_y >= 360) {
			rot_y -= 360;
		}
		while(rot_y < 0) {
			rot_y += 360;
		}

		Mtx44 rotx, roty, rot;
		MTX44RotRad(&rotx, 'x', dy / 5.0 / 180.0 * M_PI);
		MTX44RotRad(&roty, 'y', dx / 5.0 / 180.0 * M_PI);
		MTX44Concat(&roty, &rotx, &rot);
		MTX44Concat(&rotmtx, &rot, &rotmtx);

		glutPostRedisplay();
	}

	if(mouse_r && dy) {
		gain += dy;
		if(gain < 1) {
			gain = 1;
		} else if(gain > 10000) {
			gain = 10000;
		}
		glutPostRedisplay();
	}

	if(mouse_r && dx) {
		power += dx;
		if(power < 1) {
			power = 1;
		} else if(power > 10000) {
			power = 10000;
		}
		glutPostRedisplay();
	}

	mouse_x = x;
	mouse_y = y;
}

static int cmp_u64(const void* a, const void* b)
{
	s64* x = (s64*) a;
	s64* y = (s64*) b;
	return *x - *y;
}

int main(int argc, char** argv)
{
	if(argc != 2) {
		printf("Usage: %s file\n", *argv);
		return 1;
	}

	glutInit(&argc, argv);

	const char* filename = argv[1];

	long start_time = glutGet(GLUT_ELAPSED_TIME);

	u64* frequencies = (u64*) malloc(SIZE * SIZE * SIZE * sizeof(u64));
	u64* upos = (u64*) malloc(SIZE * SIZE * SIZE * sizeof(u64));
	positions = (float*) malloc(SIZE * SIZE * SIZE * sizeof(float));
	intensities = (float*) malloc(SIZE * SIZE * SIZE * sizeof(float));

	memset(frequencies, 0, SIZE * SIZE * SIZE * sizeof(u64));
	memset(upos, 0, SIZE * SIZE * SIZE * sizeof(u64));
	memset(positions, 0, SIZE * SIZE * SIZE * sizeof(float));
	memset(intensities, 0, SIZE * SIZE * SIZE * sizeof(float));

	struct stat statbuf;
	if(stat(filename, &statbuf) == -1) {
		printf("stat failed: %s\n", strerror(errno));
		return 1;
	}

	printf("file size: %lu\n", statbuf.st_size);

	FILE* in = fopen(filename, "rb");
	if(!in) {
		printf("Error opening input file: %s\n", strerror(errno));
		return 1;
	}

	for(size_t pos = 0; pos < statbuf.st_size - 2; pos++) {
		u8 buf[3];

		readvec(in, buf);

		/* double fpos = (double) pos / statbuf.st_size; */
		size_t idx = buf[0] + (buf[1] + buf[2] * SIZE) * SIZE;
		frequencies[idx]++;
		/* positions[idx] += fpos; */
		upos[idx] += pos;

		if(frequencies[idx] > maxfreq) {
			maxfreq = frequencies[idx];
		}
	}

	fclose(in);

	long end_time = glutGet(GLUT_ELAPSED_TIME);

	/* compute median */
	long qsort_start_time = glutGet(GLUT_ELAPSED_TIME);
	u64* tmp = (u64*) malloc(SIZE * SIZE * SIZE * sizeof(u64));
	size_t nonzero = 0;
	for(size_t i = 0; i < SIZE * SIZE * SIZE; i++) {
		u64 freq = frequencies[i];
		if(freq > 1) {
			tmp[nonzero++] = freq;
		}
	}
	qsort(tmp, nonzero, sizeof(u64), cmp_u64);
	double percentile = 0.95;
	u64 med_low = tmp[(size_t) (nonzero * (1.0 - percentile))];
	u64 med_high = tmp[(size_t) (nonzero * percentile)];
	u64 medfreq = (med_high + med_low) / 2;
	long qsort_end_time = glutGet(GLUT_ELAPSED_TIME);

	size_t idx = 0;
	for(unsigned int z = 0; z < SIZE; z++) {
		for(unsigned int y = 0; y < SIZE; y++) {
			for(unsigned int x = 0; x < SIZE; x++) {
				/* size_t idx = x + (y + z * SIZE) * SIZE; */
				/* positions[idx] /= frequencies[idx]; */
				positions[idx] = (double) upos[idx] / (double) (statbuf.st_size * frequencies[idx]);
				intensities[idx] = (double) frequencies[idx] / (medfreq * 64);
				idx++;
			}
		}
	}

	long dt = end_time - start_time;
	printf("Load time: %ld ms\n", dt);

	long qsort_dt = qsort_end_time - qsort_start_time;
	printf("Median time: %ld ms\n", qsort_dt);

	printf("Max: %lu, median: %lu\n", maxfreq, medfreq);

	free(frequencies);
	free(upos);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowSize(6 * SIZE, 6 * SIZE);
	glutCreateWindow("VELES");

	glutIgnoreKeyRepeat(1);

	glutDisplayFunc(display_func);
	glutKeyboardFunc(kb_func);
	glutSpecialFunc(special_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);

	const unsigned char* gl_vendor = glGetString(GL_VENDOR);
	const unsigned char* gl_renderer = glGetString(GL_RENDERER);
	const unsigned char* gl_version = glGetString(GL_VERSION);
	const unsigned char* gl_glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("GL Vendor:    %s\n", gl_vendor);
	printf("GL Renderer:  %s\n", gl_renderer);
	printf("GL Version:   %s\n", gl_version);
	printf("GLSL Version: %s\n", gl_glsl_version);

	printf("using depth buffer with %d bit\n", glutGet(GLUT_WINDOW_DEPTH_SIZE));

	/* create shader */
	shader = GXCreateShader(main_vert, main_frag);
	shader_tex_intensity = glGetUniformLocation(shader, "tex_intensity");
	shader_tex_pos = glGetUniformLocation(shader, "tex_pos");
	shader_depth = glGetUniformLocation(shader, "depth");
	shader_gain = glGetUniformLocation(shader, "gain");
	shader_power = glGetUniformLocation(shader, "power");
	shader_palette = glGetUniformLocation(shader, "palette");
	shader_view = glGetUniformLocation(shader, "view");

	/* create VBO */
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	GLuint loc = 0;

	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc , 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* create 3D textures */
	glGenTextures(1, &tex_intensity);
	glBindTexture(GL_TEXTURE_3D, tex_intensity);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, SIZE, SIZE, SIZE, 0, GL_RED, GL_FLOAT, intensities);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	/* use NEAREST interpolation for position, otherwise it causes glitches */
	glGenTextures(1, &tex_position);
	glBindTexture(GL_TEXTURE_3D, tex_position);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, SIZE, SIZE, SIZE, 0, GL_RED, GL_FLOAT, positions);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GL_ERROR();

	glEnable(GL_BLEND);
	/* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	MTX44Identity(&rotmtx);

	glutMainLoop();

	return 0;
}
