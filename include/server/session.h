#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>

#include "utils/macros.h"

namespace shagit {

enum Request { LIST_HUBS, CREATE_HUB, JOIN_HUB, APPROVE_JOIN, GET_HUB, CREATE_CR, APPROVE_CR, UNKNOWN };

class Session {
public:
    static constexpr uint16_t PORT = 1234;
    static constexpr char DELIM = '$';

    Session(boost::asio::io_service &io_service) : socket_(io_service) {}

    boost::asio::ip::tcp::socket &GetSocket()
    {
        return socket_;
    }

    void Write(const std::string &data)
    {
        boost::system::error_code error;
        boost::asio::write(socket_, boost::asio::buffer(data + DELIM), error);
        if (error) {
            LOG_DEBUG("Failed to send request: %s\n error: %s\n", data.c_str(), error.message().c_str());
            return;
        }
    }

    std::string Read()
    {
        boost::asio::streambuf buf;
        boost::system::error_code error;
        boost::asio::read_until(socket_, buf, Session::DELIM, error);
        if (error) {
            LOG_DEBUG("Failed to handle request: %s\n", error.message().c_str());
            return "";
        }

        return boost::asio::buffer_cast<const char *>(buf.data());
    }

private:
    boost::asio::ip::tcp::socket socket_;
};

}  // namespace shagit

#endif  // SERVER_SESSION_H
