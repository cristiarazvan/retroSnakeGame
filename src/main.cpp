#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <deque>

using namespace std;

Color green = {201, 235, 52, 255};
Color darkGreen = {32, 41, 13, 255};
// Color foodColor = {255, 0, 0, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 el, deque<Vector2> d)
{
    for(auto i : d)
    {
        if(Vector2Equals(el, i))
            return true;
    }
    return false;
}


bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Food
{

    public:
    
        Vector2 pos;
        Texture2D texture;
        Food(deque<Vector2> body)
        {
            Image image = LoadImage("Images/donut.png");
            texture = LoadTextureFromImage(image);
            pos = GenerateRandomPos(body);
            UnloadImage(image);
        }
        ~Food()
        {
            UnloadTexture(texture);

        }

        void Draw()
        {
            DrawTexture(texture, offset + pos.x * cellSize, offset + pos.y * cellSize, WHITE);
        }
        Vector2 GenerateRandomPos(deque<Vector2> body)
        {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            Vector2 newPos = {x, y};
            while(ElementInDeque(newPos, body))
            {
                float x = GetRandomValue(0, cellCount - 1);
                float y = GetRandomValue(0, cellCount - 1);
                newPos = {x, y};
            }
            return newPos;
        }
};

class Snake
{

public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 dir = {1, 0};
    
    bool addCell = false;


    void Draw()
    {
        for(unsigned int i = 0; i < (unsigned int)body.size(); ++i)
        {

            Rectangle rec = Rectangle{offset + body[i].x * cellSize,offset + body[i].y * cellSize, cellSize, cellSize};
            DrawRectangleRounded(rec, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        //update snake slower
        if(EventTriggered(0.2))
        {
            if(addCell == true)
            {
                body.push_front(Vector2Add(body[0], dir));
                addCell = false;
            }
            else
            {
                body.pop_back();
                body.push_front(Vector2Add(body[0], dir));
            }
        }
    }
    void Reset()
    {
         body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
         dir = {1, 0};
    }
};


class Game
{
public:
    Snake mySnake = Snake();
    Food food = Food(mySnake.body);

    int score = 0;

    bool running = true;

    void Draw()
    {
        food.Draw();
        mySnake.Draw();
    }

    void Update()
    {
        if(running)
        {
            mySnake.Update();
            CheckFoodCollision();
            CheckEdgesCollision();
            CheckTailCollision();
        }
    }

    void CheckFoodCollision()
    {

        if(Vector2Equals(mySnake.body[0], food.pos))
        {
            food.pos = food.GenerateRandomPos(mySnake.body);
            mySnake.addCell = true;
            score++;
        }

    }

    void CheckEdgesCollision()
    {
        if(mySnake.body[0].x == cellCount || mySnake.body[0].x == -1 || mySnake.body[0].y == cellCount || mySnake.body[0].y == -1)
        {
            GameOver();
        }
   
    }

    void CheckTailCollision()
    {

        deque<Vector2> headless = mySnake.body;
        headless.pop_front();
        if(ElementInDeque(mySnake.body[0], headless))
        {
            GameOver();
        }

    }

    void GameOver()
    {
       mySnake.Reset(); 
       food.pos = food.GenerateRandomPos(mySnake.body);
       running = false;
       score = 0;
    }

};


int main()
{
    cout << "Starting...";


    InitWindow(2 * offset  + cellSize * cellCount,2 * offset + cellSize * cellCount, "Retro Snake Game");
    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose() == false)
    {

        BeginDrawing();

        game.Update();
        //Events
        if(IsKeyPressed(KEY_UP) && game.mySnake.dir.y != 1)
        {
            game.mySnake.dir = {0, -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.mySnake.dir.y != -1)
        {
            game.mySnake.dir = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.mySnake.dir.x != 1)
        {
            game.mySnake.dir = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.mySnake.dir.x != -1)
        {
            game.mySnake.dir = {1, 0};
            game.running = true;
        }
       
        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5,(float) offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
        DrawText("Retro Snake Game", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score),cellCount * cellSize + offset - 20, 20, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    
    CloseWindow();
    return 0;
}