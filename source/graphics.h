#define WINDOW_W 700
#define WINDOW_H 700

/* Some structs to define some future variables */

#define UP {0, -1}
#define DOWN {0, 1}
#define LEFT {-1, 0}
#define RIGHT {1, 0}

#define EMPTY {-1, -1}

typedef struct
{
    int x;
    int y;
} Vector2;

typedef struct
{
    Vector2 direction;
    Vector2 positions[WINDOW_W * WINDOW_H]; //Change to account for pixels in grid instead of running on straight pixels of resolution size
    int amountOfSnakeParts;
} Snake;

/* Math functions relating to the graphics */

extern Vector2 emptyPos;

void InsertPosition(Vector2 positions[], Vector2 insertedPosition, int index, int totalPositions)
{
    Vector2 lastPos = insertedPosition;

    for (int i = index; i < totalPositions; i++)
    {
        // Check if the current position is empty
        if (positions[i].x == emptyPos.x && positions[i].y == emptyPos.y)
        {
            // If an empty spot is found, insert the last position here
            positions[i] = lastPos;

            break;
        }
        else
        {
            // If not empty, swap the positions to shift them
            Vector2 temp = positions[i];
            positions[i] = lastPos;
            lastPos = temp;
        }
    }
}

int RandomInt(int min, int max) 
{
    int out = min + (rand() % (max - min + 1));
    
    return out;
}

Vector2 RandomVector2(Vector2 min, Vector2 max)
{
    int x = RandomInt(min.x, max.x);
    int y = RandomInt(min.y, max.y);

    Vector2 out = {x, y};

    return out;
}

//Gets a random Vector2 within the screen with a certain padding and grid size
Vector2 RandomWindowPosition(int padding, int gridSize)
{
    if (gridSize <= 0) {
        printf("Invalid grid size! Grid size must be greater than 0.\n");
        
        return emptyPos;
    }

    int intMin = padding; //Adjust padding to be the minimum boundary
    int intMaxW = WINDOW_W - padding; //Maximum x coordinate with padding
    int intMaxH = WINDOW_H - padding; //Maximum y coordinate with padding

    //Find the number of grid cells that fit within the window bounds
    int gridCellsX = (intMaxW - intMin) / gridSize;
    int gridCellsY = (intMaxH - intMin) / gridSize;

    //Generate random grid cell positions
    int randomX = (rand() % (gridCellsX + 1)) * gridSize + intMin;
    int randomY = (rand() % (gridCellsY + 1)) * gridSize + intMin;

    //Make sure the random position does not exceed the window bounds
    if (randomX >= intMaxW) randomX = intMaxW - gridSize;
    if (randomY >= intMaxH) randomY = intMaxH - gridSize;

    Vector2 randomPos = {randomX, randomY};

    return randomPos;
}

//Recursive function to get an apple spawn that isnt overlapping with the snake
Vector2 RandomAppleSpawn(int padding, int gridSize, int positionSize, Vector2 positions[])
{
    Vector2 randomPos = RandomWindowPosition(padding, gridSize);

    int i = 0;
    for(i = 0; i <= positionSize; i++)
    {
        if(positions[i].x == randomPos.x && positions[i].y == randomPos.y)
        {
            return RandomAppleSpawn(padding, gridSize, positionSize, positions);
        }
    }

    return randomPos;
}