#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <boost/asio.hpp>

namespace shamir {

class Session {
public:
    static constexpr uint16_t PORT = 1234;

    Session(boost::asio::io_service &io_service) : socket_(io_service) {}

    boost::asio::ip::tcp::socket &GetSocket()
    {
        return socket_;
    }

private:
    boost::asio::ip::tcp::socket socket_;
};

}  // namespace shamir

#endif  // SERVER_SESSION_H
