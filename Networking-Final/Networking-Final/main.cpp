#include "raylib.h"
#include "socklib.h"
#include <array>
#include <iostream>
#include <sstream>

const int MAX_ROWS = 6;
const int MAX_COLUMNS = 7;
const int CELL_SIZE = 100;

const char* const HOST = "127.0.0.1";
const int PORT = 8080;

bool isConnected = false;
bool waitingForConnection = false;
bool waitingForResponse = false;
bool resetGame = false;

enum Player { NONE = 0, PLAYER1, PLAYER2 };

// 2d array board
std::array<std::array<Player, MAX_COLUMNS>, MAX_ROWS> board = {};

// Game state vars
Player currentPlayer = PLAYER1;
int selectedColumn = 0;
bool gameOver = false;
Player winner = NONE;
Color pieceColor;

Player you; // host is p1, online is p2

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

bool CheckInput() 
{
	bool inputDetected = false;
    if (!gameOver && currentPlayer == you)
    {
        // move on top
        if (IsKeyPressed(KEY_LEFT))
        {
            selectedColumn = (selectedColumn - 1 + MAX_COLUMNS) % MAX_COLUMNS;
			inputDetected = true;
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            selectedColumn = (selectedColumn + 1) % MAX_COLUMNS;
            inputDetected = true;
        }
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

            waitingForResponse = true;
            inputDetected = true;
        }
    }
    else if(gameOver)
    {
        if (IsKeyPressed(KEY_R))
        {
            // reset game and values
            board = {};
            gameOver = false;
            winner = NONE;
            currentPlayer = PLAYER1;
            selectedColumn = 0;
            inputDetected = true;
        }
    }

    return inputDetected;
}

void DrawBoard() 
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

	if (waitingForConnection) 
    {
		DrawText("Waiting for connection...", 50, 10, 20, BLACK);
		EndDrawing();
        return;
	}
    else if (!isConnected) 
    {
        DrawText("Press 'H' to host", 50, 10, 20, BLACK);
        DrawText("Press 'J' to join", 50, 40, 20, BLACK);
        EndDrawing();
        return;
    }

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
        if (waitingForResponse)
            DrawText("Waiting for opponent...", 50, 10, 20, BLACK);
        else
            DrawText("Your turn!", 50, 10, 20, BLACK);
    }

    EndDrawing();
}

bool ReceiveData(Socket& socket, std::string& data) 
{
	char buffer[1024];
	int bytesReceived = socket.Recv(buffer, sizeof(buffer));

	if (bytesReceived > 0) {
		data.assign(buffer, bytesReceived);
		return true;
	}

	return false;
}

std::string SerializeBoard() 
{
    std::stringstream gameState;

    gameState.write((char*)&currentPlayer, sizeof(Player));
    gameState.write((char*)&waitingForResponse, sizeof(bool));
    gameState.write((char*)&gameOver, sizeof(bool));
    gameState.write((char*)&winner, sizeof(Player));
    gameState.write((char*)&pieceColor, sizeof(Color));
    gameState.write((char*)&selectedColumn, sizeof(int));

    for (int row = 0; row < MAX_ROWS; row++)
        for (int col = 0; col < MAX_COLUMNS; col++)
            gameState.write((char*)&board[row][col], sizeof(Player));

    return gameState.str();
}

void DeserializeBoard(const std::string& data) 
{
    std::stringstream gameState(data);

    gameState.read((char*)&currentPlayer, sizeof(Player));
    gameState.read((char*)&waitingForResponse, sizeof(bool));
    gameState.read((char*)&gameOver, sizeof(bool));
    gameState.read((char*)&winner, sizeof(Player));
    gameState.read((char*)&pieceColor, sizeof(Color));
    gameState.read((char*)&selectedColumn, sizeof(int));

    for (int row = 0; row < MAX_ROWS; row++)
        for (int col = 0; col < MAX_COLUMNS; col++)
            gameState.read((char*)&board[row][col], sizeof(Player));

    waitingForResponse = !waitingForResponse;
}

int RunClient()
{
	SockLibInit();

	Socket clientSock(Socket::Family::INET, Socket::Type::STREAM);

    waitingForConnection = true;
    std::cout << "Joining: Waiting for connection..." << std::endl;

    you = PLAYER2;

    DrawBoard();

    while (!WindowShouldClose()) {
        int err = clientSock.Connect(Address(HOST), PORT);
        if (err != 0) {
            std::cerr << "Failed to connect to server: " << err << std::endl;
        }

		isConnected = true;
		waitingForConnection = false;
        waitingForResponse = true;
        std::cout << "Joining: Connected to host!" << std::endl;

        DrawBoard();

        while (isConnected) {
            if (!waitingForResponse) {
                // your turn
                if (CheckInput()) {
                    std::string boardState = SerializeBoard();
                    clientSock.Send(boardState.data(), boardState.size());
                }
            }
            else {
				// opponent's turn
                std::string gameState;
                if (ReceiveData(clientSock, gameState)); {
                    DeserializeBoard(gameState);
                }
            }

            DrawBoard();
        }
    }

	SockLibShutdown();
    return 0;
}

int RunHost() 
{
    SockLibInit();

	Socket hostSock(Socket::Family::INET, Socket::Type::STREAM);
    hostSock.Bind(Address(HOST), PORT);

	waitingForConnection = true;
    std::cout << "Hosting: Waiting for connection..." << std::endl;

    you = PLAYER1;

    DrawBoard();

	hostSock.Listen();

    while (true) {
		Socket clientSock = hostSock.Accept();
        isConnected = true;
		waitingForConnection = false;
        waitingForResponse = false;
		std::cout << "Hosting: Connected to client!" << std::endl;

        while (isConnected) {
            if (!waitingForResponse) {
                // your turn
                if (CheckInput()) {
                    std::string boardState = SerializeBoard();
                    clientSock.Send(boardState.data(), boardState.size());
                }
			}
            else {
                // opponent's turn
                std::string gameState;
                if (ReceiveData(clientSock, gameState)) {
                    DeserializeBoard(gameState);
                }
            }

            DrawBoard();

            if (IsKeyPressed(KEY_ESCAPE))
                return 0;
        }
    }

    SockLibShutdown();
    return 0;
}

int main()
{
    int screenWidth = MAX_COLUMNS * CELL_SIZE;
    int screenHeight = (MAX_ROWS + 1) * CELL_SIZE;

    InitWindow(screenWidth, screenHeight, "connect 4 tuah!");

    SetTargetFPS(60);

    pieceColor = RED;

	std::cout << "Press 'H' to host or 'J' to join" << std::endl;

    while (!WindowShouldClose())
    {
		DrawBoard();

		if (IsKeyPressed(KEY_H)) {
			RunHost();
		}
		else if (IsKeyPressed(KEY_J)) {
			RunClient();
		}
    }

    CloseWindow();
    return 0;
}
