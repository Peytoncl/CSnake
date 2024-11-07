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
    Vector2 positions[WINDOW_W * WINDOW_H];
} Snake;

/* Math functions relating to the graphics */

extern Vector2 emptyPos;

void InsertPosition(Vector2 positions[], Vector2 insertedPosition, int index, int totalPositions)
{
    // Ensure index is within bounds
    if (index < 0 || index >= totalPositions)
    {
        printf("Invalid index for insertion!\n");
        return;
    }

    Vector2 lastPos = insertedPosition;

    for (int i = index; i < totalPositions; i++)
    {
        // Check if the current position is empty
        if (positions[i].x == emptyPos.x && positions[i].y == emptyPos.y)
        {
            // If an empty spot is found, insert the position here
            positions[i] = lastPos;
            printf("Inserted position at index %d: (%f, %f)\n", i, insertedPosition.x, insertedPosition.y);
            return;  // Exit after successful insertion
        }
        else
        {
            // If not empty, swap the positions to shift them
            Vector2 temp = positions[i];
            positions[i] = lastPos;
            lastPos = temp;
        }
    }

    // If we get through the entire loop, it means the array was full, and no empty spot was found
    printf("Array full, unable to insert position.\n");
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

Vector2 RandomWindowPosition(int padding, int gridSize)
{
    if (gridSize <= 0) {
        printf("Invalid grid size! Grid size must be greater than 0.\n");
        return (Vector2){0, 0}; // Return default invalid position
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