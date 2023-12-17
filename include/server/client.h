#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "server/session.h"

namespace shagit {

class Client {
public:
    void Connect();
    void SendRequest(Request request);

private:
    // TODO(panferovi): remove from this class
    std::string GetLine(const std::string &out);

    void ListHubs();
    void CreateHub();
    void JoinHub();
    void ApproveJoin();

    boost::asio::io_service io_service_;
    Session session_ {io_service_};
};

}  // namespace shagit

#endif  // SERVER_CLIENT_H
