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

void Client::SendRequest(Args args)
{
    switch (args.request) {
        case Request::LIST_HUBS:
            ListHubs();
            break;
        case Request::CREATE_HUB:
            CreateHub();
            break;
        case Request::JOIN_HUB:
            JoinHub();
            break;
        case Request::APPROVE_JOIN:
            ApproveJoin();
            break;
        case UNKNOWN:
            return;
        default:
            UNREACHABLE();
    }
}

void Client::ListHubs()
{
    std::string request;
    request += std::to_string(Request::LIST_HUBS);
    session_.Write(request);

    auto hubs = session_.Read();
    std::cout << hubs << std::endl;
}

void Client::CreateHub()
{
    std::string request;
    request += GetLine("Project name: ");
    request += GetLine("Shamir Secret: ");
    request += GetLine("Access number: ");
    request += GetLine("Owner:\n\tName: ");
    request += GetLine("\tMail: ");
    request += std::to_string(Request::CREATE_HUB);

    session_.Write(request);
}

void Client::JoinHub()
{
    std::string request;
    request += GetLine("Project Id: ");
    request += GetLine("Participant:\n\tName: ");
    request += GetLine("\tMail: ");
    request += std::to_string(Request::JOIN_HUB);

    session_.Write(request);
}

void Client::ApproveJoin()
{
    std::string request;
    request += GetLine("Shamir secret: ");
    request += GetLine("Project Id: ");
    request += GetLine("PR Id: ");
    request += std::to_string(Request::APPROVE_JOIN);

    session_.Write(request);

    auto res = session_.Read();
    std::cout << res << std::endl;
}

std::string Client::GetLine(const std::string &out)
{
    std::string line;
    std::cout << out;
    std::getline(std::cin, line, '\n');
    line += Session::DELIM;
    return line;
}

}  // namespace shagit
