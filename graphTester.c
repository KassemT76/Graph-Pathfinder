// This program simulates the creation of randomly-generated graphs in a polygonal environment.
// This program should take in three command-line arguments and run as follows:
//
// 		graphTester <V> <K> <E>
//
// <V> the number of vertices to create (i.e., 20-2000)
// <K> the number of nerest-neighbours to connect (i.e., 1-25)
// <E> the environment number to test (i.e., 1-5)


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>                                                           
#include <signal.h>

#include "obstacles.h"
#include "display.h"


int main(int argc, char *argv[]) {
	Environment   environment;
	char          environmentNumber;
  	
	srand(time(NULL));	// Set the seed for random numbers

	// Ensure command line arg is there
	if (argc <= 3) {
		printf("Error: invalid parameters. Usage is as follows\n\n");
		printf("graphTester <V> <K> <E>\n\n");
		printf("  <V> the number of vertices to create (i.e., 20-2000)\n");
		printf("  <K> the number of nerest-neighbours to connect (i.e., 1-25)\n");
		printf("  <E> the environment number to test (i.e., 1-5)\n\n");
		exit(-1);
	}
	else {
		environment.numVertices = atoi(argv[1]);
		environment.vertices = NULL;
		if ((environment.numVertices < 20) || (environment.numVertices > 2000)) {
			printf("Error: number of vertices must be between 20 and 2000.\n");
			exit(-1);
		}
		environment.k = atoi(argv[2]);
		if ((environment.k < 1) || (environment.k > 25)) {
			printf("Error: k value (i.e., # nearest neighbours) must be between 1 and 25.\n");
			exit(-1);
		}
		environmentNumber = atoi(argv[3]);
		if ((environmentNumber < 1) || (environmentNumber > 5)) {
			printf("Error: environment number must be between 1 and 5.\n");
			exit(-1);
		}
	}

	// Initialize the environment and add some obstacles
	short int *xsp, *ysp, *wsp, *hsp;
	environment.maximumX = ENV_WIDTH;
	environment.maximumY = ENV_HEIGHT;
	switch (environmentNumber) {
		case 1: {
			short xs[] = {  0,  20, 720,  20, 120, 280, 580, 320,  80};
			short ys[] = {540, 540, 540,  20, 460, 420, 340, 280, 300};
			short ws[] = { 20, 700,  20, 700,  60, 220,  60,  40, 160};
			short hs[] = {540,  20, 540,  20,  80, 100, 200, 180, 110};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 9;
			break; }
		case 2: {
			short xs[] = {  0,  20, 720,  20,  60, 200, 355, 600, 120, 280};
			short ys[] = {540, 540, 540,  20, 480, 400, 480, 200, 200, 240};
			short ws[] = { 20, 700,  20, 700, 385, 480,  35,  20,  20,  40};
			short hs[] = {540,  20, 540,  20,  30,  45, 225,  20,  20, 220};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 10;
			break; }
		case 3: {
			short xs[] = {  0,  20, 720,  20,  20, 500, 480, 180, 360,  80};
			short ys[] = {540, 540, 540,  20, 520, 520, 160, 200, 480, 260};
			short ws[] = { 20, 700,  20, 700, 100, 220, 140,  50,   2, 400};
			short hs[] = {540,  20, 540,  20, 100, 160,  50, 130, 400,   2};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 10;
			break; }
		case 4: {
			short xs[14*10+4] = {  0,  20, 720,  20};
			short ys[14*10+4] = {540, 540, 540,  20};
			short ws[14*10+4] = { 20, 700,  20, 700};
			short hs[14*10+4] = {540,  20, 540,  20};
			for (int i=0; i<14; i++)
				for (int j=0; j<10; j++) {
					xs[i*10+j+4] = 50*(i+1);
					ys[i*10+j+4] = 50*(j+1);
					ws[i*10+j+4] = 20;
					hs[i*10+j+4] = 20;
				}
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 4+14*10;
			break; }
		case 5: {
			short xs[] = {  0,  20, 720,  20,  70};
			short ys[] = {540, 540, 540,  20, 470};
			short ws[] = { 20, 700,  20, 700, 600};
			short hs[] = {540,  20, 540,  20, 400};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 5;
			break; }
		default:
			environment.numObstacles = 0;
	}
	environment.obstacles = (Obstacle *)malloc(environment.numObstacles*sizeof(Obstacle));
	for (int i=0; i<environment.numObstacles; i++) {
		environment.obstacles[i].x = xsp[i];
		environment.obstacles[i].y = ysp[i];
		environment.obstacles[i].w = wsp[i];
		environment.obstacles[i].h = hsp[i];
	}

	// Create the Graph
	createGraph(&environment);

	// Open a window for display
	initializeWindow();
	
	// Spawn a thread to handle display
  pthread_t        thread_draw;
  pthread_attr_t   attr;
  pthread_attr_init(&attr);  // initialize the attribute structure
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // set the detach state to "detached"
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED); // override the default of INHERIT_SCHED
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
  pthread_create(&thread_draw, &attr, runDisplay, (void *)(&environment));

	runDisplay(&environment);
	
	// Cleanup by freeing all allocated memory
	cleanupEverything(&environment);
	
	// Close the environment window
	closeWindow();
}

