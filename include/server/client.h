#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include "server/session.h"
#include "storage/storage.h"
#include <optional>

namespace shagit {

struct Args {
    Request request;
    std::optional<HubStorage::Id> storage_id;
};

class Client {
public:
    void Connect();
    void SendRequest(Args args);

private:
    static constexpr std::string_view STORAGE_DIR_VAR = "STORAGE_DIR";

    // TODO(panferovi): remove from this class
    std::string GetLine(const std::string &out);

    void ListHubs();
    void CreateHub();
    void JoinHub();
    void ApproveJoin();
    void GetHub(Args args);

    boost::asio::io_service io_service_;
    Session session_ {io_service_};
};

}  // namespace shagit

#endif  // SERVER_CLIENT_H
