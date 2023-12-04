#include "server/session.h"
#include "server/tcp_server.h"
#include "utils/macros.h"

namespace shamir {

using namespace boost::asio;

void TCPServer::Start()
{
    thread_pool_.Start();
    thread_pool_.Submit([this] {
        StartSession();
        io_service_.run();
    });
}

void TCPServer::Stop()
{
    ASSERT(IsClosed());
    thread_pool_.Stop();
    delete last_session_;
}

void TCPServer::Close()
{
    is_closed_ = true;
    io_service_.stop();
}

bool TCPServer::IsClosed() const
{
    return is_closed_;
}

uint64_t TCPServer::GetConnections() const
{
    return connections_;
}

void TCPServer::StartSession()
{
    if (IsClosed()) {
        return;
    }

    last_session_ = new Session(io_service_);
    acceptor_.async_accept(last_session_->GetSocket(),
                           [this, session = last_session_](const auto &error) { ServerLoop(session, error); });
}

void TCPServer::ServerLoop(Session *session, const boost::system::error_code &error)
{
    if (error) {
        LOG_DEBUG("Failed to handle connection: %s\n", error.message().c_str());
        delete session;
        return;
    }
    HandleConnection(session);
    StartSession();
}

void TCPServer::HandleConnection(Session *session)
{
    connections_++;
    thread_pool_.Submit([this, session]() {
        HandleRequest(session->GetSocket());
        delete session;
    });
}

void TCPServer::HandleRequest(ip::tcp::socket &socket)
{
    boost::system::error_code error;
    boost::asio::streambuf req;
    // TODO(panferovi): change delim to '\0'
    boost::asio::read_until(socket, req, '\n', error);
    if (error) {
        LOG_DEBUG("Failed to handle request: %s\n", error.message().c_str());
    }
    std::string data = boost::asio::buffer_cast<const char *>(req.data());
}

}  // namespace shamir
