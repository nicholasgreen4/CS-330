#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;

// Define movement speed for the controllable brick
const float MOVE_SPEED = 0.02f;

void processInput(GLFWwindow* window, float* x, float* y);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE, CONTROLLABLE };
enum ONOFF { ON, OFF };

int score = 0;



class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	BRICKTYPE brick_type;
	ONOFF onoff;
	int hp;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		hp = 25;
		onoff = ON;
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside);
			glVertex2d(x + halfside, y - halfside);
			glVertex2d(x - halfside, y - halfside);
			glVertex2d(x - halfside, y + halfside);

			glEnd();
		}
	}

	void UpdateColor(Brick* brk, float red, float green, float blue)
	{
		brk->red = red;
		brk->green = green;
		brk->blue = blue;
	}
};


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float speed = 0.03;
	int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left
	int age = 0;

	Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		radius = rr;  
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
	}

	bool CheckCollision(Brick* brk, Circle* currentCircle)
	{
		bool collisonHappened = false;
		if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
		{
			// Change the color of the brick to match the color of the ball that collided with it
			brk->UpdateColor(brk, currentCircle->red, currentCircle->green, currentCircle->blue);

			if (brk->onoff == ON)
			{
				if (brk->brick_type == CONTROLLABLE) {
					// If a circle hits a controllable brick, the circle is destroyed and a point is added
					collisonHappened = true;
					score++;
					// Print out score
					printf("Score: %d\n", score);
				}
				if (brk->brick_type == REFLECTIVE || brk->brick_type == CONTROLLABLE)
				{
					direction = GetRandomDirection();
					x = x + 0.03;
					y = y + 0.04;
				}
				else if (brk->brick_type == DESTRUCTABLE)
				{
					// If a circle hits a destructable brick, the circle is destroyed and the destructable brick is weakened
					collisonHappened = true;
					brk->hp = brk->hp - 1;
					if (brk->hp <= 0) { brk->onoff = OFF; }
				}
			}
		}


		return collisonHappened;
	}

	// Check for collison between circles
	bool CheckCollision(Circle* otherCircle) // otherCircle instead of brk
	{
		// Calculate the distance between the centers of the circles
		float distanceX = x - otherCircle->x;
		float distanceY = y - otherCircle->y;

		// Calculate the squared distance (more efficient than square root)
		float squaredDistance = distanceX * distanceX + distanceY * distanceY;

		// Combine the radii of the circles
		float combinedRadius = radius + otherCircle->radius;

		// Check if the squared distance is less than the squared combined radius
		return squaredDistance < combinedRadius * combinedRadius;
	}
	

	int GetRandomDirection()
	{
		return (rand() % 8) + 1;
	}


	void MoveOneStep()
	{
		// If the circle is going straight up, down, left, or right, pick a random direction when the edge of the screen is hit,
		// Otherwise it is a little boring watching them bounce back and forth on nothing
		if (direction <= 1) // up
		{
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}
		else if (direction == 2) // right
		{
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}
		else if (direction == 3) // down
		{
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}
		else if (direction == 4) // left
		{
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		} 
		else if (direction == 5) // up and right
		{
			// hit upper bound
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = 7; // start going down and right
			}

			// hit right bound
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = 6; // start going up and left
			}
		}
		else if (direction == 6) // up and left
		{
			// hit upper bound
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = 7; // start going down and right
			}

			// hit left bound
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = 5; // start going up and right
			}
		}
		else if (direction == 7) // down and right
		{
			// hit lower bound
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = 5; // start going up and right 
			}

			// hit right bound
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = 8; // start going down and left
			}
		}
		else if (direction == 8) // down and left
		{
			// hit lower bound
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = 6; // start going up and left
			}
			// hit left bound
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = 7; // start going down and right
			}
		}
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
};


vector<Circle> world;


int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(480, 480, "8-2 Assignment", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	float controllableX = 0.0f;
	float controllableY = -0.9f;

	Brick brick(REFLECTIVE, 0.5, -0.33, 0.2, 1, 1, 0);
	Brick brick2(DESTRUCTABLE, -0.5, 0.33, 0.2, 0, 1, 0);
	Brick brick3(DESTRUCTABLE, -0.5, -0.33, 0.2, 0, 1, 1);
	Brick brick4(REFLECTIVE, 0, 0, 0.2, 1, 0.5, 0.5);
	Brick brick5(CONTROLLABLE, controllableX, controllableY, 0.2f, 0.8f, 0.2f, 0.2f);

	while (!glfwWindowShouldClose(window)) {
		//Setup View
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window, &controllableX, &controllableY);

		//Movement
		for (int i = 0; i < world.size(); i++)
		{
			bool brick1_collision = world[i].CheckCollision(&brick, &world[i]);
			bool brick2_collision = world[i].CheckCollision(&brick2, &world[i]);
			bool brick3_collision = world[i].CheckCollision(&brick3, &world[i]);
			bool brick4_collision = world[i].CheckCollision(&brick4, &world[i]);
			bool brick5_collision = world[i].CheckCollision(&brick5, &world[i]);

			world[i].MoveOneStep();
			world[i].DrawCircle();

			if (brick1_collision || brick2_collision || brick3_collision || brick4_collision || brick5_collision)
			{
				world.erase(world.begin() + i);
			}

		}
		// Check circles for collison against every other circle
		for (int i = 0; i < world.size(); i++)
		{
			for (int j = i + 1; j < world.size(); j++)
			{

				if (world[i].CheckCollision(&world[j])) // collison between two circles occured
				{
					world[i].direction = world[i].GetRandomDirection();
					world[j].direction = world[j].GetRandomDirection();
				}
			}
		}

		brick.drawBrick();
		brick2.drawBrick();
		brick3.drawBrick();
		brick4.drawBrick();

		brick5.x = controllableX;
		brick5.y = controllableY;
		brick5.drawBrick();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate;
	exit(EXIT_SUCCESS);
}


void processInput(GLFWwindow* window, float* x, float* y)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		double r, g, b;
		r = rand() / 10000;
		g = rand() / 10000;
		b = rand() / 10000;
		Circle B(.9, .9, 02, 2, 0.05, r, g, b);
		B.direction = B.GetRandomDirection();
		world.push_back(B);
	}

	// Check for WASD key pressed and update coordinates
	float move_x = 0.0f;
	float move_y = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		move_x = MOVE_SPEED; 
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		move_x = -MOVE_SPEED;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		move_y = MOVE_SPEED;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		move_y = -MOVE_SPEED;
	}

	// Update coordinates with movement and prevent going out of bounds
	*x += move_x;
	if (*x > 1.0f) { *x = -1.0f; }
	else if (*x < -1.0f) { *x = 1.0f; }

	*y += move_y;
	if (*y > 1.0f - 0.1f)	{ *y = 1.0f - 0.1f; }
	else if (*y < -1.0f + 0.1f) { *y = -1.0f + 0.1f; } // Constrain to bottom of screen
}