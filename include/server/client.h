#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "server/session.h"

namespace shamir {

enum Request { CREATE_HUB, JOIN_HUB, LIST_HUBS, ADD_PARTICIPANT };

class Client {
public:
    void Connect();
    void SendRequest(Request request);

private:
    boost::asio::io_service io_service_;
    Session session_ {io_service_};
};

}  // namespace shamir

#endif  // SERVER_CLIENT_H
