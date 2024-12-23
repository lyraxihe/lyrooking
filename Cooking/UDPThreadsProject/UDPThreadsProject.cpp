#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <assert.h>
#include "UDPThreadsProjectLib.h"
#include <iostream>
#include <string>
#include <Windows.h>

#define MAX_MSGS 5

int obtainNewPort(SOCKET s, sockaddr_in* server_addr, std::string prefix);

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "UDPClient usage: .\\UDPClient <id>" << std::endl;
        ExitProcess(-1);
    }
    int client = atoi(argv[1]);
    std::cout << "Client: starting..." << std::endl;
    //required intialization of WinSock 2 library, it writes some data om wsaData to check everything is ok
    int result;
    WSAData wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    assert(result == NO_ERROR);
    std::cout << "Client: WinSock started correctly" << std::endl;

    //now we create a socket that uses IP (AF_INET) with UDP (SOCK_DGRAM and IPPROTO_UDP) 
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        treatErrorExit("Client: Socket creation error: ", s, -1);
    }
    std::cout << "Client: socket created" << std::endl;
    //now we specify the other machine we want to send messages to
    sockaddr_in server_addr;
    PCSTR address = "127.0.0.1";
    if (!inet_pton(AF_INET, address, &(server_addr.sin_addr.s_addr))) { // Replace with your desired IP address
        printf("error converting IP in string to binary\n");
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);

    std::string prefix = "Client:";
    obtainNewPort(s, &server_addr, prefix);
    std::cout << "Client already obtained new port: " << ntohs(server_addr.sin_port) << std::endl;

    std::cout << std::endl << std::endl << "Lyrooking !" << std::endl;
    bool exit = false;
    int option = 0;
    int index;

    GameInfo Game;

    PDataPacket packet = new DataPacket();
    PDataPacket response = new DataPacket();

    packet->option = 8;
    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");

        Food clientFood;
        clientFood.foodName = response->clientFoodName;
        clientFood.id = response->idRecipe;
        clientFood.price = response->clientFoodprice;
        clientFood.ingredient_1.name = response->ingredientName_1;
        clientFood.ingredient_1.id = response->idIngredient_1;
        clientFood.ingredient_2.name = response->ingredientName_2;
        clientFood.ingredient_2.id = response->idIngredient_2;

    while (!exit)
    {

        std::cout << std::endl << std::endl;
       
        std::cout << "Customer's order: " << clientFood.foodName << "\ningredients: " << clientFood.ingredient_1.name << " and " << clientFood.ingredient_2.name << std::endl << std::endl;
        std::cout << "Choose one of the following options: " << std::endl;
        std::cout << "1.  serve the customer" << std::endl;
        std::cout << "2.  open recipe inventory" << std::endl;
        std::cout << "3.  open ingredient inventory" << std::endl;
        std::cout << "4.  open recipe store" << std::endl;
        std::cout << "5.  open ingredient store" << std::endl;
        std::cout << "6.  move to the next customer" << std::endl;
        std::cout << "7.  exit the game" << std::endl;
        std::cout << "option: ";
        std::cin >> option;

        if (option > 0 && option < 8)
        {
            switch (option)
            {
                case 1:
                {
                    packet = new DataPacket(client, 1, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                    if (response->exitoso)
                    {
                        std::cout << "Order completed: " << clientFood.foodName << std::endl;
                        std::cout << "Coins added: " << clientFood.price << std::endl;


                        packet = new DataPacket(client, 8, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                        sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                        if (response->exitoso)
                        {
                            clientFood.foodName = response->clientFoodName;
                            clientFood.id = response->idRecipe;
                            clientFood.price = response->clientFoodprice;
                            clientFood.ingredient_1.name = response->ingredientName_1;
                            clientFood.ingredient_1.id = response->idIngredient_1;
                            clientFood.ingredient_2.name = response->ingredientName_2;
                            clientFood.ingredient_2.id = response->idIngredient_2;
                            std::cout << "Moved to the next customer" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Could not complete the order " << clientFood.foodName << std::endl;
                    }
                    break;
                }

                case 2:
                {
                    packet = new DataPacket(client, 2, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                    Game.showRecipeInventory(response->copyRecipeInventory);
                    break;
                }

                case 3:
                {
                    packet = new DataPacket(client, 3, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                    Game.showIngredientsInventory(response->copyIngredientsInventory);
                    break;
                }

                case 4:
                {
                    packet = new DataPacket(client, 4, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                    Game.showRecipeStore();
                    std::cout << "Coins: " << response->copyMoney << std::endl << std::endl;
                    std::cout << "Select the index of the recipe you want to buy or 10 to return to the menu" << std::endl;
                    std::cout << "option: ";
                    std::cin >> index;

                    if (index >= 0 && index < 10)
                    {
                        packet = new DataPacket(client, 6, index, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                        sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                        if (response->exitoso)
                        {
                            std::cout << "The recipe for " << Game.getRecipeList()[index].name << " has been added to the recipe inventory!" << std::endl;
                        }
                        else
                        {
                            std::cout << "Could not buy the recipe for " << Game.getRecipeList()[index].name << std::endl;
                        }
                    }
                    break;
                }

                case 5:
                {
                    packet = new DataPacket(client, 5, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                    Game.showIngredientsStore();
                    std::cout << "Coins: " << response->copyMoney << std::endl << std::endl;
                    std::cout << "Select the index of the ingredient you want to buy or 10 to return to the menu" << std::endl;
                    std::cout << "option: ";
                    std::cin >> index;

                    if (index >= 0 && index < 10)
                    {
                        packet = new DataPacket(client, 7, index, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                        sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                        if (response->exitoso)
                        {
                            std::cout << "The ingredient " << Game.getIngredientsList()[index].name << " has been added to the ingredient inventory!" << std::endl;

                        }
                        else
                        {
                            std::cout << "Could not buy the ingredient " << Game.getIngredientsList()[index].name << std::endl;
                        }
                    }
                    break;
                }

                case 6:
                {
                    packet = new DataPacket(client, 8, 0, clientFood.foodName, clientFood.id, clientFood.price, clientFood.ingredient_1.name, clientFood.ingredient_1.id, clientFood.ingredient_2.name, clientFood.ingredient_2.id, NULL, NULL, NULL, false);
                    sendtorecvfromMsg(s, &server_addr, packet, response, "Client:");
                    if (response->exitoso)
                    {
                        clientFood.foodName = response->clientFoodName;
                        clientFood.id = response->idRecipe;
                        clientFood.price = response->clientFoodprice;
                        clientFood.ingredient_1.name = response->ingredientName_1;
                        clientFood.ingredient_1.id = response->idIngredient_1;
                        clientFood.ingredient_2.name = response->ingredientName_2;
                        clientFood.ingredient_2.id = response->idIngredient_2;
                        std::cout << "Moved to the next customer" << std::endl;
                    }
                    break;
                }

                case 7:
                {
                    exit = true;
                    break;
                }
            }
        }
       
    }

    std::cout << "Client finishing..." << std::endl;
    int iResult = closesocket(s);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    WSACleanup();
    return 0;
}

//sends first msg to server and returns with the new server_addr used for the server for the dedicated socket
int obtainNewPort(SOCKET s, sockaddr_in* server_addr, std::string prefix) {
    PDataPacket packet = new DataPacket(); //I don't initialize because the server won't care
    std::cout << "Client ready to send: " << *packet << std::endl;
    PDataPacket response = new DataPacket();
    //IMPORTANT: will overwrite server_addr with the server addr with the new port, since the response msg in the server is sent through the new socket!
    sendtorecvfromMsg(s, server_addr, packet, response, prefix);
    return 0;
}

