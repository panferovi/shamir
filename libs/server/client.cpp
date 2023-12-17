#include "server/client.h"
#include "utils/macros.h"

namespace shagit {

void Client::Connect()
{
    auto *shagit_ip = std::getenv("SHAGIT_IP");
    auto ip = boost::asio::ip::address::from_string(shagit_ip);
    boost::asio::ip::tcp::endpoint endpoint(ip, Session::PORT);
    session_.GetSocket().connect(endpoint);
}

void Client::SendRequest(Request request)
{
    switch (request) {
        case Request::LIST_HUBS:
            ListHubs();
            break;
        case Request::CREATE_HUB:
            CreateHub();
            break;
        default:
            UNREACHABLE();
    }
}

std::string Client::GetLine(const std::string &out)
{
    std::string line;
    std::cout << out;
    std::getline(std::cin, line, '\n');
    line += Session::DELIM;
    return line;
}

void Client::ListHubs()
{
    std::string req;
    req += std::to_string(Request::LIST_HUBS) + Session::DELIM;
    session_.Write(req);

    auto hubs = session_.Read();
    std::cout << hubs << std::endl;
}

std::string Client::CreateHub()
{
    std::string req;
    req += GetLine("Project name: ");
    req += GetLine("Shamir Secret: ");
    req += GetLine("Access number: ");
    req += GetLine("Owner:\n\tName: ");
    req += GetLine("\tMail: ");
    req += std::to_string(Request::CREATE_HUB) + Session::DELIM;

    session_.Write(req);

    return req;
}

}  // namespace shagit
