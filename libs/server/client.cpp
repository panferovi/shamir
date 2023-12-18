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
        case GET_HUB:
            GetHub(args);
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
    request += std::to_string(Request::LIST_HUBS) + Session::DELIM;
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
    auto part_cnt = std::stol(GetLine("Initial Participants Count: "));
    for (size_t i = 0; i < part_cnt; ++i) {
        request += GetLine("Participant:\n\tName: ");
        request += GetLine("\tMail: ");
    }
    request += std::to_string(Request::CREATE_HUB) + Session::DELIM;

    session_.Write(request);
}

void Client::JoinHub()
{
    std::string request;
    request += GetLine("Project Id: ");
    request += GetLine("Participant:\n\tName: ");
    request += GetLine("\tMail: ");
    request += std::to_string(Request::JOIN_HUB) + Session::DELIM;

    session_.Write(request);
}

void Client::ApproveJoin()
{
    std::string request;
    request += GetLine("Shamir secret: ");
    request += GetLine("Project Id: ");
    request += GetLine("PR Id: ");
    request += std::to_string(Request::APPROVE_JOIN) + Session::DELIM;

    session_.Write(request);

    auto res = session_.Read();
    std::cout << res << std::endl;
}

void Client::GetHub(Args args)
{
    DirGuard guard;
    std::error_code error;
    fs::path current_path(std::getenv(STORAGE_DIR_VAR.data()));
    current_path /= std::to_string(*args.storage_id);
    fs::create_directories(current_path, error);
    fs::current_path(current_path, error);
    if (error) {
        std::cout << "Troubles with dirs.\n";
        return;
    }

    // session_.Write(std::to_string(Request::GET_HUB) + Session::DELIM);

    std::string filename = session_.Read();
    while (!filename.length()) {
        fs::path dir_path(filename);
        dir_path.remove_filename();
        fs::create_directories(dir_path);
        std::string file = session_.Read();
        std::fstream file_stream(filename, std::ios::binary | std::ios::out);
        file_stream.write(file.data(), file.max_size());
        filename = session_.Read();
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

}  // namespace shagit
