#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>

#include "obstacles.h"
#include "display.h"

#define  VERTEX_RADIUS    3

const int   OBSTACLE_COLOR = 0x7799FF;
const int   BORDER_COLOR   = 0x000000;
const int   EDGE_COLOR     = 0x000000;
const int   VERTEX_COLOR   = 0xDD0000;

// Window attributes
int   WINDOW_WIDTH = ENV_WIDTH;
int   WINDOW_HEIGHT = ENV_HEIGHT;


//Display-related variables
Display *display;
Window   win;
GC       gc;


// Initialize and open the simulator window with size ENV_WIDTH x ENV_HEIGHT.
void initializeWindow() {
	XInitThreads();

  // Open connection to X server
  display = XOpenDisplay(NULL);

  // Create a simple window, set the title and get the graphics context then
  // make is visible and get ready to draw
  win = XCreateSimpleWindow(display,  RootWindow(display, 0), 0, 0, ENV_WIDTH, ENV_HEIGHT, 0, 0x000000, 0xFFFFFF);
  XStoreName(display, win, "Graph Pathfinder");
  gc = XCreateGC(display, win, 0, NULL);
  XMapWindow(display, win);
  XFlush(display);
  usleep(20000);  // sleep for 20 milliseconds.

  // Indicate that we'd like to be able to gracefully handle window closing
  Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False); 
  XSetWMProtocols(display, win, &WM_DELETE_WINDOW, 1);

  // Indicate which events we want to handle
  XSelectInput(display, win, ExposureMask | ClientMessage );// |	StructureNotifyMask);     
}

// Close the display window
void closeWindow() {
  XFreeGC(display, gc);
  XUnmapWindow(display, win);
  XDestroyWindow(display, win);
  exit(0);
}


// Redraw the graph
void redraw(Environment *env) {
	// Erase the background 
	XSetForeground(display, gc, 0xFFFFFF);
	XFillRectangle(display, win, gc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Draw all the obstacles
	for (int i=0; i<env->numObstacles; i++) {
		XSetForeground(display, gc, OBSTACLE_COLOR);
		XFillRectangle(display, win, gc, env->obstacles[i].x,WINDOW_HEIGHT-env->obstacles[i].y, 
		               env->obstacles[i].w, env->obstacles[i].h);
		XSetForeground(display, gc, BORDER_COLOR);
		XDrawRectangle(display, win, gc, env->obstacles[i].x,WINDOW_HEIGHT-env->obstacles[i].y, 
		               env->obstacles[i].w, env->obstacles[i].h);
	}
    
	// Draw the edges of the graph as long as there is indeed a graph created
	if (env->vertices != NULL) {
		// Go through all the vertices of the graph and draw edges from them to their neighbours
		XSetForeground(display, gc, EDGE_COLOR);
		for (int i=0; i<env->numVertices; i++) {
			Vertex *v = env->vertices[i];
			Neighbour *n = v->neighbours;
			while (n != NULL) {
				XDrawLine(display, win, gc, v->x, WINDOW_HEIGHT-(v->y), n->vertex->x, WINDOW_HEIGHT-(n->vertex->y));
				n = n->next;
			}	
		}
	  
		// Draw all the vertices of the graph
		for (int i=0; i<env->numVertices; i++) {
			XFlush(display);
			Vertex  *v = env->vertices[i];
			XSetForeground(display, gc, VERTEX_COLOR);
			XFillArc(display, win, gc, v->x-VERTEX_RADIUS, WINDOW_HEIGHT-(v->y+VERTEX_RADIUS),
		  	       2*VERTEX_RADIUS, 2*VERTEX_RADIUS, 0, 360*64);
			XSetForeground(display, gc, BORDER_COLOR);
			XDrawArc(display, win, gc, v->x-VERTEX_RADIUS, WINDOW_HEIGHT-(v->y+VERTEX_RADIUS),
		  	       2*VERTEX_RADIUS, 2*VERTEX_RADIUS, 0, 360*64);
		}
	}
	XFlush(display);
}



// Redraw all the obstacles and the edges and the vertices
void *runDisplay(void *e) {
	Environment      *env = (Environment *)e;
  XConfigureEvent  cEvent;
  XEvent           event;

	// Draw the environment to start
	redraw(env);
	
  // Go into infinite loop, updating the animation at FPS rate
  //unsigned long lastRepaint = getTimeInMicroseconds(); // time in microseconds
  while(1) {
    // Handle any pending events, and then we'll deal with redrawing
    XNextEvent(display, &event);
    switch(event.type) {
      case Expose:
				redraw(env);
				break;
      case ConfigureNotify:
				cEvent = event.xconfigure;
				// Need to check for window resizing, since this type of event can be
				// generated for other reasons too
				if ((cEvent.width != WINDOW_WIDTH) || (cEvent.height != WINDOW_HEIGHT)) {
	  			WINDOW_WIDTH = cEvent.width;
	  			WINDOW_HEIGHT = cEvent.height;
	  			redraw(env);
				}
				break;
			case ClientMessage:
				// We really should check here for other client message types, 
				// but since the only protocol registered above is WM_DELETE_WINDOW,
 				// it is safe to assume that we want the window closing event.
				closeWindow();
				break;
     }
  }
}

