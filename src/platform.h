#ifndef PLATFORM_H
#define PLATFORM_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <glad/glx.h>

typedef struct {
  int                  screen;
  Display             *display;
  Window               root;
  Visual              *visual;
  Colormap             color_map;
  XSetWindowAttributes attributes;
  Window               window;
  GLXContext           glx_context;
  XWindowAttributes    window_attributes;
  XEvent               event;
  void      (*viewport_size_callback) (const unsigned int width, const unsigned int height);
} x_data_t;

x_data_t  x_start(  const char *window_title,
        unsigned int window_width,
        unsigned int window_height);

void    x_stop    (x_data_t *x);
void    x_end_frame  (x_data_t *x_data);

#endif // PLATFORM_H
