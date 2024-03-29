// Copyright(c) 2022-present, Salvatore Cuzzilla (Swisscom AG)
// Distributed under the MIT License (http://opensource.org/licenses/MIT)


// mdt-dialout-collector Library headers
#include "zmq_delivery.h"
#include "../bridge/grpc_collector_bridge.h"
#include <zmq.hpp>


ZmqDelivery::ZmqDelivery()
{
    spdlog::get("multi-logger")->debug("constructor: ZmqDelivery()");
    this->set_zmq_transport_uri();
}

bool ZmqPush::ZmqPusher(
    DataWrapper &data_wrapper,
    zmq::socket_t &zmq_sock,
    const std::string &zmq_transport_uri)
{
    grpc_payload *pload;

    InitGrpcPayload(
        &pload,
        data_wrapper.get_event_type().c_str(),
        data_wrapper.get_serialization().c_str(),
        data_wrapper.get_writer_id().c_str(),
        data_wrapper.get_telemetry_node().c_str(),
        data_wrapper.get_telemetry_port().c_str(),
        data_wrapper.get_telemetry_data().c_str());

    // Message Buff preparation
    // PUSH-ing only the pointer to the data-struct
    const size_t size = sizeof(grpc_payload *);
    zmq::message_t message(&pload, size);

    try {
        zmq_sock.send(message, zmq::send_flags::dontwait);
        spdlog::get("multi-logger")->
            info("[ZmqPusher] data-delivery: "
                "message successfully sent");
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));
    } catch(const zmq::error_t &zex) {
        spdlog::get("multi-logger")->
            error("[ZmqPusher] data-delivery issue: "
            "{}", zex.what());
        return false;
    }

    return true;
}

void ZmqPull::ZmqPoller(
    zmq::socket_t &zmq_sock,
    const std::string &zmq_transport_uri)
{
    // Message Buff preparation
    // POLL-ing only the pointer to the data-struct
    const size_t size = sizeof(grpc_payload *);
    zmq::message_t message(size);

    try {
        auto res = zmq_sock.recv(message, zmq::recv_flags::none);
        if (res.value() != 0) {
            spdlog::get("multi-logger")->
                info("[ZmqPoller] data-delivery: "
                    "message successfully received");
            grpc_payload *pload = *(grpc_payload **) message.data();
            std::cout << "PULL-ing from " << zmq_transport_uri << ": "
                << pload->event_type
                << " "
                << pload->serialization
                << " "
                << pload->writer_id
                << " "
                << pload->telemetry_node
                << " "
                << pload->telemetry_port
                << " "
                << pload->telemetry_data
                << "\n";
            free_grpc_payload(pload);
            //std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    } catch(const zmq::error_t &zex) {
        spdlog::get("multi-logger")->
            error("[ZmqPoller] data-delivery issue: "
            "{}", zex.what());
        std::exit(EXIT_FAILURE);
    }
}

