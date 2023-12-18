#include <vector>

#include "server/session.h"
#include "server/tcp_server.h"
#include "utils/macros.h"
#include "utils/mailutils.h"
#include "math/math.h"

namespace shagit {

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
        HandleRequest(session);
        delete session;
    });
}

void TCPServer::HandleRequest(Session *session)
{
    auto buf = session->Read();
    auto data = SplitData(buf, Session::DELIM);
    ASSERT(!data.empty());

    auto request = static_cast<Request>(std::atoi(data[data.size() - 1].c_str()));
    data.resize(data.size() - 1);

    switch (request) {
        case Request::LIST_HUBS:
            ListHubs(session);
            break;
        case Request::CREATE_HUB:
            CreateHub(data);
            break;
        case Request::JOIN_HUB:
            JoinHub(data);
            break;
        case Request::APPROVE_JOIN:
            ApproveJoin(session, data);
            break;
        default:
            UNREACHABLE();
    }
}

void TCPServer::ListHubs(Session *session)
{
    std::stringstream ss;
    storage_.ListHubs(ss);
    session->Write(ss.str());
}

void TCPServer::CreateHub(const std::vector<std::string> &data)
{
    HubInfo info;
    info.proj_name = data[0];
    info.secret = std::stoll(data[1]);
    info.access_number = std::stoull(data[2]);
    info.owner.name = data[3];
    info.owner.mail = data[4];
    for (size_t i = 5; i < data.size(); i += 2) {
        info.participants.push_back({data[i], data[i + 1]});
    }

    auto ids = storage_.CreateHub(info);

    auto secret_piece = math::ShareSecret(info.secret, info.access_number, info.participants.size());
    for (size_t i = 0; i < secret_piece.points_.size(); ++i) {
        SendMail("New hub", "Hi, " + info.participants[i].name + "!\nYou have been invited to the " +
                  info.proj_name + " hub.\nYour Id is " + std::to_string(ids[i]) +
                  "\nYour key is " + std::to_string(secret_piece.points_[i].key_) +
                  "\nYour secret piece is " + std::to_string(secret_piece.points_[i].value_),
                  info.participants[i].mail);
    }

}

void TCPServer::JoinHub(const std::vector<std::string> &data)
{
    HubStorage::Id hub_id = std::stoull(data[0]);
    ParticipantInfo participant = {data[1], data[2]};
    auto req_id = storage_.JoinHub(hub_id, participant);

    auto owner = storage_.GetOwner(hub_id);
    SendMail("Join Request",
             "Hi, " + owner.name + "!\nPlease, could you approve join request " + std::to_string(req_id) + " for " +
                 participant.name,
             owner.mail);
}

void TCPServer::ApproveJoin(Session *session, const std::vector<std::string> &data)
{
    int64_t secret = std::stoll(data[0]);
    HubStorage::Id hub_id = std::stoull(data[1]);

    if (!storage_.CheckSecret(hub_id, secret)) {
        session->Write("Your secret is not valid");
        return;
    }

    session->Write("Your secret is valid");

    HubStorage::Id pr_id = std::stoull(data[2]);
    auto partcipant = storage_.GetParticipant(hub_id, pr_id);
    storage_.ApproveJoin(hub_id, pr_id);

    SendMail("Join Approved",
             "Hi, " + partcipant.name + "!\nThank you for join to the hub. Your Id is " +
             std::to_string(pr_id) + "\nYour key is: ..." + "\nYour secret piece is: ...",
             partcipant.mail);
}

std::vector<std::string> TCPServer::SplitData(const std::string &str, char separator)
{
    std::vector<std::string> strings;
    int start_idx = 0, end_idx = 0;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == separator) {
            end_idx = i;
            std::string tmp;
            tmp.append(str, start_idx, end_idx - start_idx);
            strings.push_back(tmp);
            start_idx = end_idx + 1;
        }
    }
    return strings;
}

}  // namespace shagit
