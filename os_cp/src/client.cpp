#include <iostream>
#include <cstring>
#include "zmq.hpp"
#include <string>
#include <thread>
#include <string>
#include <unistd.h>

// g++ client.cpp -lzmq -pthread -o client -w

void send_message(std::string message_string, zmq::socket_t &socket)
{
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size());
    if (!socket.send(message_back))
    {
        std::cout << "Error" << std::endl;
    }
}

std::string receive_message(zmq::socket_t& socket){
    zmq::message_t message_main;
    socket.recv(&message_main);
    std::string answer(static_cast<char*>(message_main.data()), message_main.size());
    return answer;
}

void process_terminal(zmq::socket_t &pusher, std::string login)
{
    std::string command = "";
    std::cout << "Enter command" << std::endl;
    while (std::cin >> command)
    {
        if (command == "send") {
            std::cout << "Enter nickname of recipient" << std::endl;
            std::string recipient = "";
            std::cin >> recipient;
            std::cout << "Enter your message" << std::endl;
            std::string client_message = "";
            char a; std::cin >> a;
            std::getline (std::cin, client_message);
            std::string message_string = "send " + recipient + " " + login + " " + a + client_message;
            send_message(message_string, pusher);
        }
        if (command == "history") {
            std::string message_string = "history";
            send_message(message_string, pusher);
        }
        else if (command == "exit") {
            send_message("exit " + login, pusher);
            break;
        }
        std::cout << "Enter command" << std::endl;
    }
}

void process_server(zmq::socket_t &puller)
{
    while (1)
    {
        std::string command = "";
        std::string recieved_message = receive_message(puller);
        for (char i : recieved_message) {
            if (i != ' ') {
                command += i;
            } else {
                break;
            }
        }
        if (command == "send") {
            int i;
            std::string recipient = "", sender = "", mes_to_me = "";
            for(i = 5; i < recieved_message.size(); ++i){
                if(recieved_message[i] != ' '){
                    recipient += recieved_message[i];
                } else{
                    break;
                }
            }
            ++i;
            for(i; i < recieved_message.size(); ++i){
                if(recieved_message[i] != ' '){
                    sender += recieved_message[i];
                } else{
                    break;
                }
            }
            ++i;
            for(i; i < recieved_message.size(); ++i){
                mes_to_me += recieved_message[i];
            }
            std::cout << "Message from " << sender << ":" << std::endl << mes_to_me << std::endl;
        }else if (command == "history") {
            std::string history;
            for(int i = 8; i < recieved_message.size(); ++i){
                history += recieved_message[i];
            }
            std::cout << history << std::endl;
        }else if (command == "no") {
            std::cout << "We didn`t find this user" << std::endl;
        } else if (command == "exit") {
            break;
        }
    }
}

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket_for_login(context, ZMQ_REQ);

    socket_for_login.connect("tcp://localhost:4042");
    std::cout << "Enter login: " << std::endl;
    std::string login = "";
    std::cin >> login;
    send_message(std::to_string(getpid()) + " " + login, socket_for_login);

    std::string recieved_message = receive_message(socket_for_login);
    if (recieved_message == "0") {
        std::cout << "login is already used" << std::endl;
        _exit(0);
    } else if (recieved_message == "1") {
        zmq::context_t context1(1);
        zmq::socket_t puller(context1, ZMQ_PULL);
        puller.connect("tcp://localhost:3" + std::to_string(getpid()));
        zmq::context_t context2(1);
        zmq::socket_t pusher(context2, ZMQ_PUSH);
        pusher.connect("tcp://localhost:3" + std::to_string(getpid() + 1));
        std::thread thr[1];
        thr[0] = std::thread(process_server, std::ref(puller));
        thr[0].detach();
        process_terminal(pusher, login);
        thr[0].join();
        context1.close();
        context2.close();
        puller.disconnect("tcp://localhost:3" + std::to_string(getpid()));
        pusher.disconnect("tcp://localhost:3" + std::to_string(getpid() + 1));
    }
    context.close();
    socket_for_login.disconnect("tcp://localhost:4042");
    return 0;
}
