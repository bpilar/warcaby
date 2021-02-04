#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <stdio.h>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <ncurses.h>
//#include <syscall.h>
#include <arpa/inet.h>


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
    move(2, 0);
    printw("                 ");
    for (int j = 0; j < 8; j++)
    {
        move(j+3, 0);
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
                    printw("|*");
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

void printScreen(char board[8][8], bool white, bool meWhite, int y, int x, std::string address, std::string port)
{
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    attrset(COLOR_PAIR(2));
    clear();
    move(0,6);
    printw("English draughts\n");
    attrset(COLOR_PAIR(3));
    move(1,0);
	if (meWhite)
    {
        printw("%s", "Playing white   ");
    }
    else
    {
        printw("%s", "Playing black   ");
    }
    if (white)
    {
        printw("%s", "White's turn:");
    }
    else
    {
        printw("%s", "Black's turn:");
    }
    attrset(COLOR_PAIR(1));
    attron(A_UNDERLINE);
    printBoard(board, y, x);
    attroff(A_UNDERLINE);
    attrset(COLOR_PAIR(2));
    move(4, 20); printw("w - white's man");
    move(5, 20); printw("v - white's king");
    move(6, 20); printw("b - black's man");
    move(7, 20); printw("a - black's king");
    move(8, 20); printw("HOME to reconnect");
    move(9, 20); printw("END to quit");
    move(11,0); printw("Connected to - %s:%s", address.c_str(), port.c_str());
    attrset(COLOR_PAIR(4));
    move(12,0);
    refresh();
}

std::pair<std::string,std::string> getConParameters()
{
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    nocbreak();
    echo();
    clear();
    nodelay(stdscr,FALSE);
    char cAddr[20];
    char cPort[20];

    move(0,0);
    attrset(COLOR_PAIR(2));
    printw("%s", "Address:");
    attrset(COLOR_PAIR(1));
    refresh();
    scanf("%s", cAddr);
    refresh();

    move(1,0);
    attrset(COLOR_PAIR(2));
    printw("%s", "Port:");
    attrset(COLOR_PAIR(1));
    refresh();
    scanf("%s", cPort);
    refresh();
    
    std::string address(cAddr);
    std::string port(cPort);
    cbreak();
    noecho();
    return std::make_pair(address,port);
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
    if((abs(pFrom.first-pTo.first) == 2) && (abs(pFrom.second-pTo.second) == 2))
    {
        board[(pFrom.first+pTo.first)/2][(pFrom.second+pTo.second)/2] = ' ';
    }
}

void kingsRow(char board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        if (board[0][i] == 'w') board[0][i] = 'v';
        if (board[7][i] == 'b') board[7][i] = 'a';
    }
}

int main()
{
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	bool connected = false;
	bool keepPlaying = true;

	while(keepPlaying)
	{
		struct sockaddr_in sa;

		int server_fd = socket(AF_INET,SOCK_STREAM,0);
        if (server_fd == -1)
        {
            printw("%s\n", "socket err");
            break;
        }

		char sendBuff[10];
		char recvBuff[10+1];


		char board[8][8];
		initBoard(board);
		int posY = 0, posX = 0;
		bool white = false;
        bool meWhite = false;
		std::string address = "jakis";
		std::string port = "errur";

		if (!connected)
		{
			std::pair<std::string,std::string> conParameters = getConParameters();
			address = conParameters.first;
			port = conParameters.second;

			memset(&sa, 0 , sizeof(sa));
			sa.sin_family = AF_INET;
			    char* cPort = new char[port.length() + 1];
                strcpy(cPort, port.c_str());
			sa.sin_port = htons(atoi(cPort));
                char* cAddress = new char[address.length() + 1];
                strcpy(cAddress, address.c_str());
			inet_pton(AF_INET,cAddress,&(sa.sin_addr));

            move(3,0);
			int connRes = connect(server_fd, (struct sockaddr*) &sa, sizeof(struct sockaddr));
            if (connRes != 0)
            {
                printw("%s,%d\n", "Connect err", connRes);
                break;
            }

			printw("%s\n", "Connecting");
            refresh();
            recv(server_fd,recvBuff,sizeof(recvBuff),0);
            std::string myColor(recvBuff);
            if (!myColor.compare("white_"))
            {
                meWhite = true;
            }
			connected = true;
            refresh();
		}
		

		while (connected)
		{
			printScreen(board, white, meWhite, posY, posX, address, port);
			

            if (white == meWhite)
            {
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
                    case KEY_DOWN:
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
                    case KEY_HOME:
                        connected = false;
                        printw("%s\n", "Reconnect");
                        break;
                    case KEY_END:
                        connected = false;
                        keepPlaying = false;
                        printw("%s\n", "Disconnect");
                        break;
                    default:
                        break;
                    }
                    if(!connected) break;
                    if (posY < 0) posY = 0;
                    if (posY > 7) posY = 7;
                    if (posX < 0) posX = 0;
                    if (posX > 7) posX = 7;
                    printScreen(board, white, meWhite, posY, posX, address, port);
                }
                if(!connected) break;
                char* cMv = new char[sMv.length() + 1];
                strcpy(cMv, sMv.c_str());
                sprintf(sendBuff,"%s", cMv);
                send(server_fd,sendBuff,strlen(sendBuff)+1,0);
                doMove(board, sMv);
            }
            else
            {
                recv(server_fd,recvBuff,sizeof(recvBuff),0);
                std::string recvdMsg(recvBuff);
                if (!recvdMsg.compare("blaWin"))
                {
                    if(meWhite) printw("  Lost :(\n");
                    else printw("  Victory :)\n");
                    break;
                }
                if (!recvdMsg.compare("whiWin"))
                {
                    if(meWhite) printw("  Victory :)\n");
                    else printw("  Lost :(\n");
                    break;
                }
                if (!recvdMsg.compare("draw__"))
                {
                    printw("  Draw ---\n");
                    break;
                }
                printw("    %s\n", recvBuff);
                refresh();
                recv(server_fd,recvBuff,sizeof(recvBuff),0);
                std::string sMv(recvBuff);
                if (!sMv.compare("blaWin"))
                {
                    if(meWhite) printw("  Lost :(\n");
                    else printw("  Victory :)\n");
                    break;
                }
                if (!sMv.compare("whiWin"))
                {
                    if(meWhite) printw("  Victory :)\n");
                    else printw("  Lost :(\n");
                    break;
                }
                if (!sMv.compare("draw__"))
                {
                    printw("  Draw ---\n");
                    break;
                }
                //check move or !taking!
                doMove(board, sMv);
            }

			kingsRow(board);

			white = !white;
			clear();
		}
		refresh();

        sprintf(sendBuff,"%s", "quit__");
        if(connected) send(server_fd,sendBuff,strlen(sendBuff)+1,0);
		close(server_fd);

		connected = false;
		int key = getch();
		if (key == KEY_END)
		{
			keepPlaying = false;
		}
		if (!keepPlaying)
		{
			printw("%s\n", "Quit");
		}
		
	}
	refresh();
	getch();
	endwin();

}