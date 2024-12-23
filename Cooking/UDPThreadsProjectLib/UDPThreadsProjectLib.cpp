// UDPLib1.cpp : Define las funciones de la biblioteca estática.
//
#include <iostream>
#include <ostream>
#include "pch.h"
#include "framework.h"
#include "UDPThreadsProjectLib.h"
#include <format>

std::ostream& operator << (std::ostream& os, const DataPacket& dp) {
    return (os << "DataPacket{client: " << dp.client_id
        << " option: " << dp.option
        << " index: " << dp.index << "}");
}

//will print msg with WSAGetLastError, then closesocket
void treatError(const std::string msg, SOCKET s) {
    std::cout << msg << WSAGetLastError() << std::endl;
    closesocket(s);
}

//will print msg with WSAGetLastError, then closesocket, WSACleanup and ExitProcess(error)
void treatErrorExit(const std::string msg, SOCKET s, int error) {
    std::cout << msg << WSAGetLastError() << std::endl;
    closesocket(s);
    WSACleanup();
    ExitProcess(error);
}

//these functions should be visible to the library and it will call them automagically!
void to_json(json& j, const DataPacket& d) 
{
    j = json{ 

                {"client_id", d.client_id}, 
                {"option", d.option},
                {"index", d.index},
                {"clientFoodName", d.clientFoodName},
                {"idRecipe", d.idRecipe},
                {"clientFoodprice", d.clientFoodprice},
                {"ingredientName_1", d.ingredientName_1},
                {"idIngredient_1", d.idIngredient_1},
                {"ingredientName_2", d.ingredientName_2},
                {"idIngredient_2", d.idIngredient_2},
                {"copyRecipeInventory",d.copyRecipeInventory},
                {"copyIngredientsInventory",d.copyIngredientsInventory},
                {"copyMoney",d.copyMoney},
                {"exitoso",d.exitoso}
            };
}

//create Datapacket from json object
void from_json(const json& j, DataPacket& d) 
{
    j.at("client_id").get_to(d.client_id);
    j.at("option").get_to(d.option);
    j.at("index").get_to(d.index);
    j.at("clientFoodName").get_to(d.clientFoodName);
    j.at("idRecipe").get_to(d.idRecipe);
    j.at("clientFoodprice").get_to(d.clientFoodprice);
    j.at("ingredientName_1").get_to(d.ingredientName_1);
    j.at("idIngredient_1").get_to(d.idIngredient_1);
    j.at("ingredientName_2").get_to(d.ingredientName_2);
    j.at("idIngredient_2").get_to(d.idIngredient_2);
    j.at("copyRecipeInventory").get_to(d.copyRecipeInventory);
    j.at("copyIngredientsInventory").get_to(d.copyIngredientsInventory);
    j.at("copyMoney").get_to(d.copyMoney);
    j.at("exitoso").get_to(d.exitoso);
}

//UDP calls

//performs sendto, assuming all required WinSock2 previous calls were succesfull
int sendtoMsg(SOCKET s, sockaddr_in* dest_addr, PDataPacket packet, std::string prefix) {
    //now we just send the data through the socket
    //we make the casting to char* because it expects data as just chars, last param 0 is for flags that we don't need

    // conversion: DataPacket -> json invoking to_json
    json j = *packet;
    //std::cout << j.dump() << std::endl; 
    
    //std::cout << j << std::endl;
    
    char json_text[MSG_SIZE];
    memcpy(json_text, j.dump().c_str(), MSG_SIZE); //deep copy of data to buffer that will be sent
    //int result = sendto(s, (char)packet, sizeof(DataPacket), 0, (SOCKADDR)dest_addr, sizeof(SOCKADDR));
    int result = sendto(s, json_text, sizeof(json_text), 0, (SOCKADDR*)dest_addr, sizeof(SOCKADDR));
    assert(result != SOCKET_ERROR);

    //std::cout << prefix << " succesfully sent msg: " << *packet << std::endl;
    return result;
}


//performs recvfrom, assuming all required WinSock2 previous calls were succesfull
int recvfromMsg(SOCKET s, sockaddr_in* sender_addr, PDataPacket response, std::string prefix) {
    //receive response from Server
    //DataPacket response;
    char buffer[MSG_SIZE];
    int fromlen = sizeof(SOCKADDR);
    //recvfrom addr is ALWAYS an out param 
    //int result = recvfrom(s, (char)response, sizeof(DataPacket), 0, (SOCKADDR)sender_addr, &fromlen);
    int result = recvfrom(s, buffer, sizeof(buffer), 0, (SOCKADDR*)sender_addr, &fromlen);
    assert(result != SOCKET_ERROR);
    json j2 = json::parse(buffer);
    //std::cout << j2 << std::endl;
    //std::cout << j2.dump() << std::endl;
    //conversion json -> DataPacket invoking from_json
    *response = j2;

    //std::cout << prefix << " succesfully received: " << response << std::endl;
    return result;
}

//performs sendto and then recvfrom, assuming all required WinSock2 previous calls were succesfull
int sendtorecvfromMsg(SOCKET s, sockaddr_in* dest_addr, PDataPacket packet, PDataPacket response, std::string prefix) {
    sendtoMsg(s, dest_addr, packet, prefix);
    return recvfromMsg(s, dest_addr, response, prefix);
}

//performs recvfrom and then sendto, assuming all required WinSock2 previous calls were succesfull
int recvfromsendtoMsg(SOCKET s, PDataPacket response, std::string prefix) {
    sockaddr_in sender_addr;
    recvfromMsg(s, &sender_addr, response, prefix);
    return sendtoMsg(s, &sender_addr, response, prefix);
}


//TCP calls

//performs send, assuming all required WinSock2 previous calls were succesfull
int sendMsg(SOCKET acceptSocket, PDataPacket packet, std::string prefix) {
    //IMPORTANT: size of struct/class not the size of the pointer!
    int sbyteCount = send(acceptSocket, (char*)packet, sizeof(DataPacket), 0);
    if (sbyteCount < 0) {
        treatError(std::format("{} send error: ", prefix), acceptSocket);
    }
    else {
        //std::cout << prefix << " succesfully sent msg: " << *packet << std::endl;
    }
    return sbyteCount;
}

//performs recv, assuming all required WinSock2 previous calls were succesfull
int recvMsg(SOCKET acceptSocket, PDataPacket recv_msg, std::string prefix) {
    int rbyteCount = recv(acceptSocket, (char*)recv_msg, sizeof(DataPacket), 0);
    if (rbyteCount < 0) {
        treatError(std::format("{} recv error: ", prefix), acceptSocket); //change when loop to treatError
    }
    else {
        DataPacket clientPacket = (DataPacket)*recv_msg;
        //std::cout << prefix << " succesfully received: " << clientPacket << std::endl;
    }
    return rbyteCount;
}

//performs send and then recv, assuming all required WinSock2 previous calls were succesfull
int sendrecvMsg(SOCKET s, PDataPacket packet, PDataPacket response, std::string prefix) {
    sendMsg(s, packet, prefix);
    return recvMsg(s, response, prefix);
}

//performs recv and then send, assuming all required WinSock2 previous calls were succesfull
int recvsendMsg(SOCKET s, PDataPacket response, std::string prefix) {
    recvMsg(s, response, prefix);
    return sendMsg(s, response, prefix);
}

//invokes getsockname and retrieves the port number in this machine's architecture encoding
int getAssignedPort(SOCKET s, sockaddr_in* my_addr) {
    int namelen = sizeof(my_addr);
    if (getsockname(s, (SOCKADDR*)my_addr, &namelen) != 0) {
        treatErrorExit("Server: getsockname error: ", s, -1);
    }
    int assigned_port = ntohs(my_addr->sin_port);
    std::cout << "Server: socket bound to port: " << my_addr->sin_port << std::endl;
    return assigned_port;
}