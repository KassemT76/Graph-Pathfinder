
// This represents a rectangular obstacle
typedef struct obst {
	short   	 x;
	short   	 y; 
	short   	 w;
	short   	 h;
} Obstacle;


// This represents a vertex of the graph
typedef struct vert {
	short          x;           // x-coord of the vertex within the environment
	short          y;           // y-coord of the vertex within the environment
	struct neigh  *neighbours;	// A linked list of k-nearest neighbours
} Vertex;


// This represents a neighbour of a vertex in the graph
typedef struct neigh {
	Vertex        *vertex;	// Pointer to a neighbour vertex
	struct neigh  *next;		// Next vertex in the list of neighbours
} Neighbour;



// This represents an environment with obstacles, a graph and a path in the graph
typedef struct {
	unsigned short  k;             // Value of "k" for finding the k-nearest-neighbours
	unsigned short  maximumX;      // The maximum X value in the environment
	unsigned short  maximumY;      // The maximum Y value in the environment
	Obstacle       *obstacles;     // rectangular obstacles in the environment
	unsigned short  numObstacles;	 // # of obstacles
	Vertex        **vertices;      // Pointer to first vertex in dynamically-allocated array of vertex pointers
	unsigned short  numVertices;   // # of vertices allocated
} Environment;
	

// These are the external functions being used by the test program
extern void createGraph(Environment *);
extern void cleanupEverything(Environment *);

