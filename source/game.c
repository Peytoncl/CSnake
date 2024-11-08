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
int amountOfSnakeParts = 0;

//5 fps
#define FRAME_DELAY 1 //Key input speed

#define SNAKE_MOVE_DELAY 75 //Snake Speed CALCULATION: 1000 / TARGET_FPS = FRAME_DELAY

//Grid of the apples and snake
int sizes = 50;

//Players spawns inside a random area, this is the padding in the grid from the very edge of the window.
#define SPAWN_PADDING_MULTIPLIER 1;
int padding;

//Max amount of apples to be spawned at once
#define AMOUNT_OF_APPLES 2

Vector2 apples[AMOUNT_OF_APPLES];

Color appleColor = {0, 0, 255};

Color snakeColor = {0, 255, 0};
Color headColor = {255, 0, 0};

//Extra setup
#define center {WINDOW_W / 2, WINDOW_H / 2};
Vector2 emptyPos = EMPTY;

bool gameStarted = false;
bool colorsNormalized = false;

//Initialize game variables
void startNewGame()
{
  gameStarted = true;

  padding = sizes * SPAWN_PADDING_MULTIPLIER;

  Vector2 spawnDir = RIGHT;

  amountOfSnakeParts = 0;

  //Initialize snake position values
  int i = 0;

  for (i = 0; i < WINDOW_W * WINDOW_H; i++)
  {
    snake.positions[i] = emptyPos;
  }

  Vector2 randomWindowPos = RandomWindowPosition(padding, sizes);

  //snake.positions[0] = randomWindowPos;

  Vector2 cen = center;

  snake.positions[0] = cen;
  snake.direction = spawnDir;

  if(!colorsNormalized)
  {
    Color normalizedColor = NormalizeColor(snakeColor);
    snakeColor = normalizedColor;

    normalizedColor = NormalizeColor(headColor);
    headColor = normalizedColor;
  }

  //Initialize apples
  for (i = 0; i < AMOUNT_OF_APPLES; i++)
  {
    apples[i] = emptyPos;
  }

  if(!colorsNormalized)
  {
    Color normalizedColor1 = NormalizeColor(appleColor);
    appleColor = normalizedColor1;
  }

  colorsNormalized = true;
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
      //Soon to be removed because i dont wanna be generating the apple positions on the display function
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
      if (i != 0) glColor3f(snakeColor.r, snakeColor.g, snakeColor.b); //Body color
      else glColor3f(headColor.r, headColor.g, headColor.b); //Special head color

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

bool moveSnake = false;

//Snake movement update
void snakeMovement()
{
  //Movement calculations

  int nextX = snake.positions[0].x + (snake.direction.x * sizes);
  int nextY = snake.positions[0].y + (snake.direction.y * sizes);

  /* Collision Detection */

  Vector2 newHeadPosition = {nextX, nextY};

  int i = 0;

  bool hitApple = false;

  //Check if hitting own body
  for(i = 0; i < WINDOW_W * WINDOW_H; i++)
  {
    if(snake.positions[i].x != -1)
    {
      if(nextX == snake.positions[i].x && nextY == snake.positions[i].y)
      {
        //To ignore the apple collision detection
        hitApple = true;

        startNewGame();
      }
    }
    else break;
  }

  //Check if head is outside window
  if(nextX > WINDOW_W || nextY > WINDOW_H || nextY < 0 || nextX < 0)
  {
    //To ignore the apple collision detection
    hitApple = true;

    startNewGame();
  }

  //Apple detection
  for(i = 0; i < AMOUNT_OF_APPLES; i++)
  {
    if(nextX == apples[i].x && nextY == apples[i].y)
    {
      InsertPosition(snake.positions, apples[i], 0, WINDOW_W * WINDOW_H);

      amountOfSnakeParts++;
      hitApple = true;

      apples[i] = emptyPos;

      //generate new apple position

      break;
    }
  }

  /* Snake movement */

  if (!hitApple)
  {
    //Move body
    for(i = amountOfSnakeParts; i > 0; i--)
    {
      snake.positions[i] = snake.positions[i - 1];
    }

    snake.positions[0] = newHeadPosition;
  }
}

//Idle update
void update()
{
  if (!gameStarted) startNewGame();

  if (moveSnake)
  {
    snakeMovement();

    moveSnake = false;
  }

  //Handle key input to set the snakes direction
  if (keys['w'] || keys['W'])
  {
    if (snake.direction.x != Down.x && snake.direction.y != Down.y && amountOfSnakeParts > 0)
    {
      snake.direction = Up;
    }
    else if (amountOfSnakeParts == 0) snake.direction = Up;
  }
  else if (keys['s'] || keys['S']) 
  {
    if (snake.direction.x != Up.x && snake.direction.y != Up.y && amountOfSnakeParts > 0)
    {
      snake.direction = Down;
    }
    else if (amountOfSnakeParts == 0) snake.direction = Down;
  }
  else if (keys['a'] || keys['A']) 
  {
    if (snake.direction.x != Right.x && snake.direction.y != Right.y && amountOfSnakeParts > 0)
    {
      snake.direction = Left;
    }
    else if (amountOfSnakeParts == 0) snake.direction = Left;
  }
  else if (keys['d'] || keys['D'])
  {
    if (snake.direction.x != Left.x && snake.direction.y != Left.y && amountOfSnakeParts > 0)
    {
      snake.direction = Right;
    }
    else if (amountOfSnakeParts == 0) snake.direction = Right;
  }

  //Fire display update
  glutPostRedisplay();

  glutTimerFunc(FRAME_DELAY, update, 0); // Reschedule update after FRAME_DELAY ms
}

void snakeMoveTimer(int value)
{
    // Trigger snake movement
    moveSnake = true;

    // Re-set the timer for the next move
    glutTimerFunc(SNAKE_MOVE_DELAY, snakeMoveTimer, 0);
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

  glutTimerFunc(SNAKE_MOVE_DELAY, snakeMoveTimer, 0); //Make the snake function run based on the frame delay

  glutTimerFunc(FRAME_DELAY, update, 0);

  glutMainLoop();      
  return 0;   
}