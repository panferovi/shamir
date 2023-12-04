#include "server/client.h"
#include "utils/macros.h"

namespace shamir {

void Client::Connect()
{
    auto ip = boost::asio::ip::address::from_string("127.0.0.1");
    boost::asio::ip::tcp::endpoint endpoint(ip, Session::PORT);
    session_.GetSocket().connect(endpoint);
}

void Client::SendRequest(Request request)
{
    auto req = std::to_string(request) + '\n';
    boost::system::error_code error;
    boost::asio::write(session_.GetSocket(), boost::asio::buffer(req), error);
    if (error) {
        LOG_DEBUG("Failed to send request: %s\n", error.message().c_str());
        return;
    }
}

}  // namespace shamir
