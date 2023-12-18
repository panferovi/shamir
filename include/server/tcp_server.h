#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/asio.hpp>

#include "threadpool/thread_pool.h"
#include "server/session.h"
#include "storage/storage.h"

namespace shagit {

class TCPServer {
public:
    void Start();
    void Stop();

    void Close();
    bool IsClosed() const;

    uint64_t GetConnections() const;

private:
    void StartSession();
    void ServerLoop(Session *session, const boost::system::error_code &error);
    void HandleConnection(Session *session);
    void HandleRequest(Session *session);

    void ListHubs(Session *session);
    void CreateHub(const std::vector<std::string> &data);
    void JoinHub(const std::vector<std::string> &data);
    void ApproveJoin(Session *session, const std::vector<std::string> &data);
    void GetHub(Session *session);

    std::vector<std::string> SplitData(const std::string &str, char separator);

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::endpoint endpoint_ {boost::asio::ip::tcp::v4(), Session::PORT};
    boost::asio::ip::tcp::acceptor acceptor_ {io_service_, endpoint_};
    ThreadPool thread_pool_ {8};
    Session *last_session_ {nullptr};
    std::atomic<uint64_t> connections_ {0};
    std::atomic<bool> is_closed_ {false};

    HubStorage storage_;
};

}  // namespace shagit

#endif  // TCP_SERVER_H
