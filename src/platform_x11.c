#include "platform_x11.h"
#include <stdlib.h>
#include <stdio.h>

void viewport_size_callback(
		const unsigned int width,
		const unsigned int height) {

	glViewport(0, 0, width, height);
}

x_data_t *x_start(const char *window_title,
		unsigned int window_width,
		unsigned int window_height) {

	x_data_t *x = malloc(sizeof(*x));
	x->viewport_size_callback = viewport_size_callback;

	x->display = XOpenDisplay(NULL);

	if(x->display == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(0);
	}
	
	x->screen	= DefaultScreen(x->display);
	x->root		= RootWindow(x->display, x->screen);
	x->visual	= DefaultVisual(x->display, x->screen);

	x->color_map = XCreateColormap(
			x->display,
			x->root,
			x->visual,
			AllocNone);
	
	x->attributes.event_mask =
		ExposureMask |
		KeyPressMask |
		KeyReleaseMask;

	x->attributes.colormap = x->color_map;
	
	x->window = XCreateWindow(
			x->display,
			x->root,
			0, 0, window_width, window_height, 0,
			DefaultDepth(x->display, x->screen),
			InputOutput,
			x->visual,
			CWColormap | CWEventMask, &x->attributes);

	XMapWindow(x->display, x->window);
	XStoreName(x->display, x->window, window_title);
	if (!x->window) {
		fprintf(stderr, "\n\tfailed to create X window\n");
		exit(0);
	}

	int glx_version = gladLoaderLoadGLX(x->display, x->screen);
	if (!glx_version) {
		fprintf(stderr, "failed to load GLX");
		exit(0);
	}

	printf("Loaded GLX %d.%d\n", GLAD_VERSION_MAJOR(glx_version), GLAD_VERSION_MINOR(glx_version));

	GLint visual_attributes[] = {
		GLX_RENDER_TYPE,	GLX_RGBA_BIT,
		GLX_DEPTH_SIZE,		24,
		GLX_STENCIL_SIZE,	8,
		GLX_RED_SIZE,		8,
		GLX_GREEN_SIZE,		8,
		GLX_BLUE_SIZE,		8,
		GLX_SAMPLE_BUFFERS,	0,
		GLX_SAMPLES,		0,
		GLX_DOUBLEBUFFER,	1,
		None
	};

	int num_fbc = 0;
	GLXFBConfig *fb_config = glXChooseFBConfig(x->display, x->screen, visual_attributes, &num_fbc);

	GLint context_attributes[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	x->glx_context = glXCreateContextAttribsARB(
			x->display, fb_config[0], NULL, 1, context_attributes);	

	if (!x->glx_context) {
		fprintf(stderr, "failed to create OpenGL context");
		exit(0);
	}

	glXMakeCurrent(x->display, x->window, x->glx_context);
	
	int gl_version = gladLoaderLoadGL();
	if (!gl_version) {
		fprintf(stderr, "failed to load OpenGL functions\n");
		exit(0);
	}
	printf("Loaded GL %d.%d\n", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));

	XWindowAttributes gwa;
	XGetWindowAttributes(x->display, x->window, &gwa);

	return x;
}

void x_stop(x_data_t *x) {
	glXMakeCurrent(x->display, 0, 0);
	glXDestroyContext(x->display, x->glx_context);
	XDestroyWindow(x->display, x->window);
	XFreeColormap(x->display, x->color_map);
	XCloseDisplay(x->display);
	free(x);
}

void x_end_frame(x_data_t *x) {
	if (XPending(x->display)) {
		XNextEvent(x->display, &x->event);

		switch (x->event.type) {
			case KeyPress: {
				//x->running = 0; // to kill on key press
				return;
			} break;

			case Expose: {
				XGetWindowAttributes(x->display, x->window, &x->window_attributes);
				glViewport(0, 0, x->window_attributes.width, x->window_attributes.height);
				x->viewport_size_callback(x->window_attributes.width, x->window_attributes.height);
			} break;
		}
	}
	glXSwapBuffers(x->display, x->window);
}
