#ifdef OS_Linux


#include <stdbool.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <hap.h>

#include "window_management.h"


struct X11Window {
	Window                 ref;

	Display                *display;
	GLXContext             glContext;
};


typedef struct X11Window X11Window;


Atom                   WM_DELETE_WINDOW;


GLint glAttributes[] = {
	GLX_RGBA, GLX_DEPTH_SIZE, 24,
	GLX_DOUBLEBUFFER, None
};


void* window_create(HAPEngine *engine, const int width, const int height) {
	X11Window* window = (X11Window*) calloc(1, sizeof(X11Window*));
	Window root;
	XSetWindowAttributes setAttributes;
	XVisualInfo *visualInfo;

	if (window == NULL) {
		(*engine).log_error(engine, "Could not allocate memory to create a window.");
		return NULL;
	}

	(*window).display = XOpenDisplay(NULL);

	// Occurs when the XServer can't be connected to
	if ((*window).display == NULL) {
		(*engine).log_error(engine, "Could not open display. Is there an XServer running?\n");
		return NULL;
	}

	root = DefaultRootWindow((*window).display);

	if (!root) {
		(*engine).log_error(engine, "Could not find root window.\n");
		root = DefaultRootWindow((*window).display);
	}

	visualInfo = glXChooseVisual((*window).display, 0, glAttributes);

	if (visualInfo == NULL) {
		(*engine).log_error(engine, "Couldn't get GLX VisualInfo for XServer.\n");
		return NULL;
	}

	setAttributes.event_mask = ExposureMask | KeyPressMask;
	setAttributes.colormap = XCreateColormap(
	                             (*window).display, root,
	                             (*visualInfo).visual,
	                             AllocNone
	                         );


	(*window).ref = XCreateWindow(
	                    (*window).display, root,
	                    10, 10, width, height, 0,
	                    (*visualInfo).depth,
	                    InputOutput,
	                    (*visualInfo).visual,
	                    CWColormap | CWEventMask,
	                    &setAttributes
	                );

	XMapWindow((*window).display, (*window).ref);
	WM_DELETE_WINDOW = XInternAtom((*window).display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols((*window).display, (*window).ref, &WM_DELETE_WINDOW, 1);

	(*window).glContext = glXCreateContext(
	                          (*window).display, visualInfo,
	                          NULL, GL_TRUE
	                      );

	glXMakeCurrent(
	    (*window).display, (*window).ref,
	    (*window).glContext
	);

	return (void*) window;
}


int window_update(void* state) {
	XEvent event;
	XWindowAttributes attributes;

	X11Window *window = (X11Window*) state;

	if (window == NULL) return -1;

	// TODO: Non-blocking pulling to not block simulation too long
	while (XEventsQueued((*window).display, QueuedAfterFlush)) {
		XNextEvent((*window).display, &event);

		switch (event.type) {
		case ClientMessage:
			if ((Atom)event.xclient.data.l[0] != WM_DELETE_WINDOW) break;

			return -1;
			break;

		case Expose:
			XGetWindowAttributes((*window).display, (*window).ref, &attributes);
			glViewport(0, 0, attributes.width, attributes.height);
			break;
		}
	}

	return 0;
}


void window_render(void *state) {
	X11Window *window = (X11Window*) state;
	glXSwapBuffers((*window).display, (*window).ref);
}

void window_close(void* state) {
	X11Window *window = (X11Window*) state;

	glXMakeCurrent((*window).display, None, NULL);
	glXDestroyContext((*window).display, (*window).glContext);
	(*window).glContext = 0;

	XDestroyWindow((*window).display, (*window).ref);
	XCloseDisplay((*window).display);

	free(window);
}


#endif
