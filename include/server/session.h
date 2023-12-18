#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>

#include "utils/macros.h"

namespace shagit {

enum Request { LIST_HUBS, CREATE_HUB, JOIN_HUB, APPROVE_JOIN, GET_HUB, CREATE_CR, APPROVE_CR, ECHO_TEST, UNKNOWN };

class Session {
public:
    static constexpr uint16_t PORT = 1234;
    static constexpr char DELIM = '\n';

    Session(boost::asio::io_service &io_service) : socket_(io_service) {}

    boost::asio::ip::tcp::socket &GetSocket()
    {
        return socket_;
    }

    void Write(std::string data)
    {
        boost::system::error_code error;
        size_t data_size = data.size();
        boost::asio::write(socket_, boost::asio::buffer(&data_size, sizeof(data_size)), error);
        boost::asio::write(socket_, boost::asio::buffer(data), error);
        if (error) {
            LOG_DEBUG("Failed to send request: %s\n error: %s\n", data.c_str(), error.message().c_str());
            return;
        }
    }

    void GoodWrite(std::string data)
    {
        boost::system::error_code error;
        size_t data_size = data.size();
        boost::asio::write(socket_, boost::asio::buffer(&data_size, sizeof(data_size)), error);
        boost::asio::write(socket_, boost::asio::buffer(data), error);
        if (error) {
            LOG_DEBUG("Failed to send request: %s\n error: %s\n", data.c_str(), error.message().c_str());
            return;
        }
    }

    std::string Read()
    {
        boost::asio::streambuf buf;
        boost::system::error_code error;
        size_t read_size = sizeof(size_t);
        size_t readed = boost::asio::read(socket_, buf.prepare(read_size), error);
        if (error) {
            LOG_DEBUG("Failed to handle request: %s\n", error.message().c_str());
            return "";
        }
        read_size = *boost::asio::buffer_cast<const size_t *>(buf.data());
        readed = boost::asio::read(socket_, buf.prepare(read_size), error);
        if (error) {
            LOG_DEBUG("Failed to handle request: %s\n", error.message().c_str());
            return "";
        }
        return std::string(boost::asio::buffer_cast<const char *>(buf.data()), readed);
    }

    std::string GoodRead()
    {
        boost::asio::streambuf buf;
        boost::system::error_code error;
        size_t read_size = sizeof(size_t);
        size_t readed = boost::asio::read(socket_, buf.prepare(read_size), error);
        if (error) {
            LOG_DEBUG("Failed to handle requestaaaa: %s\n", error.message().c_str());
            return "";
        }
        read_size = *boost::asio::buffer_cast<const size_t *>(buf.data());
        readed = boost::asio::read(socket_, buf.prepare(read_size), error);
        if (error) {
            LOG_DEBUG("Failed to handle requestbbbb: %s\n", error.message().c_str());
            return "";
        }
        return std::string(boost::asio::buffer_cast<const char *>(buf.data()), readed);
    }

private:
    boost::asio::ip::tcp::socket socket_;
};

}  // namespace shagit

#endif  // SERVER_SESSION_H
