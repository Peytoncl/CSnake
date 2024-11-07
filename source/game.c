#include <GL/glut.h>

#include <stdio.h>
#include "stdbool.h"
#include <stdlib.h> 
#include <math.h>

#include "graphics.h"
#include "colors.h"

//Setup key inputs
#define KEY_COUNT 256 
bool keys[KEY_COUNT];

//Global snake variable
Snake snake = {{0, 0}, WINDOW_W * WINDOW_H};

//5 fps
#define FRAME_DELAY 75 //Snake Speed CALCULATION: 1000 / TARGET_FPS = FRAME_DELAY

//Grid of the apples and snake
int sizes = 25;

//Players spawns inside a random area, this is the padding in the grid from the very edge of the window.
#define SPAWN_PADDING_MULTIPLIER 1;
int padding;

//Max amount of apples to be spawned at once
#define AMOUNT_OF_APPLES 2

Vector2 apples[AMOUNT_OF_APPLES];

Color appleColor = {0, 0, 255};
Color snakeColor = {0, 255, 0};

//Extra setup
Vector2 emptyPos = EMPTY;

bool gameStarted = false;

//Initialize game variables
void startNewGame()
{
  gameStarted = true;

  padding = sizes * SPAWN_PADDING_MULTIPLIER;

  Vector2 spawnDir = RIGHT;

  //Initialize snake position values
  int i = 0;

  for (i = 0; i < WINDOW_W * WINDOW_H; i++)
  {
    snake.positions[i] = emptyPos;
  }

  Vector2 randomWindowPos = RandomWindowPosition(padding, sizes);

  snake.positions[0] = randomWindowPos;
  snake.direction = spawnDir;

  Color normalizedColor = NormalizeColor(snakeColor);
  snakeColor = normalizedColor;

  //Initialize apples
  for (i = 0; i < AMOUNT_OF_APPLES; i++)
  {
    apples[i] = emptyPos;
  }

  Color normalizedColor1 = NormalizeColor(appleColor);
  appleColor = normalizedColor1;
}

//Window initialization
void init()
{
  glClearColor(0.1, 0.1, 0.1, 0);

  gluOrtho2D(0, WINDOW_W, WINDOW_H, 0);
}

//Display update
void display() 
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Make sure all apples are spawned in
  int i = 0;

  glBegin(GL_QUADS);

  glColor3f(appleColor.r, appleColor.g, appleColor.b);

  for (i = 0; i < AMOUNT_OF_APPLES; i++)
  {
    if (apples[i].x == emptyPos.x && apples[i].y == emptyPos.y)
    {
      Vector2 appleSpawn = RandomWindowPosition(padding, sizes);

      apples[i] = appleSpawn;

      glVertex2f(appleSpawn.x, appleSpawn.y);  //Bottom left corner
      glVertex2f(appleSpawn.x + sizes, appleSpawn.y);  //Bottom right corner
      glVertex2f(appleSpawn.x + sizes, appleSpawn.y + sizes);  //Top right corner
      glVertex2f(appleSpawn.x, appleSpawn.y + sizes);  //Top left corner
    }
    else
    {
      glVertex2f(apples[i].x, apples[i].y);  
      glVertex2f(apples[i].x + sizes, apples[i].y);  
      glVertex2f(apples[i].x + sizes, apples[i].y + sizes); 
      glVertex2f(apples[i].x, apples[i].y + sizes); 
    }
  }

  //Draw snake
  glColor3f(snakeColor.r, snakeColor.g, snakeColor.b);

  for (i = 0; i < WINDOW_W * WINDOW_H; i++)
  {
    if (snake.positions[i].x != emptyPos.x && snake.positions[i].y != emptyPos.y)
    {
      glVertex2f(snake.positions[i].x, snake.positions[i].y); 
      glVertex2f(snake.positions[i].x + sizes, snake.positions[i].y); 
      glVertex2f(snake.positions[i].x + sizes, snake.positions[i].y + sizes);  
      glVertex2f(snake.positions[i].x, snake.positions[i].y + sizes); 
    }
    else break;
  }

  glEnd();

  glutSwapBuffers();
}

//Handle key inputs
void keyDown(unsigned char key, int x, int y) 
{
  keys[key] = true; 
}

void keyUp(unsigned char key, int x, int y) 
{
  keys[key] = false; 
}

//Movement variables
Vector2 Up = UP;
Vector2 Down = DOWN;
Vector2 Left = LEFT;
Vector2 Right = RIGHT;

//Idle update
void update()
{
  if (!gameStarted) startNewGame();

  //Handle key input to set the snakes direction
  if (keys['w'])
  {
    snake.direction = Up;
  }
  if (keys['s']) 
  {
    snake.direction = Down;
  }
  if (keys['a']) 
  {
    snake.direction = Left;
  }
  if (keys['d']) 
  {
    snake.direction = Right;
  }

  //Movement calculations
  int nextX = snake.positions[0].x + (snake.direction.x * sizes);
  int nextY = snake.positions[0].y + (snake.direction.y * sizes);

  printf("X: %d", nextX);
  printf("Y: %d\n", nextY);

  Vector2 newHeadPosition = {nextX, nextY};

  int i = 0;

  //Apple detection
  for(i = 0; i < AMOUNT_OF_APPLES; i++)
  {
    if(nextX == apples[i].x && nextY == apples[i].y)
    {
      printf("Apple collision! Inserting position: (%f, %f)\n", apples[i].x, apples[i].y);

      InsertPosition(snake.positions, apples[i], 0, WINDOW_W * WINDOW_H);

      printf("New head position after insert: (%f, %f)\n", apples[i].x, apples[i].y);

      newHeadPosition = snake.positions[0];

      apples[i] = emptyPos;

      break;
    }
  }

  //Move body
  for(i = 1; i < WINDOW_W * WINDOW_H; i++)
  {
    snake.positions[i - 1] = snake.positions[i];

    if (snake.positions[i].x == -1 && snake.positions[i].y == -1) break;
  }

  snake.positions[0] = newHeadPosition;

  //Fire display update
  glutPostRedisplay();

  glutTimerFunc(FRAME_DELAY, update, 0); // Reschedule update after FRAME_DELAY ms
}

//Keeps window locked at all times to prevent from making window bigger during gameplay
void windowResize(int width, int height)
{
  glutReshapeWindow(WINDOW_W, WINDOW_H);
}

//Runs when application starts, assigning OpenGL events
int main(int argc, char** argv) 
{
  glutInit(&argc, argv);    

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);        
  glutInitWindowSize(WINDOW_W, WINDOW_H);        
    
  glutCreateWindow("CSnake");  

  init();

  for (int i = 0; i < KEY_COUNT; i++) 
  {
    keys[i] = false;
  }

  glutDisplayFunc(display);

  glutKeyboardFunc(keyDown);
  glutKeyboardUpFunc(keyUp);

  glutReshapeFunc(windowResize);

  glutTimerFunc(FRAME_DELAY, update, 0); //Make the update function run based on the frame delay

  glutMainLoop();      
  return 0;   
}