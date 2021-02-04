
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
//#include <ncurses.h>


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

void kingsRow(char board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        if (board[0][i] == 'w') board[0][i] = 'v';
        if (board[7][i] == 'b') board[7][i] = 'a';
    }
}

struct thread_data_t
{
    int desc_1;
    int desc_2;
};

void *gameThread(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;

    char sendBuff[10];
    char recvBuff[10+1];

    char board[8][8];
    initBoard(board);
    int countMoves = 0;
    bool white = false;

    int white_fd = (*th_data).desc_1;
    int black_fd = (*th_data).desc_2;
    int curr_fd, wait_fd;
    sprintf(sendBuff,"%s", "white_");
    //printf("sending:%s\n", sendBuff);
    send(white_fd,sendBuff,strlen(sendBuff)+1,0);
    sprintf(sendBuff,"%s", "black_");
    //printf("sending:%s\n", sendBuff);
    send(black_fd,sendBuff,strlen(sendBuff)+1,0);
    sprintf(sendBuff,"%s", "Wait__");
    //printf("sending:%s\n", sendBuff);
    send(white_fd,sendBuff,strlen(sendBuff)+1,0);

    while (true)
    {

        if (white)
        {
            curr_fd = white_fd;
            wait_fd = black_fd;
        }
        else
        {
            curr_fd = black_fd;
            wait_fd = white_fd;
        }

        //printf("reciving:\n");
        recv(curr_fd,recvBuff,sizeof(recvBuff),0);
        //printf("recived:%s\n", recvBuff);
        std::string sMv(recvBuff);

        bool isOK = false;

        std::vector<std::pair<int, int>> pieces = findPieces(board, white);
        std::vector<std::string> captures;
        for (std::pair<int, int> i : pieces)
        {
            std::vector<std::string> newCaptures = findCaptures(board, i.first, i.second);
            captures.insert(captures.end(), newCaptures.begin(), newCaptures.end());
        }
        if (captures.size() > 0)
        {
            if (std::find(captures.begin(), captures.end(), sMv) != captures.end())
            {
                doMove(board, sMv);
                isOK = true;
                countMoves = 0;
                std::pair<std::pair<int, int>, std::pair<int, int>> ppMv = mvToPP(sMv);
                board[(ppMv.first.first + ppMv.second.first) / 2][(ppMv.first.second + ppMv.second.second) / 2] = ' ';
                std::vector<std::string> nextCaptures = findCaptures(board, ppMv.second.first, ppMv.second.second);
                if (nextCaptures.size() > 0) white = !white;
            }
            else
            {
                isOK = false;
            }
        }
        else
        {
            if (checkMove(board, sMv, white))
            {
                doMove(board, sMv);
                isOK = true;
            }
            else
            {
                isOK = false;
            }
        }

        kingsRow(board);
        countMoves++;
        if (countMoves > 15)
        {
            sprintf(sendBuff,"%s", "draw__");
            //printf("sending:%s\n", sendBuff);
            send(curr_fd,sendBuff,strlen(sendBuff)+1,0);
            send(wait_fd,sendBuff,strlen(sendBuff)+1,0);
            break;
        }

        if (!isOK)
        {
            if (white) sprintf(sendBuff,"%s", "blaWin");
            else sprintf(sendBuff,"%s", "whiWin");
            //printf("sending:%s\n", sendBuff);
            send(curr_fd,sendBuff,strlen(sendBuff)+1,0);
            send(wait_fd,sendBuff,strlen(sendBuff)+1,0);
            break;
        }
        else
        {
            sprintf(sendBuff,"%s", "wait__");
            //printf("sending:%s\n", sendBuff);
            send(curr_fd,sendBuff,strlen(sendBuff)+1,0);
        }
        

        char* cMv = new char[sMv.length() + 1];
        strcpy(cMv, sMv.c_str());
        sprintf(sendBuff,"%s", cMv);
        //printf("sending:%s\n", sendBuff);
        send(wait_fd,sendBuff,strlen(sendBuff)+1,0);

        white = !white;
    }
    close(white_fd);
    close(black_fd);

    free(t_data);
    pthread_exit(NULL);
}




int main(int argc, char **argv)
{
	if(false) printf("%d", argc);

    if (argc != 2)
   {
     fprintf(stderr, "Run like: %s port\n", argv[0]);
     exit(1);
   }

	struct sockaddr_in sa;

	int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if (socket_fd == -1)
    {
        printf("%s\n", "socket err");
        return 0;
    }
	int nFoo = 5, fd_1, fd_2;

	memset(&sa, 0 , sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(atoi(argv[1]));
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));
	
	int bindRes = bind(socket_fd,(struct sockaddr*) &sa, sizeof(struct sockaddr));
    if (bindRes != 0)
    {
        printf("%s,%d\n", "Bind err", bindRes);
        return 0;
    }
	int listRes = listen(socket_fd,100);
    if (listRes != 0)
    {
        printf("%s,%d\n", "Listen err", listRes);
        return 0;
    }

    while(true)
    {

        fd_1 = accept(socket_fd, NULL, NULL);
        //printf("%s\n", "Got one");
        fd_2 = accept(socket_fd, NULL, NULL);
        //printf("%s\n", "Got two");
        if((fd_1 == -1) || (fd_2 == -1))
        {
            printf("%s,%d\n", "Listen err", listRes);
            close(fd_1);
            close(fd_2);
            return 0;
        }

        int pthread_result = 0;
        pthread_t theThread;
        struct thread_data_t *t_data = (struct thread_data_t*)malloc(sizeof(struct thread_data_t));
        (*t_data).desc_1 = fd_1;
        (*t_data).desc_2 = fd_2;

        pthread_result = pthread_create(&theThread, NULL, gameThread, (struct thread_data_t* )t_data);
        if (pthread_result)
        {
            printf("Pthread err,%d\n", pthread_result);
            return 0;
        }

    }
    close(socket_fd);

}