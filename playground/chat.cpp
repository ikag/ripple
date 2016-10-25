#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "zmq.hpp"

using namespace std;

void listen(zmq::context_t& ctx) {
    try {
        zmq::socket_t subscriber(ctx, ZMQ_SUB);

        // TODO: Try to connect to every IP in the same subnet
        // e.g. if network adapter address is 192.168.0.2, connect to
        // 192.168.0.2, 192.168.0.3, 192.168.0.4, etc.
        subscriber.connect("tcp://localhost:9000");
        subscriber.setsockopt(ZMQ_SUBSCRIBE, "", strlen(""));

        cout << "[thread] started listening thread..." << endl;

        while (true) {
            zmq::message_t update;
            subscriber.recv(&update);

            int number;
            std::istringstream iss(static_cast<char*>(update.data()));
            iss >> number;

            cout << "[thread] received number: " << number << endl;
        }

        cout << "[thread] terminated" << endl;
    } catch (const zmq::error_t &e) {
        cerr << "[thread] terminated because of an error: " << e.what() << endl;
    }
}

int main(int argc, char *argv[]) {
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:9000");

    thread t(listen, ref(context));
    t.detach();

    srandom((unsigned) time (NULL));

    for (int i = 0; i < 3; i++) {
        //  Send message to all subscribers
        zmq::message_t message(20);
        snprintf ((char *) message.data(), 20, "%d", (int) random());

        publisher.send(message);
        cout << "message published: " << message.data() << endl;

        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    return 0;
}
