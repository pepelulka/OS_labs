//  Hello World client
#include "zmq.hpp"
#include <string>
#include <iostream>
#include <thread>

#include "base.hpp"

int64_t Compute(const std::vector<int64_t> &data) {
    int64_t result = 0;
    for (const auto i : data) {
        result += i;
    }
    return result;
}

void ManagingRoutine(int) {
    // printf ("Connecting to hello world server...\n");
    zmq::context_t context (2);
    zmq::socket_t socket (context, zmq::socket_type::req);
    socket.connect("tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        printf ("Sending Hello %d...\n", request_nbr);

        zmq::message_t msg1(std::string("Hello"));
        socket.send(msg1, zmq::send_flags::none);
        zmq::message_t msg2(10);
        auto res = socket.recv(msg2);
        *res;
        printf ("Received World %d\n", request_nbr);
    }
    socket.close();
    context.close();
}

using namespace lab5;

int main (int argc, char ** argv) {
    NodeId id = std::atoi(argv[1]), port = std::atoi(argv[2]);


    return 0;
}