#include <iostream>
#include <map>
#include "zmq.hpp"
#include <vector>
#include <cstring>
#include <memory>
#include <thread>

// g++ server.cpp -lzmq -pthread -o server -w

//хранение логинов и сокетов PULL (client) - PUSH (server)
std::map<std::string, std::shared_ptr<zmq::socket_t>> ports;

std::map<std::string, bool> logged_in;

zmq::context_t context1(1);

void history_save(std::string login_sender, std::string login_accepter, std::string message, std::map<std::string, std::map<std::string, std::string>> &history_of_messages)
{
    history_of_messages[login_sender][login_accepter] = history_of_messages[login_sender][login_accepter].append("\n" + message);
}

void send_message(std::string message_string, zmq::socket_t &socket)
{
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size());
    if (!socket.send(message_back))
    {
        std::cout << "Error" << std::endl;
    }
}

std::string receive_message(zmq::socket_t& socket) {
    zmq::message_t message_main;
    socket.recv(&message_main);
    std::string answer(static_cast<char*>(message_main.data()), message_main.size());
    return answer;
}

void process_client(int id, std::map<std::string, std::map<std::string, std::string>> &history_of_messages, std::string nickname)
{
    zmq::context_t context2(1);
    zmq::socket_t puller(context2, ZMQ_PULL);
    puller.bind("tcp://*:3" + std::to_string(id + 1));
    while (1)
    {
        std::string command = "";
        std::string client_mes = receive_message(puller);
        for (char i : client_mes) {
            if (i != ' ') {
                command += i;
            } else {
                break;
            }
        }
        int i;
        if (command == "send") {
            std::string recipient = "";
            for(i = 5; i < client_mes.size(); ++i){
                if(client_mes[i] != ' '){
                    recipient += client_mes[i];
                } else{
                    break;
                }
            }
            if(logged_in[recipient]) {
                std::string message;
                ++i;
                while(client_mes[i] != ' ') ++i;
                ++i;
                for(i; i < client_mes.size(); ++i){
                    message += client_mes[i];
                }
                send_message(client_mes, *ports[recipient]);
                history_save(nickname, recipient, message, history_of_messages);
            } else {
                ++i;
                std::string sender = "";
                for(i; i < client_mes.size(); ++i){
                    if(client_mes[i] != ' '){
                        sender += client_mes[i];
                    } else {
                        break;
                    }
                }
                send_message("no client", *ports[sender]);
            }
        }else if (command == "history") {
            std::string history;
            for(auto sender : history_of_messages){
                if(sender.first == nickname){
                    for(auto accepter : sender.second){
                        history += accepter.first + " " + accepter.second += '\n';
                    }
                }
            }
            std::cout << history << std::endl;
            send_message("history " + history, *ports[nickname]);
        }else if (command == "exit") {
            std::string sender = "";
            for(i = 5; i < client_mes.size(); ++i){
                if(client_mes[i] != ' '){
                    sender += client_mes[i];
                } else{
                    break;
                }
            }
            send_message("exit", *ports[sender]);
            logged_in[sender] = false;
        }
    }

}

int main(){
    zmq::context_t context(1);
    zmq::socket_t socket_for_login(context, ZMQ_REP);

    socket_for_login.bind("tcp://*:4042");

    std::map<std::string, std::map<std::string, std::string>> history_of_messages;

    while (1) {
        std::string recieved_message = receive_message(socket_for_login);
        std::string id_s = "";
        int i;
        for(i = 0; i < recieved_message.size(); ++i){
            if(recieved_message[i] != ' '){
                id_s += recieved_message[i];
            } else{
                break;
            }
        }
        int id = std::stoi(id_s);
        std::string nickname;
        ++i;
        for(i; i < recieved_message.size(); ++i){
            if(recieved_message[i] != ' '){
                nickname += recieved_message[i];
            } else{
                break;
            }
        }
        if(logged_in[nickname]) {
            std::cout << "This user already logged in..." << std::endl;
            send_message("0", socket_for_login);
        }
        else{
            logged_in[nickname] = true;
            std::cout << "User " << nickname << " logged in with id " << id << std::endl;
            send_message("1", socket_for_login);

            std::shared_ptr<zmq::socket_t> socket_client = std::make_shared<zmq::socket_t>(context1, ZMQ_PUSH);
            socket_client->bind("tcp://*:3" + id_s);
            ports[nickname] = socket_client;
            std::thread worker = std::thread(std::ref(process_client), id, std::ref(history_of_messages), nickname);
            worker.detach();
        }
    }
}
