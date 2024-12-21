#pragma once
#pragma once
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <ostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <assert.h>
#include "GameInfo.h"
#include "PlayerInfo.h"

#define MSG_SIZE 256
enum Operation
{
    SUM, DIFF, PROD, DIV, POWER
};

// dos ints estarían bien, el primero la opción y el segundo el objecto que quiera comprar (en caso de comprar)
typedef class DataPacket 
{
public:
    int client_id;
    int sequence;
    int option;
    int index;
    bool copyRecipeInventory[10];
    int copyIngredientsInventory[10];
    int copyMoney;
    DataPacket() {};
    DataPacket(int _client_id, int _sequence, int _option, int _index, bool* _copyRecipeInventory, int* _copyIngredientsInventory, int _copyMoney)
    {
        client_id = _client_id;
        sequence = _sequence;
        option = _option;
        index = _index;
        if (_copyRecipeInventory)
        {
            for (int i = 0; i < 10; ++i)
            {
                copyRecipeInventory[i] = _copyRecipeInventory[i];
            }
        }
        if (_copyIngredientsInventory)
        {
            for (int i = 0; i < 10; ++i)
            {
                copyIngredientsInventory[i] = _copyIngredientsInventory[i];
            }
        }
        if (_copyMoney)
        {
            copyMoney = _copyMoney;
        }
    }
} *PDataPacket;

typedef class ThreadInfo {
public:
    int thread_id;
    SOCKET s;
    std::string prefix;

    ThreadInfo() {};
    ThreadInfo(int _thread_id, SOCKET _s, std::string _prefix) {
        thread_id = _thread_id;
        s = _s;
        prefix = _prefix.c_str();
    }
    ~ThreadInfo() {
        closesocket(s);
    }
} *PThreadInfo;

std::ostream& operator << (std::ostream& os, const DataPacket& dp);
// The reason this is in a separate file is because I want to use this
// on the server and the client

void treatError(const std::string msg, SOCKET s);

void treatErrorExit(const std::string msg, SOCKET s, int error);

//UDP calls

int sendtoMsg(SOCKET s, sockaddr_in* dest_addr, PDataPacket packet, std::string prefix);

int recvfromMsg(SOCKET s, sockaddr_in* sender_addr, PDataPacket response, std::string prefix);

int sendtorecvfromMsg(SOCKET s, sockaddr_in* dest_addr, PDataPacket packet, PDataPacket response, std::string prefix);

int recvfromsendtoMsg(SOCKET s, PDataPacket response, std::string prefix);

//TCP calls

int sendMsg(SOCKET s, PDataPacket packet, std::string prefix);

int recvMsg(SOCKET s, PDataPacket response, std::string prefix);

int sendrecvMsg(SOCKET s, PDataPacket packet, PDataPacket response, std::string prefix);

int recvsendMsg(SOCKET s, PDataPacket response, std::string prefix);

int getAssignedPort(SOCKET s, sockaddr_in* my_addr);