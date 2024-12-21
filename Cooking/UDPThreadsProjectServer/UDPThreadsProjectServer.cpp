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
#include "GameInfo.h"
#include <cstdlib>

using namespace std;

#define MAX_THREADS 5

HANDLE serverFun(PDataPacket clientPacket, SOCKET s, sockaddr_in* client_addr, int i, string prefix);
int serverThreadFun(PDataPacket clientPacket, GameInfo *Game, PlayerInfo *Player);

DWORD WINAPI threadFun(LPVOID param);

Food newClientFood(GameInfo Game);


int main()
{
    srand(static_cast<unsigned int>(time(0)));
    string prefix = "Server: ";
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
    std::cout << "Server: cleaning up and returning" << endl;
    // cleanup
    closesocket(s);
    WSACleanup();
}


HANDLE serverFun(PDataPacket clientPacket, SOCKET s, sockaddr_in* client_addr, int i, string prefix) {
    int result = -1;
    //now we create a socket that uses IP (AF_INET) with UDP (SOCK_DGRAM, IPPROTO_UDP) 
    SOCKET s_new = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_new == INVALID_SOCKET) {
        treatErrorExit(format("Server: Thread[{}] socket creation error: ", i), s, -1);
    }
    std::cout << "Server: socket created" << std::endl;

    //now we specify the other machine we want to send messages to
    sockaddr_in my_addr;
    PCSTR address = "127.0.0.1";
    if (!inet_pton(AF_INET, address, &(my_addr.sin_addr.s_addr))) { // Replace with your desired IP address
        treatErrorExit(format("Server: Thread[{}] error converting IP in string to binary: ", i), s, -1);
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
    //call to thread(... s_new ...) with the new socket that only the thread will use


    DWORD dwThreadId;
    HANDLE hThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        threadFun,       // thread function name
        (void*)thInfo,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadId);   // returns the thread identifier 

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
    PDataPacket packet = new DataPacket();


        GameInfo Game;
        PlayerInfo Player;

        for (int i = 0; i < 10; i++)
        {
            if (Player.getMoney() >= Game.getRecipeList()[i].price)
            {
                Player.setMoney(-Game.getRecipeList()[i].price);
                Player.setNewRecipe(Game.getRecipeList()[i].id);
            }
        }

        for (int i = 0; i < 10; i++)
        {
            if (Player.getMoney() >= Game.getIngredientsList()[i].price)
            {
                Player.setMoney(-Game.getIngredientsList()[i].price);
                Player.setIngredients(i, 2);
            }
        }



    while (serve) {
        std::cout << "Server Thread ready to recv" << std::endl;
        //recv msg, then cast it to DataPacket and call serverFun
        //sockaddr_in client_addr;
        //recv msg, then cast it to DataPacket and call serverFun
        sockaddr_in client_addr;
        recvfromMsg(thInfo->s, &client_addr, packet, "Server:");

        DataPacket clientPacket = (DataPacket)*packet;

        //do something
        if (!serverThreadFun(&clientPacket, &Game, &Player)) {
            sendtoMsg(thInfo->s, &client_addr, &clientPacket, thInfo->prefix);
        }
        else { //if there is an error in serverThreadFun, or if the client sent some other unknown operation, close thread
            serve = false; //will exit the loop and clean before returning
        }
    }
    //cleanup of thread
    //closesocket(thInfo->s); //NOT NEEDED!! already in destructor of ThreadInfo
    delete thInfo;
    thInfo = NULL;
    return 0;
}

//makes operation with op1 and op2 storing the result in res, all of them fields of clientPacket
int serverThreadFun(PDataPacket clientPacket, GameInfo *Game, PlayerInfo *Player) 
{
  
    bool exit = false;
    while (!exit)
    {
      /*  Game.showIngredientsStore();
        Game.showRecipeStore();*/

        //Game.showRecipeInventory(Player.getRecipeInventory());
        cout << "money: " << (*Player).getMoney() << endl;
        Food clientFood = newClientFood(*Game);
        std::cout << "nuevo cliente! \nPedido: " << clientFood.foodName << "\ningredientes: " << clientFood.ingredient_1.name << " y " << clientFood.ingredient_2.name << "\n";
        switch (clientPacket->option)
        {

            case 0:
                if ((*Player).getRecipeInventory()[clientFood.id] && ((*Player).getIngredientsInventory()[clientFood.ingredient_1.id]) > 0 && ((*Player).getIngredientsInventory()[clientFood.ingredient_2.id]) > 0)
                {
                    (*Player).setIngredients(clientFood.ingredient_1.id, -1);
                    (*Player).setIngredients(clientFood.ingredient_2.id, -1);
                    (*Player).setMoney(clientFood.price);
                    cout << "receta " << clientFood.foodName << " hecha" << endl;
                }
                (*Game).showIngredientsInventory((*Player).getIngredientsInventory());

                 break;

            case 1:

                break;

            case 2:

                break;

            case 3:

                break;

            case 4:

                break;

            case 5:

                break;

            case 6:

                break;

            case 7:

                break;
        }



        exit = true;
        
        //enviar datapacket con el food.
        //imprimir lo siguiente en el cliente
        // 
        //  std::cout << "nuevo cliente! \n Pedido: " << clientFood.foodName << "\ningredientes: " << clientFood.ingredient_1.name << " y " << clientFood.ingredient_2.name << std::endl;
        //          0 atender al cliente                (el server comprueba si puede hacer la receta,si sí se restan los ingredientes y se suma la plata, si no, se devuelve)
        //          1 abrir inventario de recetas       (el server envía una copia de datos de Player.ingredientsInventory mediante packet)
        //          2 abrir inventario de ingredientes  (el server envía una copia de datos de Player.recipeInventory mediante packet)
        //          3 abrir store de recetas            (el server envía una copia de datos de Player.money mediante packet)
        //          4 abrir store de ingredientes       (el server envía una copia de datos de Player.money mediante packet)
        //          5 comprar receta                    (el server comprueba si se tiene el dinero y si ya compró esa receta, le actualiza el inventario de recetas y le resta dinero)
        //          6 comprar ingrediente               (el server comprueba si se tiene el dinero, le suma una unidad al inventario de ingredientes y le resta dinero)
        //          7 pasar al siguiente cliente        (crea un newClientFood)

    }

    //cout << "ClientPacket: " << *clientPacket << endl;
    //switch (clientPacket->operation) {
    //case SUM: clientPacket->res = clientPacket->op1 + clientPacket->op2;
    //    break;
    //case DIFF: clientPacket->res = clientPacket->op1 - clientPacket->op2;
    //    break;
    //case PROD: clientPacket->res = clientPacket->op1 * clientPacket->op2;
    //    break;
    //case DIV: clientPacket->res = clientPacket->op1 / clientPacket->op2;
    //    break;
    //case POWER: clientPacket->res = (long long)pow(clientPacket->op1, clientPacket->op2);
    //    break;
    //default:
    //    return -1;
    //}

    //cout << "Operation performed: " << *clientPacket << endl;


    return 0;
}

Food newClientFood(GameInfo Game)
{

    int random_number = rand() % 10;
    return Game.getFoodList()[random_number];
}