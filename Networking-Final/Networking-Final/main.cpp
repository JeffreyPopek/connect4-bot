#include "raylib.h"
#include <array>
#include <iostream>

const int MAX_ROWS = 6;
const int MAX_COLUMNS = 7;
const int CELL_SIZE = 100;

enum Player { NONE = 0, PLAYER1, PLAYER2 };

// 2d array board
std::array<std::array<Player, MAX_COLUMNS>, MAX_ROWS> board = {};

// Game state vars
Player currentPlayer = PLAYER1;
int selectedColumn = 0;
bool gameOver = false;
Player winner = NONE;
Color pieceColor;

bool DropPiece(int col, Player player)
{
    for (int row = MAX_ROWS - 1; row >= 0; --row)
    {
        if (board[row][col] == NONE)
        {
            // set board position to player
            board[row][col] = player;
            return true;
        }
    }
    return false;

}

bool CheckWin(Player player)
{
    /*
        check for each piece across
        in each row look across columns to find 4 same colored pieces in a row
        check each row
    */
    for (int row = 0; row < MAX_ROWS; row++)
        for (int col = 0; col <= MAX_COLUMNS - 4; col++)
            if (board[row][col] == player && board[row][col + 1] == player && board[row][col + 2] == player && board[row][col + 3] == player)
                return true;


    /*
        check for each piece vert
        in each column look down rows to find 4 same colored pieces in a column
        check each column
    */
    for (int col = 0; col < MAX_COLUMNS; col++)
        for (int row = 0; row <= MAX_ROWS - 4; row++)
            if (board[row][col] == player && board[row + 1][col] == player && board[row + 2][col] == player && board[row + 3][col] == player)
                return true;


    // Diagonal 
    for (int row = 3; row < MAX_ROWS; row++)
        for (int col = 0; col <= MAX_COLUMNS - 4; col++)
            if (board[row][col] == player && board[row - 1][col + 1] == player && board[row - 2][col + 2] == player && board[row - 3][col + 3] == player)
                return true;

    for (int row = 0; row <= MAX_ROWS - 4; row++)
        for (int col = 0; col <= MAX_COLUMNS - 4; col++)
            if (board[row][col] == player && board[row + 1][col + 1] == player && board[row + 2][col + 2] == player && board[row + 3][col + 3] == player)
                return true;


    // switch player if not a win
    switch (player)
    {
    case NONE:
        break;
    case PLAYER1:
        currentPlayer = PLAYER2;
        pieceColor = YELLOW;
        break;
    case PLAYER2:
        currentPlayer = PLAYER1;
        pieceColor = RED;
        break;
    default:
        break;
    }

    return false;
}

bool IsBoardFull()
{
    for (int row = 0; row < MAX_ROWS; ++row)
    {
        for (int col = 0; col < MAX_COLUMNS; ++col)
        {
            if (board[row][col] == NONE)
                return false;
        }
    }
    return true;
}


int main()
{
    int screenWidth = MAX_COLUMNS * CELL_SIZE;
    int screenHeight = (MAX_ROWS + 1) * CELL_SIZE;

    InitWindow(screenWidth, screenHeight, "connect 4 tuah!");

    SetTargetFPS(60);

    pieceColor = RED;

    while (!WindowShouldClose())
    {
        if (!gameOver)
        {
            // move on top
            if (IsKeyPressed(KEY_LEFT))
                selectedColumn = (selectedColumn - 1 + MAX_COLUMNS) % MAX_COLUMNS;

            else if (IsKeyPressed(KEY_RIGHT))
                selectedColumn = (selectedColumn + 1) % MAX_COLUMNS;

            // place piece
            else if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
            {
                // drop piece in column
                if (DropPiece(selectedColumn, currentPlayer))
                {
                    // check for player win based on player and switch player
                    if (CheckWin(currentPlayer))
                    {
                        gameOver = true;
                        winner = currentPlayer;
                        pieceColor = LIGHTGRAY; // make piece color same color as bg to hide
                    }
                    else if (IsBoardFull())
                    {
                        std::cout << "GAME TIED" << std::endl;
                        gameOver = true;
                        winner = NONE;
                    }
                }
            }
        }
        else
        {
            if (IsKeyPressed(KEY_R))
            {
                // reset game and values
                board = {};
                gameOver = false;
                winner = NONE;
                currentPlayer = PLAYER1;
                selectedColumn = 0;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // selection
        for (int col = 0; col < MAX_COLUMNS; col++)
        {
            if (col == selectedColumn)
                DrawRectangle(col * CELL_SIZE, 0, CELL_SIZE, CELL_SIZE, pieceColor);
            else
                DrawRectangle(col * CELL_SIZE, 0, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
        }

        // Draw board
        for (int row = 0; row < MAX_ROWS; row++)
        {
            for (int col = 0; col < MAX_COLUMNS; col++)
            {
                int x = col * CELL_SIZE + CELL_SIZE / 2;
                int y = (row + 1) * CELL_SIZE + CELL_SIZE / 2;

                Color color = LIGHTGRAY;

                if (board[row][col] == PLAYER1)
                    color = RED;
                else if (board[row][col] == PLAYER2)
                    color = YELLOW;

                DrawCircle(x, y, CELL_SIZE / 2 - 10, color);
            }
        }

        // draw game state messages
        if (gameOver)
        {
            switch (winner)
            {
            case NONE:
                DrawText("Draw?! Press R to restart", 50, 10, 20, BLACK);
                break;
            case PLAYER1:
                DrawText("Player 1 Wins! Press R to restart", 50, 10, 20, RED);
                break;
            case PLAYER2:
                DrawText("Player 2 Wins! Press R to restart", 50, 10, 20, YELLOW);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (currentPlayer)
            {
            case NONE:
                // fortnite battle pass 19 dolalrs
                break;
            case PLAYER1:
                DrawText("Player 1's Turn (Red)", 50, 10, 20, BLACK);
                break;
            case PLAYER2:
                DrawText("Player 2's Turn (Yellow)", 50, 10, 20, BLACK);
                break;
            default:
                break;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}