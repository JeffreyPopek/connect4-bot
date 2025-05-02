#include "raylib.h"
#include <array>
#include <iostream>
#include <limits>
#include <vector>

/*
 Credit to this blog: http://blog.gamesolver.org/
 Helped me with implementation and understanding of minimax
*/

const int MAX_ROWS = 6;
const int MAX_COLUMNS = 7;
const int CELL_SIZE = 100;
const int MAX_DEPTH = 5; // limit depth for mini max search

enum Player { NONE = 0, PLAYER1, PLAYER2 };

std::array<std::array<Player, MAX_COLUMNS>, MAX_ROWS> board = {};

Player currentPlayer = PLAYER1;
int selectedColumn = 0;
bool gameOver = false;
Player winner = NONE;
Color pieceColor;

bool DropPiece(int column, Player player)
{
    for (int row = MAX_ROWS - 1; row >= 0; row--)
    {
        if (board[row][column] == NONE)
        {
            board[row][column] = player;
            return true;
        }
    }
    return false;
}

bool CheckWin(Player player)
{
    // check horizontal
    for (int row = 0; row < MAX_ROWS; row++)
        for (int column = 0; column <= MAX_COLUMNS - 4; column++)
            if (board[row][column] == player && board[row][column + 1] == player && board[row][column + 2] == player && board[row][column + 3] == player)
                return true;

    // check vertical
    for (int column = 0; column < MAX_COLUMNS; column++)
        for (int row = 0; row <= MAX_ROWS - 4; row++)
            if (board[row][column] == player && board[row + 1][column] == player && board[row + 2][column] == player && board[row + 3][column] == player)
                return true;

    // check diagonals 
    for (int row = 3; row < MAX_ROWS; row++)
        for (int column = 0; column <= MAX_COLUMNS - 4; column++)
            if (board[row][column] == player && board[row - 1][column + 1] == player && board[row - 2][column + 2] == player && board[row - 3][column + 3] == player)
                return true;

    for (int row = 0; row <= MAX_ROWS - 4; row++)
        for (int col = 0; col <= MAX_COLUMNS - 4; col++)
            if (board[row][col] == player && board[row + 1][col + 1] == player && board[row + 2][col + 2] == player && board[row + 3][col + 3] == player)
                return true;

    return false;
}

bool IsBoardFull()
{
    for (int row = 0; row < MAX_ROWS; row++)
        for (int column = 0; column < MAX_COLUMNS; column++)
            if (board[row][column] == NONE)
                return false;
    return true;
}


#pragma region mini max

void UndoDrop(int col)
{
    // used in min max checking, resets last move
    for (int row = 0; row < MAX_ROWS; ++row)
    {
        if (board[row][col] != NONE)
        {
            board[row][col] = NONE;
            return;
        }
    }
}

int EvaluateBoard()
{
    if (CheckWin(PLAYER1))
        return -1;

    if (CheckWin(PLAYER2))
        return 1;

    // no changes
    return 0;
}

int Minimax(int depth, bool max)
{
    int score = EvaluateBoard();

    // if game over, can't go deeper, or full board then exit
    if (score != 0 || depth == 0 || IsBoardFull())
        return score;

    if (max)
    {
        int best = -9999;
        for (int column = 0; column < MAX_COLUMNS; column++)
        {
            if (board[0][column] == NONE)
            {
                DropPiece(column, PLAYER2);
                best = std::max(best, Minimax(depth - 1, false));
                UndoDrop(column);
            }
        }
        return best;
    }
    else
    {
        int best = 9999;
        for (int column = 0; column < MAX_COLUMNS; column++)
        {
            if (board[0][column] == NONE)
            {
                DropPiece(column, PLAYER1);
                best = std::min(best, Minimax(depth - 1, true));
                UndoDrop(column);
            }
        }
        return best;
    }
}

int GetBestMove()
{
    int bestScore = -9999;
    int best = 0;

    for (int column = 0; column < MAX_COLUMNS; column++)
    {
        if (board[0][column] == NONE)
        {
            /*
            place piece
            evaluate board
            undo drop
            if move is good then play it
            */
            DropPiece(column, PLAYER2);
            int score = Minimax(MAX_DEPTH, false);
            UndoDrop(column);
            if (score > bestScore)
            {
                bestScore = score;
                best = column;
            }
        }
    }
    return best;
}

#pragma endregion


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
            if (currentPlayer == PLAYER1)
            {
                if (IsKeyPressed(KEY_LEFT))
                    selectedColumn = (selectedColumn - 1 + MAX_COLUMNS) % MAX_COLUMNS;
                else if (IsKeyPressed(KEY_RIGHT))
                    selectedColumn = (selectedColumn + 1) % MAX_COLUMNS;
                else if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
                {
                    if (DropPiece(selectedColumn, PLAYER1))
                    {
                        if (CheckWin(PLAYER1))
                        {
                            gameOver = true;
                            winner = PLAYER1;
                            pieceColor = LIGHTGRAY;
                        }
                        else if (IsBoardFull())
                        {
                            gameOver = true;
                            winner = NONE;
                        }
                        else
                        {
                            currentPlayer = PLAYER2;
                            pieceColor = YELLOW;
                        }
                    }
                }
            }
            else if (currentPlayer == PLAYER2) 
            {
                int aiMove = GetBestMove();
                if (DropPiece(aiMove, PLAYER2))
                {
                    if (CheckWin(PLAYER2))
                    {
                        gameOver = true;
                        winner = PLAYER2;
                        pieceColor = LIGHTGRAY;
                    }
                    else if (IsBoardFull())
                    {
                        gameOver = true;
                        winner = NONE;
                    }
                    else
                    {
                        currentPlayer = PLAYER1;
                        pieceColor = RED;
                    }
                }
            }
        }
        else if (IsKeyPressed(KEY_R))
        {
            /*
            reset board, game over status, winner, set human's turn, reset piece color
            */
            board = {};
            gameOver = false;
            winner = NONE;
            currentPlayer = PLAYER1;
            selectedColumn = 0;
            pieceColor = RED;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // selection highlight
        for (int col = 0; col < MAX_COLUMNS; col++)
        {
            if (col == selectedColumn)
                DrawRectangle(col * CELL_SIZE, 0, CELL_SIZE, CELL_SIZE, pieceColor);
            else
                DrawRectangle(col * CELL_SIZE, 0, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
        }

        // draw board and pieces
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
                case PLAYER1:
                    DrawText("Player 1's Turn (Red)", 50, 10, 20, BLACK);
                    break;
                case PLAYER2:
                    DrawText("AI's turn (yellow)", 50, 10, 20, BLACK);
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
