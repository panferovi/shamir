#include "server/client.h"
#include <getopt.h>

namespace shagit {
namespace {

Args ParseArgs(int argc, char **argv)
{
    int request_opt = 0, index = 0;
    option opts[] = {{"list", no_argument, &request_opt, LIST_HUBS},
                     {"create-hub", no_argument, &request_opt, CREATE_HUB},
                     {"join-hub", no_argument, &request_opt, JOIN_HUB},
                     {"approve-hub", no_argument, &request_opt, APPROVE_JOIN},
                     {"get-hub", required_argument, &request_opt, GET_HUB},
                     {"create-cr", required_argument, &request_opt, CREATE_CR},
                     {"approve-cr", required_argument, &request_opt, APPROVE_CR},
                     {"echo", no_argument, &request_opt, ECHO_TEST},
                     {nullptr, 0, nullptr, 0}};
    int getopt_result = getopt_long(argc, argv, "", opts, &index);
    if (getopt_result == -1)
        return {UNKNOWN};

    Request request = static_cast<Request>(request_opt);
    std::optional<HubStorage::Id> id = std::nullopt;
    switch (request) {
        case GET_HUB:
            id.emplace(std::stoull(argv[index]));
            break;
        case CREATE_CR:
            id.emplace(std::stoull(argv[index]));
            break;
        case APPROVE_CR:
            id.emplace(std::stoull(argv[index]));
            break;
    }
    return {request, id};
}

}  // namespace
}  // namespace shagit

int main(int argc, char **argv)
{
    shagit::Client client;
    client.Connect();

    client.SendRequest(shagit::ParseArgs(argc, argv));

    return 0;
}
