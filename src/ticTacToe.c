#define SIDE 3
#include <stdio.h>
#include <stdlib.h>

void initialise(char board[][SIDE], int moves[])
{
    for (int i=0; i<SIDE; i++)
    {
        for (int j=0; j<SIDE; j++)
            board[i][j] = ' ';
    }

    // Fill the moves with numbers
    for (int i=0; i<SIDE*SIDE; i++)
        moves[i] = i;

}

void showInstructions()
{
    printf("\t\t\t  Tic-Tac-Toe\n\n");
    printf("Choose a cell numbered from 1 to 9 as below"
           " and play\n\n");

    printf("\t\t\t  1 | 2  | 3  \n");
    printf("\t\t\t--------------\n");
    printf("\t\t\t  4 | 5  | 6  \n");
    printf("\t\t\t--------------\n");
    printf("\t\t\t  7 | 8  | 9  \n\n");

    printf("-\t-\t-\t-\t-\t-\t-\t-\t-\t-\n\n");

    return;
}

void DrawBoard(char board[SIDE][SIDE])
{
    printf("\n\n");

    printf("\t\t\t  %c | %c  | %c  \n", board[0][0],
           board[0][1], board[0][2]);
    printf("\t\t\t--------------\n");
    printf("\t\t\t  %c | %c  | %c  \n", board[1][0],
           board[1][1], board[1][2]);
    printf("\t\t\t--------------\n");
    printf("\t\t\t  %c | %c  | %c  \n\n", board[2][0],
           board[2][1], board[2][2]);

    return;
}

bool rowCrossed(char board[][SIDE])
{
    for (int i=0; i<SIDE; i++)
    {
        if (board[i][0] == board[i][1] &&
            board[i][1] == board[i][2] &&
            board[i][0] != ' ')
            return (true);
    }
    return(false);
}


bool columnCrossed(char board[][SIDE])
{
    for (int i=0; i<SIDE; i++)
    {
        if (board[0][i] == board[1][i] &&
            board[1][i] == board[2][i] &&
            board[0][i] != ' ')
            return (true);
    }
    return(false);
}

int PositionCalculation(char board[][SIDE], int selection, char input)
{
    int column = selection % 3;
    int row = selection / 3;

    if (column == 0)
    {
        column += 3;
        row -= 1;
    }

    if (board[row][column - 1] == 'o' || board[row][column - 1] == 'x')
        return -1;

    board[row][column - 1] = input;
    return 0;
}

bool diagonalCrossed(char board[][SIDE])
{
    if (board[0][0] == board[1][1] &&
        board[1][1] == board[2][2] &&
        board[0][0] != ' ')
        return(true);

    if (board[0][2] == board[1][1] &&
        board[1][1] == board[2][0] &&
        board[0][2] != ' ')
        return(true);

    return(false);
}

bool cellEmpty(char board[][SIDE], int selection)
{
    int column = selection % 3;
    int row = selection / 3;
    if (board[row][column - 1] == ' ')
        return true;
    else return false;
}

bool gameOver(char board[][SIDE])
{
    return(rowCrossed(board) || columnCrossed(board)
           || diagonalCrossed(board) );
}
void declareWinner(int moveIndex)
{
    if (moveIndex % 2 != 0)
        printf("Player1 has won\n");
    else
        printf("Player2 has won\n");
}
void gameEnd(int moveIndex, char board[][SIDE])
{
    if (gameOver(board) == false &&
        moveIndex == SIDE * SIDE)
        printf("It's a draw\n");
    else
    {
        declareWinner(moveIndex);
    }
}