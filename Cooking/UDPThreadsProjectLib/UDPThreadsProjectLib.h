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
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;


#define MSG_SIZE 512

typedef class DataPacket 
{
public:
    int client_id;
    int option;
    int index;

    std::string clientFoodName;
    int idRecipe;
    int clientFoodprice;
    std::string ingredientName_1;
    int idIngredient_1;
    std::string ingredientName_2;
    int idIngredient_2;

    bool copyRecipeInventory[10];
    int copyIngredientsInventory[10];
    int copyMoney;
    bool exitoso;
    DataPacket() {};
    DataPacket(int _client_id, int _option, int _index, std::string _clientFoodName, int _idRecipe, int _clientFoodprice, std::string _ingredientName_1, int _idIngredient_1, std::string _ingredientName_2, int _idIngredient_2, bool* _copyRecipeInventory, int* _copyIngredientsInventory, int _copyMoney, bool _exitoso)
    {
        client_id = _client_id;
        option = _option;
        index = _index;
        clientFoodName = _clientFoodName;
        idRecipe = _idRecipe;
        clientFoodprice = _clientFoodprice;
        ingredientName_1 = _ingredientName_1;
        idIngredient_1 = _idIngredient_1;
        ingredientName_2 = _ingredientName_2;
        idIngredient_2 = _idIngredient_2;
        exitoso = _exitoso;

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

void to_json(json& j, const DataPacket& d);

void from_json(const json& j, DataPacket& p);

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