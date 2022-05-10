#include <zmq.hpp>
#include <unistd.h>
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    zmq::context_t context(1);                   //создаём контекст
    zmq::socket_t main_socket(context, ZMQ_REP); //создание сокета - в аргументах контекст и тип.
    main_socket.setsockopt(ZMQ_RCVTIMEO, 3000);  //ставим время ожидания <= 3000ms
    string adr = "tcp://127.0.0.1:300";
    string command;
    int child_id = 0; //у нас пока нет ребёнка
    cout << "Commands:\ncreate id\nexec id k n1...nk\nping id\nkill id\nexit\n";
    while (1)
    {
        cout << "Please, enter command\n";
        cin >> command;
        if (command == "create")
        {
            if (child_id == 0) //если нет ребёнка
            {
                int id;
                cin >> id;
                while (id == 0)
                {
                    std::cout << "This ID is permitted, insert another one\n";
                    std::cin >> id;
                }
                int id_tmp = id - 1;
                main_socket.bind(adr + to_string(++id_tmp));
                string new_adr = adr + to_string(id_tmp);
                char *adr_ = new char[new_adr.size() + 1];
                memcpy(adr_, new_adr.c_str(), new_adr.size() + 1);
                char *id_ = new char[to_string(id).size() + 1];
                memcpy(id_, to_string(id).c_str(), to_string(id).size() + 1);
                char *args[] = {"./child_node", adr_, id_, NULL}; //adr_ - куда подключаться ребёнку, id_ - имя ребёнка (которое он сохранит)
                int id2 = fork();
                if (id2 == -1)
                {
                    std::cout << "ERROR: CALCULATING NODE WAS NOT CREATED\n";
                    id = 0;
                    exit(1);
                }
                else if (id2 == 0)
                {
                    execv("./child_node", args);
                }
                else
                {
                    child_id = id;
                }
                zmq::message_t message;
                main_socket.recv(&message); //получаем сообщение от ребенка
                string recieved_message(static_cast<char *>(message.data()), message.size());
                cout << recieved_message << "\n";
                delete[] adr_;
                delete[] id_;
            }
            else
            {
                int id;
                cin >> id;
                while (id == 0)
                {
                    std::cout << "This ID is permitted, insert another one\n";
                    std::cin >> id;
                }
                string message_string = command + " " + to_string(id);
                zmq::message_t message(message_string.size());
                memcpy(message.data(), message_string.c_str(), message_string.size());
                main_socket.send(message);
                main_socket.recv(&message);
                string recieved_message(static_cast<char *>(message.data()), message.size());
                cout << recieved_message << "\n";
            }
        }
        else if (command == "exec")
        {
            int id, N;
            cin >> id >> N;
            string s;
            vector<int> v(N);
            for (int i = 0; i < N; ++i)
            {
                cin >> v[i];
                s = s + to_string(v[i]) + '$'; //exec 5 4 3$22$1$4$
            }
            string message_string = command + " " + to_string(id) + " " + s;
            zmq::message_t message(message_string.size());
            memcpy(message.data(), message_string.c_str(), message_string.size());
            main_socket.send(message);
            main_socket.recv(&message);
            string recieved_message(static_cast<char *>(message.data()), message.size());
            cout << recieved_message << "\n";
        }
        else if (command == "ping")
        {
            int id;
            cin >> id;
            string message_string = command + " " + to_string(id);
            zmq::message_t message(message_string.size());
            memcpy(message.data(), message_string.c_str(), message_string.size());
            main_socket.send(message);
            // receive answer from child
            main_socket.recv(&message);
            string recieved_message(static_cast<char *>(message.data()), message.size());
            cout << recieved_message << "\n";
        }
        else if (command == "kill")
        {
            int id;
            cin >> id;
            if (child_id == 0)
            {
                cout << "Error: there aren't any nodes\n";
            }
            else if (child_id == id) //приказали умереть именно этой ноде
            {
                string kill_message = "DIE";
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                cout << "Tree was deleted\n";
                child_id = 0;
            }
            else
            {
                string kill_message = command + " " + to_string(id);
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                main_socket.recv(&message);
                string received_message(static_cast<char *>(message.data()), message.size());
                cout << received_message << "\n";
            }
        }
        else if (command == "exit")
        {
            if (child_id)
            {
                string kill_message = "DIE";
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                cout << "Tree was deleted\n";
                child_id = 0;
            }
            main_socket.close();
            context.close();
            return 0;
        }
        else
        {
            cout << "Error: incorrect command\n";
        }
    }
}
