//#define _CRT_SECURE_NO_WARNINGS
 #include <pthread.h>
#include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 #include <unistd.h>
#include <stdio.h>
 #include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <ncurses.h>


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

std::pair<std::pair<int, int>, std::pair<int, int>> mvToPP(std::string move)
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
    return std::make_pair(pFrom, pTo);
}

void printBoard(char board[8][8], int y, int x)
{
    for (int j = 0; j < 8; j++)
    {
        move(j+3,0);
        for (int i = 0; i < 8; i++)
        {
            if (j == y && i == x)
            {
                switch (board[j][i])
                {
                case 'w':
                    printw("|W");
                    break;
                case 'v':
                    printw("|V");
                    break;
                case 'b':
                    printw("|B");
                    break;
                case 'a':
                    printw("|A");
                    break;
                case ' ':
                    printw("|.");
                    break;
                default:
                    break;
                }
            }
            else
            {
                printw("|%c", board[j][i]);
            }
            
        }
        printw("|");
    }
}

void printScreen(char board[8][8], bool white, int y, int x)
{
    clear();
    move(0,4);
    printw("Draughts\n");
    move(1,0);
    if (white)
    {
        printw("%s", "White's turn:");
    }
    else
    {
        printw("%s", "Black's turn:");
    }
    printBoard(board, y, x);
    move(12,0);
    refresh();
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
    std::pair<std::pair<int, int>, std::pair<int, int>> ppMove = mvToPP(move);
    std::pair<int, int> pFrom = ppMove.first;
    std::pair<int, int> pTo = ppMove.second;
    char color = board[pFrom.first][pFrom.second];
    board[pFrom.first][pFrom.second] = ' ';
    board[pTo.first][pTo.second] = color;
}

bool checkMove(char board[8][8], std::string move, bool white)
{
    std::pair<std::pair<int, int>, std::pair<int, int>> ppMove = mvToPP(move);
    std::pair<int, int> pFrom = ppMove.first;
    std::pair<int, int> pTo = ppMove.second;
    if (pFrom.first < 0 || pFrom.first>7 || pFrom.second < 0 || pFrom.second>7) return false;
    if (pTo.first < 0 || pTo.first>7 || pTo.second < 0 || pTo.second>7) return false;
    if (white)
    {
        if ((board[pFrom.first][pFrom.second] != 'w') && (board[pFrom.first][pFrom.second] != 'v')) return false;
    }
    else
    {
        if ((board[pFrom.first][pFrom.second] != 'b') && (board[pFrom.first][pFrom.second] != 'a')) return false;
    }
    if (board[pTo.first][pTo.second] != ' ') return false;
    switch (board[pFrom.first][pFrom.second])
    {
    case 'w':
        if ((pTo.first == pFrom.first - 1) &&
            ((pTo.second == pFrom.second - 1) || (pTo.second == pFrom.second + 1))) return true;
        else return false;
        break;
    case 'b':
        if ((pTo.first == pFrom.first + 1) &&
            ((pTo.second == pFrom.second - 1) || (pTo.second == pFrom.second + 1))) return true;
        else return false;
        break;
    case 'v':
    case 'a':
        if (((pTo.first == pFrom.first - 1) || (pTo.first == pFrom.first + 1)) &&
            ((pTo.second == pFrom.second - 1) || (pTo.second == pFrom.second + 1))) return true;
        else return false;
        break;
    default:
        return false;
        break;
    }
}

std::vector<std::pair<int, int>> findPieces(char board[8][8], bool white)
{
    std::vector<std::pair<int, int>> pieces;
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (white)
            {
                if (board[j][i] == 'w' || board[j][i] == 'v')
                {
                    pieces.push_back(std::make_pair(j, i));
                }
            }
            else
            {
                if (board[j][i] == 'b' || board[j][i] == 'a')
                {
                    pieces.push_back(std::make_pair(j, i));
                }
            }
        }
    }
    return pieces;
}

std::vector<std::string> findCaptures(char board[8][8], int y, int x)
{
    std::vector<std::string> captures;
    switch (board[y][x])
    {
    case 'w':
        if (y >= 2)
        {
            if (x >= 2)
            {
                if ((board[y - 1][x - 1] == 'b' || board[y - 1][x - 1] == 'a') &&
                    (board[y - 2][x - 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y - 2, x - 2));
            }
            if (x <= 5)
            {
                if ((board[y - 1][x + 1] == 'b' || board[y - 1][x + 1] == 'a') &&
                    (board[y - 2][x + 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y - 2, x + 2));
            }
        }
        break;
    case 'b':
        if (y <= 5)
        {
            if (x >= 2)
            {
                if ((board[y + 1][x - 1] == 'w' || board[y + 1][x - 1] == 'v') &&
                    (board[y + 2][x - 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y + 2, x - 2));
            }
            if (x <= 5)
            {
                if ((board[y + 1][x + 1] == 'w' || board[y + 1][x + 1] == 'v') &&
                    (board[y + 2][x + 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y + 2, x + 2));
            }
        }
        break;
    case 'v':
        if (y >= 2)
        {
            if (x >= 2)
            {
                if ((board[y - 1][x - 1] == 'b' || board[y - 1][x - 1] == 'a') &&
                    (board[y - 2][x - 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y - 2, x - 2));
            }
            if (x <= 5)
            {
                if ((board[y - 1][x + 1] == 'b' || board[y - 1][x + 1] == 'a') &&
                    (board[y - 2][x + 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y - 2, x + 2));
            }
        }
        if (y <= 5)
        {
            if (x >= 2)
            {
                if ((board[y + 1][x - 1] == 'b' || board[y + 1][x - 1] == 'a') &&
                    (board[y + 2][x - 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y + 2, x - 2));
            }
            if (x <= 5)
            {
                if ((board[y + 1][x + 1] == 'b' || board[y + 1][x + 1] == 'a') &&
                    (board[y + 2][x + 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y + 2, x + 2));
            }
        }
        break;
    case 'a':
        if (y >= 2)
        {
            if (x >= 2)
            {
                if ((board[y - 1][x - 1] == 'w' || board[y - 1][x - 1] == 'v') &&
                    (board[y - 2][x - 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y - 2, x - 2));
            }
            if (x <= 5)
            {
                if ((board[y - 1][x + 1] == 'w' || board[y - 1][x + 1] == 'v') &&
                    (board[y - 2][x + 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y - 2, x + 2));
            }
        }
        if (y <= 5)
        {
            if (x >= 2)
            {
                if ((board[y + 1][x - 1] == 'w' || board[y + 1][x - 1] == 'v') &&
                    (board[y + 2][x - 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y + 2, x - 2));
            }
            if (x <= 5)
            {
                if ((board[y + 1][x + 1] == 'w' || board[y + 1][x + 1] == 'v') &&
                    (board[y + 2][x + 2] == ' '))
                    captures.push_back(intToDis(y, x) + "to" + intToDis(y + 2, x + 2));
            }
        }
        break;
    default:
        break;
    }
    return captures;
}

int main()
{
    initscr();
    
    //printf("%d_%d\n", disToInt("H7").first, disToInt("H7").second);
    //std::string res = intToDis(1, 1);
    //char* cRes = new char[res.length() + 1];
    //strcpy(cRes, res.c_str());
    //printf("%s\n", cRes);


    char board[8][8];
    initBoard(board);
    int posY = 0, posX = 0;
    //printBoard(board, posY, posX);
    

    bool white = true;
    while (true)
    {
        printScreen(board, white, posY, posX);

        //char mv[10];
        //scanf("%s", mv);
        //std::string sMv(mv);

        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        int countPos = 0;
        std::string sMv = "";
        while (countPos < 2)
        {
            int key;
            clrtoeol();
            key = getch();
            switch (key)
            {
            case KEY_UP:
                posY--;
                break;
            case KEY_DOWN   :
                posY++;
                break;
            case KEY_LEFT:
                posX--;
                break;
            case KEY_RIGHT:
                posX++;
                break;
            case 10:
                sMv += intToDis(posY, posX);
                if (countPos == 0)
                {
                    sMv += "to";
                }
                countPos++;
                break;
            default:
               break;
            }
            if (posY < 0) posY = 0;
            if (posY > 7) posY = 7;
            if (posX < 0) posX = 0;
            if (posX > 7) posX = 7;
            printScreen(board, white, posY, posX);
            printw("%s\n", sMv.c_str());
            refresh();
        }
        nocbreak();

        std::vector<std::pair<int, int>> pieces = findPieces(board, white);
        std::vector<std::string> captures;
        for (std::pair<int, int> i : pieces)
        {
            std::vector<std::string> newCaptures = findCaptures(board, i.first, i.second);
            captures.insert(captures.end(), newCaptures.begin(), newCaptures.end());
        }
        if (captures.size() > 0)
        {
            printw("Captures: %d\n", captures.size());
            printw("%s\n", captures[0].c_str());
            refresh();
            refresh();
            getch();
            if (std::find(captures.begin(), captures.end(), sMv) != captures.end())
            {
                doMove(board, sMv);
                std::pair<std::pair<int, int>, std::pair<int, int>> ppMv = mvToPP(sMv);
                board[(ppMv.first.first + ppMv.second.first) / 2][(ppMv.first.second + ppMv.second.second) / 2] = ' ';
                std::vector<std::string> nextCaptures = findCaptures(board, ppMv.second.first, ppMv.second.second);
                if (nextCaptures.size() > 0) white = !white;
            }
            else
            {
                if (white) printw("%s\n", "White loose");
                else printw("%s\n", "Black loose");
                break;
            }
        }
        else
        {
            if (checkMove(board, sMv, white))
            {
                doMove(board, sMv);
            }
            else
            {
                if (white) printw("%s\n", "White loose");
                else printw("%s\n", "Black loose");
                break;
            }
        }
        white = !white;
        clear();
    }

    refresh();
    getch();
    endwin();

    //doMove(board, "B8toC8");
    //printBoard(board);
    //if (checkMove(board, "C3toD4", true)) printf("si\n"); else printf("ni\n");
    //printf("%d\n", findPieces(board, true).size());
    //doMove(board, "B6toB4");
    //printBoard(board);
    //std::vector<std::pair<int, int>> pieces = findPieces(board, true);
    //std::vector<std::string> captures;
    //for (std::pair<int, int> i : pieces)
    //{
    //    std::vector<std::string> newCaptures = findCaptures(board, i.first, i.second);
    //    captures.insert(captures.end(), newCaptures.begin(), newCaptures.end());
    //}
    ////printf("%d\n", captures.size());
    //char mv[10];
    //scanf("%s", &mv);
    //std::string sMv(mv);
    //if(!sMv.compare("C5toD5")) printf("si\n");
    //char* cMv = new char[sMv.length() + 1];
    //strcpy(cMv, sMv.c_str());
    //printf(":%s\n", cMv);

}