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

#define FRAME_DELAY 25 //Key input speed (same calc as snake speed)

//200 Easy
//75 Normal
//50 Hard
#define SNAKE_MOVE_DELAY 75 //Snake Speed CALCULATION: 1000 / TARGET_FPS = FRAME_DELAY ms

//140 Easy (Actually kinda hard)
//70 Normal (In the middle)
//35 Hard (Preferred)

//Grid of the apples and snake, needs to be along a 70x grid
#define GRID_SIZE 35
int sizes = GRID_SIZE;

//Players spawns inside a random area, this is the padding in the grid from the very edge of the window.
#define SPAWN_PADDING_MULTIPLIER 0;
int padding;

//Global snake variable
Snake snake = {{0, 0}, WINDOW_W / GRID_SIZE * WINDOW_H / GRID_SIZE, 0};

//Max amount of apples to be spawned at once
#define AMOUNT_OF_APPLES 2

Vector2 apples[AMOUNT_OF_APPLES];

Color appleColor = {0, 0, 255};

Color snakeColor = {0, 255, 0};
Color headColor = {255, 0, 0};

//Extra setup
Vector2 emptyPos = EMPTY;

bool gameStarted = false;
bool colorsNormalized = false;

//Initialize game variables
void startNewGame()
{
  gameStarted = true;

  padding = sizes * SPAWN_PADDING_MULTIPLIER;

  Vector2 spawnDir = emptyPos;

  snake.amountOfSnakeParts = 0;

  //Initialize snake position values
  int i = 0;

  for (i = 0; i < WINDOW_W / GRID_SIZE * WINDOW_H / GRID_SIZE; i++)
  {
    snake.positions[i] = emptyPos;
  }

  Vector2 randomWindowPos = RandomWindowPosition(padding, sizes);

  //snake.positions[0] = randomWindowPos;

  //This is not correct and the snake is not spawning in the middle of the window
  Vector2 cen = {sizes, sizes};

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
  /* Movement calculations */

  //If player hasnt pressed a key yet then freeze them in place
  if (snake.direction.x == -1 && snake.direction.y == -1) return;

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
      if(nextX == snake.positions[i].x && nextY == snake.positions[i].y && i != snake.amountOfSnakeParts)
      {
        //To ignore the apple collision detection
        hitApple = true;

        startNewGame();
      }
    }
    else break;
  }

  //Check if head is outside window
  if(nextX >= WINDOW_W || nextY >= WINDOW_H || nextY < 0 || nextX < 0)
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
      //Add new position to front of snake positions to essentially create a new head
      InsertPosition(snake.positions, apples[i], 0, WINDOW_W * WINDOW_H);

      snake.amountOfSnakeParts++;

      //When the player beats the game
      if(snake.amountOfSnakeParts >= WINDOW_W / GRID_SIZE * WINDOW_H / GRID_SIZE)
      {
        startNewGame();
      }

      hitApple = true;

      //generate new apple position
      apples[i] = RandomAppleSpawn(padding, GRID_SIZE, snake.amountOfSnakeParts, snake.positions);

      break;
    }
  }

  /* Snake movement */

  if (!hitApple)
  {
    //Move body
    for(i = snake.amountOfSnakeParts; i > 0; i--)
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

  //Handle key input to set the snakes direction and stop double movement
  bool moved = false;

  // Vertical movement (W and S keys)
  if ((keys['w'] || keys['W']) && !moved || keys['W'] && snake.amountOfSnakeParts == 0)
  {
    // Prevent moving up if currently moving down
    if (snake.direction.y != Down.y && snake.amountOfSnakeParts != 0)
    {
        snake.direction = Up;
        moved = true; 
    }
    else if (snake.amountOfSnakeParts == 0) snake.direction = Up;
  }
  else if ((keys['s'] || keys['S']) && !moved)
  {
    // Prevent moving down if currently moving up
    if (snake.direction.y != Up.y && snake.amountOfSnakeParts != 0)
    {
        snake.direction = Down;
        moved = true; 
    }
    else if (snake.amountOfSnakeParts == 0) snake.direction = Down;
  }

  // Horizontal movement (A and D keys)
  if (!moved || snake.amountOfSnakeParts == 0) // Only allow horizontal movement if no vertical movement happened or if youre on first snake block
  {
    if ((keys['a'] || keys['A']))
    {
      if (snake.direction.x != Right.x && snake.amountOfSnakeParts != 0)
      {
        snake.direction = Left;
        moved = true;
      }
      else if (snake.amountOfSnakeParts == 0) snake.direction = Left;
    }
    else if ((keys['d'] || keys['D']))
    {
      if (snake.direction.x != Left.x && snake.amountOfSnakeParts != 0)
      {
        snake.direction = Right;
        moved = true; 
      }
      else if (snake.amountOfSnakeParts == 0) snake.direction = Right;
    }
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