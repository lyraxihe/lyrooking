#pragma comment(lib, "ws2_32.lib") //add WinSock2 library

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <assert.h>
#include "UDPThreadsProjectLib.h"
#include <iostream>
#include <math.h>
#include <format>
#include "PlayerInfo.h"
#include <cstdlib>

#define MAX_THREADS 5

HANDLE serverFun(PDataPacket clientPacket, SOCKET s, sockaddr_in* client_addr, int i, std::string prefix);
int serverThreadFun(PDataPacket clientPacket, GameInfo *Game, PlayerInfo *Player);

DWORD WINAPI threadFun(LPVOID param);

Food newClientFood(GameInfo Game);


int main()
{
    srand(static_cast<unsigned int>(time(0)));
    std::string prefix = "Server: ";
    std::cout << "Server: starting..." << std::endl;
    //required intialization of WinSock 2 library, it writes some data om wsaData to check everything is ok
    int result;
    WSAData wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    assert(result == NO_ERROR);

    std::cout << "Server: WinSock started correctly" << std::endl;

    //now we create a socket that uses IP (AF_INET) with UDP (SOCK_DGRAM and IPPROTO_UDP)
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //assert(s != INVALID_SOCKET);
    if (result == INVALID_SOCKET) {
        treatErrorExit("Server: Socket creation error: ", s, -1);
    }
    std::cout << prefix << "socket created" << std::endl;

    //now we specify the other machine we want to send messages to
    sockaddr_in my_addr;
    PCSTR address = "127.0.0.1";
    if (!inet_pton(AF_INET, address, &(my_addr.sin_addr.s_addr))) { // Replace with your desired IP address
        treatErrorExit("Server: error converting IP in string to binary: ", s, -1);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4000); //store bytes in network format big-endian

    //in the server we need to bind in order to be able to recvfrom through the socket
    result = bind(s, (sockaddr*)&my_addr, sizeof(my_addr));
    //assert(result != SOCKET_ERROR);
    if (result == SOCKET_ERROR) {
        treatErrorExit("Server: bind error", s, -1);
    }

    PDataPacket packet = new DataPacket();
    std::cout << "Server: socket bound to address: " << address << " port: " << ntohs(my_addr.sin_port) << std::endl;

    HANDLE  hThreadArray[MAX_THREADS]; //handlers of threads
    int i = 0;
    while (i < MAX_THREADS) {
        std::cout << "Server ready to recv" << std::endl;
        //recv msg and call serverFun
        sockaddr_in client_addr;
        recvfromMsg(s, &client_addr, packet, prefix);

        //do something
        hThreadArray[i] = serverFun(packet, s, &client_addr, i, prefix);
        i++;
    }

    // Wait until all threads have terminated.
    //how many to wait for, array of handlers, wait for all of them, how long to wait
    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    // Close all thread handles and free memory allocations.
    //this could be in a different function
    for (int i = 0; i < MAX_THREADS; i++) {
        CloseHandle(hThreadArray[i]);
    }
    std::cout << "Server: cleaning up and returning" << std::endl;
    // cleanup
    closesocket(s);
    WSACleanup();
}


HANDLE serverFun(PDataPacket clientPacket, SOCKET s, sockaddr_in* client_addr, int i, std::string prefix) {
    int result = -1;
    //now we create a socket that uses IP (AF_INET) with UDP (SOCK_DGRAM, IPPROTO_UDP) 
    SOCKET s_new = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_new == INVALID_SOCKET) {
        treatErrorExit(std::format("Server: Thread[{}] socket creation error: ", i), s, -1);
    }
    std::cout << "Server: socket created" << std::endl;

    //now we specify the other machine we want to send messages to
    sockaddr_in my_addr;
    PCSTR address = "127.0.0.1";
    if (!inet_pton(AF_INET, address, &(my_addr.sin_addr.s_addr))) { // Replace with your desired IP address
        treatErrorExit(std::format("Server: Thread[{}] error converting IP in string to binary: ", i), s, -1);
    }
    my_addr.sin_family = AF_INET;
    //store bytes in network format == big-endian
    my_addr.sin_port = htons(0); //4000 Replace with your desired port or 0 so that the OS chooses one that is available

    //in the server we need to bind in order to be able to listen through the socket
    result = bind(s_new, (sockaddr*)&my_addr, sizeof(my_addr));
    //assert(result != SOCKET_ERROR);
    if (result == SOCKET_ERROR) {
        treatErrorExit("Server: bind error", s, -1);
    }
    std::cout << "Server: socket bound to address: " << address << " port: " << ntohs(my_addr.sin_port) << std::endl;

    sendtoMsg(s_new, client_addr, clientPacket, prefix);
    //create object that serves as param for the thread function

    PThreadInfo thInfo = new ThreadInfo(i, s_new, prefix);


    DWORD dwThreadId;
    HANDLE hThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        threadFun,              // thread function name
        (void*)thInfo,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadId);           // returns the thread identifier 

    if (hThread == NULL) {
        treatErrorExit("CreateThread", s, -1);
        ExitProcess(3);
    }

    return hThread;
}

//function of dedicated thread in the server for a specific client
DWORD WINAPI threadFun(LPVOID param) {
    PThreadInfo thInfo = (ThreadInfo*)param;
    bool serve = true;

    //se crea el nuevo juego y player
    GameInfo Game;
    PlayerInfo Player;

    while (serve) {
        std::cout << "Server Thread ready to recv" << std::endl;
        //recv msg, then cast it to DataPacket and call serverFun
        //sockaddr_in client_addr;
        //recv msg, then cast it to DataPacket and call serverFun
        sockaddr_in client_addr;
        PDataPacket packet = new DataPacket();
        recvfromMsg(thInfo->s, &client_addr, packet, "Server:");
        DataPacket clientPacket = (DataPacket)*packet;
        if (!serverThreadFun(&clientPacket, &Game, &Player)) 
        {
            sendtoMsg(thInfo->s, &client_addr, &clientPacket, thInfo->prefix);
        }
        else 
        { //if there is an error in serverThreadFun, or if the client sent some other unknown operation, close thread
            serve = false; //will exit the loop and clean before returning
            std::cout << "error al entrar en server threadfun" << std::endl;
        }
    }
    //cleanup of thread
    delete thInfo;
    thInfo = NULL;
    return 0;
}

int serverThreadFun(PDataPacket clientPacket, GameInfo* Game, PlayerInfo* Player)
{
        switch (clientPacket->option)
        {
            case 1:
            {
                //comprueba si se tiene la receta y los ingredientes, se restan los ingredientes y se suma la plata
                if (((*Player).getRecipeInventory()[clientPacket->idRecipe]) && ((*Player).getIngredientsInventory()[clientPacket->idIngredient_1]) > 0 && ((*Player).getIngredientsInventory()[clientPacket->idIngredient_2]) > 0)
                {
                    (*Player).setIngredients(clientPacket->idIngredient_1, -1);
                    (*Player).setIngredients(clientPacket->idIngredient_2, -1);
                    (*Player).setMoney(clientPacket->clientFoodprice);
                    clientPacket->exitoso = true;
                }
                break;
            }

            case 2:
            {
                // envía una copia de datos del recipeInventory del jugador
                for (int i = 0; i < 10; ++i)
                {
                    clientPacket->copyRecipeInventory[i] = (*Player).getRecipeInventory()[i];
                }
                break;
            }

            case 3:
            {
                // envía una copia de datos del ingredientsInventory del jugador
                for (int i = 0; i < 10; ++i)
                {
                    clientPacket->copyIngredientsInventory[i] = (*Player).getIngredientsInventory()[i];
                }
                break;
            }

            case 4:
            {
                // envía una copia de datos del dinero del jugador
                clientPacket->copyMoney = (*Player).getMoney();
                break;
            }

            case 5:
            {
                // envía una copia de datos del dinero del jugador
                clientPacket->copyMoney = (*Player).getMoney();
                break;
            }

            case 6:
            {
                // comprueba si se tiene el dinero y si ya se compró esa receta, le actualiza el inventario de recetas, le resta dinero y asigna la operación como exitosa
                if (!(*Player).getRecipeInventory()[clientPacket->index] && ((*Player).getMoney() >= (*Game).getRecipeList()[clientPacket->index].price))
                {
                    (*Player).setNewRecipe(clientPacket->index);
                    (*Player).setMoney(-(*Game).getRecipeList()[clientPacket->index].price);
                    clientPacket->exitoso = true;
                }
                break;
            }

            case 7:
            {
                //comprueba si se tiene el dinero, le suma una unidad al inventario de ingredientes, le resta dinero y asigna la operación como exitosa
                if ((*Player).getMoney() >= (*Game).getIngredientsList()[clientPacket->index].price)
                {
                    (*Player).setIngredients(clientPacket->index, 1);
                    (*Player).setMoney(-(*Game).getIngredientsList()[clientPacket->index].price);
                    clientPacket->exitoso = true;
                }
                break;
            }

            case 8:
            {
                //pasa al siguiente cliente, es decir, crea un nuevo clientFood
                Food clientFood = newClientFood(*Game);

                clientPacket->clientFoodName = clientFood.foodName;
                clientPacket->idRecipe = clientFood.id;
                clientPacket->clientFoodprice = clientFood.price;
                clientPacket->ingredientName_1 = clientFood.ingredient_1.name;
                clientPacket->idIngredient_1 = clientFood.ingredient_1.id;
                clientPacket->ingredientName_2 = clientFood.ingredient_2.name;
                clientPacket->idIngredient_2 = clientFood.ingredient_2.id;
                clientPacket->exitoso = true;
                break;
            }
        }

    return 0;
}

Food newClientFood(GameInfo Game)
{
    int random_number = rand() % 10;
    return Game.getFoodList()[random_number];
}