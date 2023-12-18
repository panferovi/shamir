#include <cstdlib>

#include "storage/storage.h"
#include "utils/macros.h"

namespace shagit {

HubStorage::HubStorage() : storage_dir_(std::getenv(STORAGE_DIR_VAR.data()))
{
    ASSERT_PRINT(!storage_dir_.path().empty(), "The STORAGE_DIR environment variable is not set");
    ASSERT_PRINT(storage_dir_.exists(), "The Storage directory is not exists");
}

std::vector<HubStorage::Id> HubStorage::CreateHub(const HubInfo &hub_info)
{
    auto hub_id = GenerateId(hub_info.proj_name);
    fs::path hub_path(storage_dir_.path() / std::to_string(hub_id));

    fs::create_directories(hub_path / hub_info.proj_name);
    fs::create_directory(hub_path / CHANGE_REQ);
    fs::create_directory(hub_path / PARTICIPANTS_REQ);

    std::ofstream meta_info(hub_path / METAFILE);
    meta_info << "Project name: " << hub_info.proj_name << std::endl;
    meta_info << "Project Id: " << hub_id << std::endl;
    meta_info << SECRET << hub_info.secret << std::endl;
    meta_info << "Access number: " << hub_info.access_number << std::endl;
    meta_info << OWNER << std::endl;
    meta_info << NAME << hub_info.owner.name << std::endl;
    meta_info << MAIL << hub_info.owner.mail << std::endl;
    meta_info << "Participants:" << std::endl;

    std::vector<Id> participant_ids;
    for (auto &&[name, mail] : hub_info.participants) {
        auto id = GenerateId(name);
        meta_info << "Id: " << id << std::endl;
        meta_info << NAME << name << std::endl;
        meta_info << MAIL << mail << std::endl << std::endl;
        participant_ids.push_back(id);
    }
    participant_ids.push_back(hub_id);

    return participant_ids;
}

HubStorage::Id HubStorage::JoinHub(HubStorage::Id hub_id, const ParticipantInfo &participant)
{
    auto pr_id = GenerateId(participant.name);
    fs::path pr_path(storage_dir_.path() / std::to_string(hub_id) / PARTICIPANTS_REQ / ("PR_" + std::to_string(pr_id)));

    std::ofstream pr_info(pr_path);
    ASSERT(pr_info.is_open());

    pr_info << "Id: " << pr_id << std::endl
            << "Name: " << participant.name << std::endl
            << "Mail: " << participant.mail << std::endl;

    return pr_id;
}

ParticipantInfo HubStorage::GetParticipant(HubStorage::Id hub_id, HubStorage::Id pr_id)
{
    fs::path pr_path(storage_dir_.path() / std::to_string(hub_id) / PARTICIPANTS_REQ / ("PR_" + std::to_string(pr_id)));

    std::ifstream pr_info(pr_path);
    ASSERT(pr_info.is_open());

    ParticipantInfo participant;

    std::string line;
    std::getline(pr_info, line);
    std::getline(pr_info, line);
    participant.name = line.substr(NAME.length());
    std::getline(pr_info, line);
    participant.mail = line.substr(MAIL.length());
    return participant;
}

void HubStorage::ApproveJoin(HubStorage::Id hub_id, HubStorage::Id pr_id)
{
    fs::path pr_path(storage_dir_.path() / std::to_string(hub_id) / PARTICIPANTS_REQ / ("PR_" + std::to_string(pr_id)));

    std::ifstream pr_info(pr_path);
    ASSERT(pr_info.is_open());

    std::string id;
    std::getline(pr_info, id);
    std::string name;
    std::getline(pr_info, name);
    std::string mail;
    std::getline(pr_info, mail);

    fs::path hub_path(storage_dir_.path() / std::to_string(hub_id));
    std::fstream hub_info(hub_path / METAFILE, hub_info.app);
    ASSERT(hub_info.is_open());

    hub_info << id << std::endl << name << std::endl << mail << std::endl << std::endl;

    fs::remove(pr_path);
}

void HubStorage::ApproveCR(HubStorage::Id cr_id) {}

void HubStorage::ListHubs(std::stringstream &ss)
{
    auto visitor = [&ss](fs::path hub_id, DirEntry proj) {
        auto proj_name = proj.path().filename();
        if (proj_name != METAFILE && proj_name != CHANGE_REQ && proj_name != PARTICIPANTS_REQ) {
            ss << "Project Name: " << proj.path().filename() << " Project ID: " << hub_id.filename() << std::endl;
        }
    };
    TraverseStorage(visitor);
}

bool HubStorage::CheckSecret(HubStorage::Id hub_id, int64_t secret)
{
    fs::path hub_path(storage_dir_.path() / std::to_string(hub_id));
    std::ifstream meta_info(hub_path / METAFILE);

    std::string line;
    while (std::getline(meta_info, line)) {
        if (line.find(SECRET) != std::string::npos) {
            return secret == std::stoll(line.substr(SECRET.length()));
        }
    }
    return false;
}

ParticipantInfo HubStorage::GetOwner(HubStorage::Id hub_id)
{
    fs::path hub_path(storage_dir_.path() / std::to_string(hub_id));
    std::ifstream meta_info(hub_path / METAFILE);

    std::string line;
    while (std::getline(meta_info, line)) {
        if (line.find(OWNER) != std::string::npos) {
            ParticipantInfo owner;
            std::getline(meta_info, line);
            owner.name = line.substr(NAME.length());
            std::getline(meta_info, line);
            owner.mail = line.substr(MAIL.length());
            return owner;
        }
    }
    return ParticipantInfo {};
}

void HubStorage::TraverseStorage(const StorageVisitor &visitor)
{
    for (auto &id : fs::directory_iterator(storage_dir_)) {
        for (auto &proj : fs::directory_iterator(id)) {
            visitor(id.path(), proj);
        }
    }
}

HubStorage::Id HubStorage::GenerateId(const std::string &src)
{
    int64_t cur_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::size_t h1 = std::hash<std::string> {}(src);
    std::size_t h2 = std::hash<int64_t> {}(cur_time);
    return h1 ^ (h2 << 1);
}

}  // namespace shagit
