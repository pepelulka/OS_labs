#include <iostream>

#include <unistd.h>

#include "zmq.hpp"

int main() {
    zmq::message_t msg("A");
    zmq::context_t context(1);
    zmq::socket_t socket1(context, zmq::socket_type::push);
    zmq::socket_t socket2(context, zmq::socket_type::pull);
    socket2.bind("tcp://localhost:5555");
    socket1.connect("tcp://localhost:5555");
    // socket1.send(msg, zmq::send_flags::none);
    sleep(0.5);
    zmq::message_t m;
    std::vector<zmq::pollitem_t> items = {
        { static_cast<void*>(socket2), 0, ZMQ_POLLIN, 0 }
    };
    int result = zmq::poll(items, 1000);
    std::cout << result << std::endl;
    if (result == 1)
        socket2.recv(m, zmq::recv_flags::none);
        std::cout << m.to_string() << std::endl;
}