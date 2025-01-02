#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "obstacles.h"


// Return 1 if line segment (v1---v2) intersects line segment (v3---v4), otherwise return 0
unsigned char linesIntersect(short v1x, short v1y, short v2x, short v2y, short v3x, short v3y, short v4x, short v4y) {
	float uA = ((v4x-v3x)*(v1y-v3y) - (v4y-v3y)*(v1x-v3x)) / (float)(((v4y-v3y)*(v2x-v1x) - (v4x-v3x)*(v2y-v1y)));
	float uB = ((v2x-v1x)*(v1y-v3y) - (v2y-v1y)*(v1x-v3x)) / (float)(((v4y-v3y)*(v2x-v1x) - (v4x-v3x)*(v2y-v1y)));

	// If uA and uB are between 0-1, there is an intersection
	if (uA > 0 && uA < 1 && uB > 0 && uB < 1) 
		return 1;

	return 0;
}

float d(Vertex *v1, Vertex *v2){
	return (v2->x-v1->x)*(v2->x-v1->x) + (v2->y-v1->y)*(v2->y-v1->y);
}

int checkObstacle(Environment *env, Vertex *v1, Vertex *v2, Obstacle ob){
	if (linesIntersect(v1->x, v1->y, v2->x, v2->y, ob.x, ob.y, ob.x + ob.w, ob.y) || //top
		linesIntersect(v1->x, v1->y, v2->x, v2->y, ob.x, ob.y - ob.h, ob.x + ob.w, ob.y - ob.h) || //bottom
		linesIntersect(v1->x, v1->y, v2->x, v2->y, ob.x, ob.y, ob.x, ob.y - ob.h) || //left
		linesIntersect(v1->x, v1->y, v2->x, v2->y, ob.x + ob.w, ob.y, ob.x + ob.w, ob.y - ob.h)) { //right
		return 0; // Intersection found
	}
	return 1;
}

void createEdges(Environment *env){
	for(int i = 0; i < env->numVertices; i++){
		//Find neighboirs of vertice I
		int k = 0;
		for(int j = 0; j < env->numVertices; j++){
			if(i == j) continue;

			int cont = 0;
			for(int k = 0; k < env->numObstacles; k++){
				if(checkObstacle(env, env->vertices[i], env->vertices[j], env->obstacles[k]) == 0){
					cont = 1;
					break;
				} 
			}
			if(cont) continue;
			

			float distanceOfJ = d(env->vertices[i], env->vertices[j]);

			Neighbour* currNode = env->vertices[i]->neighbours;
			Neighbour* newNode = malloc(sizeof(Neighbour));
			if(newNode == NULL) exit(-1);

			newNode->vertex = env->vertices[j];
			newNode->next = NULL;
			//If head is not init.
			if(currNode == NULL){
				env->vertices[i]->neighbours = newNode;
				k++;
			} 
			//If newNode is less than head
			else if(distanceOfJ < d(env->vertices[i], currNode->vertex)){
				newNode->next = currNode;
				env->vertices[i]->neighbours = newNode;
				k++;
			}
			//Somewhere else 
			else {
				Neighbour* prevNode = NULL;
				while (currNode->next != NULL && distanceOfJ > d(env->vertices[i], currNode->next->vertex))
				{
					prevNode = currNode;
					currNode = currNode->next;
				}
				newNode->next = currNode->next;
				currNode->next = newNode;
				k++;
			}
			if(k > env->k){
				int l = 0;
				Neighbour* currNode = env->vertices[i]->neighbours;
				while (currNode != NULL && l != env->k - 1)
				{
					l++;
					currNode = currNode->next;
				}
				Neighbour* nodefordeletion = currNode->next;
				if(currNode != NULL)
					currNode->next = NULL;
				free(nodefordeletion);
			}

		}
	}
}


void createVertices(Environment *env){
	for(int i = 0; i < env->numVertices; i++){
		env->vertices[i] = malloc(sizeof(Vertex));

		if(env->vertices[i] == NULL){
			printf("A critical error occured. ");
			exit(-1);
		}

		
		env->vertices[i]->x = abs((short) (rand()))%env->maximumX;	
		env->vertices[i]->y = abs((short) (rand()))%env->maximumY;
		env->vertices[i]->neighbours = NULL;

		int inObstacle = 1;
		while(inObstacle){
			inObstacle = 0;

			for(int j = 0; j < env->numObstacles; j++){
				Obstacle ob = env->obstacles[j];

				if(env->vertices[i]->x  >= env->obstacles[j].x && env->vertices[i]->x  <= env->obstacles[j].x + env->obstacles[j].w && env->vertices[i]->y  <= env->obstacles[j].y && env->vertices[i]->y  >= env->obstacles[j].y - env->obstacles[j].h ){
					env->vertices[i]->y = abs((short) (rand()))%env->maximumY;
					env->vertices[i]->x = abs((short) (rand()))%env->maximumX;
					inObstacle = 1;
					break;
				}		
			}
		}
	}
}
void cleanupEverything(Environment *env);
// Create a graph using the numVertices and k parameters of the given environment.
void createGraph(Environment *env) {	
	env->vertices = malloc(sizeof(Vertex *)*env->numVertices);
	
	if(env->vertices == NULL){
		printf("A critical error occured. ");
		exit(-1);
	}

	srand(time(NULL));
	//Make vertices
	createVertices(env);
	//Now Make Edges
	createEdges(env);
}


// This procedure cleans up everything by creeing all alocated memory
void cleanupEverything(Environment *env) {
	for (int i = 0; i < env->numVertices; i++) {
        Neighbour *currentNode = env->vertices[i]->neighbours;
        while (currentNode != NULL) {
            Neighbour *nextNode = currentNode->next;
            free(currentNode);
            currentNode = nextNode;
        }
        free(env->vertices[i]);
    }
    free(env->vertices);
}