#define _CRT_SECURE_NO_WARNINGS
//#include <pthread.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <stdio.h>
//#include <iostream>
#include <utility>
#include <string>


std::pair<int, int> disToInt(std::string dis)
{
    char cX, cY;
    cX = dis[0];
    cY = dis[1];
    int iX, iY;
    iX = (int)cX - 'A';
    iY = 7 - ((int)cY - '1');
    return std::make_pair(iY, iX);
}

std::string intToDis(int y, int x)
{
    std::string dis = "";
    dis += (char)x + 'A';
    dis += (char)(7 - y) + '1';
    return dis;
}

void printBoard(char board[8][8])
{
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            printf("|%c", board[j][i]);
        }
        printf("|\n");
    }
}

void initBoard(char board[8][8])
{
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if ((i + j) % 2 == 1)
            {
                board[j][i] = ' ';
                if (j <= 2) board[j][i] = 'b';
                if (j >= 5) board[j][i] = 'w';
            }
            else
            {
                board[j][i] = ' ';
            }
        }
    }
}

void doMove(char board[8][8], std::string move)
{
    char* cMove = new char[move.length() + 1];
    strcpy(cMove, move.c_str());
    char fromY, fromX, toY, toX;
    sscanf(cMove, "%c%cto%c%c", &fromY, &fromX, &toY, &toX);
    std::string from = "";
    std::string to = "";
    from += fromY;
    from += fromX;
    to += toY;
    to += toX;
    std::pair<int, int> pFrom = disToInt(from);
    std::pair<int, int> pTo = disToInt(to);
    char color = board[pFrom.first][pFrom.second];
    board[pFrom.first][pFrom.second] = ' ';
    board[pTo.first][pTo.second] = color;
}

int main()
{
    printf("S :%s\n", "Hello World!\n");
    printf("%d_%d\n", disToInt("H7").first, disToInt("H7").second);
    std::string res = intToDis(1, 1);
    char* cRes = new char[res.length() + 1];
    strcpy(cRes, res.c_str());
    printf("%s\n", cRes);


    char board[8][8];
    initBoard(board);
    printBoard(board);

    doMove(board, "B8toA8");
    printBoard(board);


}