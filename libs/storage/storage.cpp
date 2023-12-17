#include <cstdlib>

#include "storage/storage.h"
#include "utils/macros.h"

namespace shagit {

ProjectStorage::ProjectStorage() : storage_dir_(std::getenv(STORAGE_DIR_VAR.data()))
{
    ASSERT_PRINT(!storage_dir_.path().empty(), "The STORAGE_DIR environment variable is not set");
    ASSERT_PRINT(storage_dir_.exists(), "The Storage directory is not exists");
}

ProjectStorage::Id ProjectStorage::CreateProject(const ProjectInfo &proj_info)
{
    auto proj_id = GenerateId(proj_info.proj_name);
    fs::path proj_path(storage_dir_.path() / std::to_string(proj_id));

    fs::create_directories(proj_path / proj_info.proj_name);
    fs::create_directory(proj_path / CHANGE_REQ);
    fs::create_directory(proj_path / PARTICIPANTS_REQ);

    std::ofstream meta_info(proj_path / METAFILE);
    meta_info << "Project name: " << proj_info.proj_name << std::endl;
    meta_info << "Project Id: " << proj_id << std::endl;
    meta_info << "Shamir Secret: " << proj_info.secret << std::endl;
    meta_info << "Access number: " << proj_info.access_number << std::endl;
    meta_info << "Owner:" << std::endl;
    meta_info << "\tName: " << proj_info.owner_name << std::endl;
    meta_info << "\tMail: " << proj_info.owner_mail << std::endl;
    meta_info << "Participants:" << std::endl;

    return proj_id;
}

ProjectStorage::Id ProjectStorage::CreatePR(ProjectStorage::Id proj_id, std::string name, std::string mail)
{
    auto pr_id = GenerateId(name);
    fs::path pr_path(storage_dir_.path() / std::to_string(proj_id) / PARTICIPANTS_REQ /
                     ("PR_" + std::to_string(pr_id)));

    std::ofstream pr_info(pr_path);
    ASSERT(pr_info.is_open());

    pr_info << "Id: " << pr_id << std::endl << "Name: " << name << std::endl << "Mail: " << mail << std::endl;

    return pr_id;
}

void ProjectStorage::ApprovePR(ProjectStorage::Id proj_id, ProjectStorage::Id pr_id)
{
    fs::path pr_path(storage_dir_.path() / std::to_string(proj_id) / PARTICIPANTS_REQ /
                     ("PR_" + std::to_string(pr_id)));

    std::ifstream pr_info(pr_path);
    ASSERT(pr_info.is_open());

    std::string id;
    std::getline(pr_info, id, '\n');
    std::string name;
    std::getline(pr_info, name, '\n');
    std::string mail;
    std::getline(pr_info, mail, '\n');

    fs::path proj_path(storage_dir_.path() / std::to_string(proj_id));
    std::fstream proj_info(proj_path / METAFILE, proj_info.app);
    ASSERT(proj_info.is_open());

    proj_info << '\t' << id << std::endl << '\t' << name << std::endl << '\t' << mail << std::endl << std::endl;

    fs::remove(pr_path);
}

void ProjectStorage::ApproveCR(ProjectStorage::Id cr_id) {}

void ProjectStorage::ListProjects(std::stringstream &ss)
{
    auto visitor = [&ss](fs::path proj_id, DirEntry proj) {
        auto proj_name = proj.path().filename();
        if (proj_name != METAFILE && proj_name != CHANGE_REQ && proj_name != PARTICIPANTS_REQ) {
            ss << "Project Name: " << proj.path().filename() << " Project ID: " << proj_id.filename() << std::endl;
        }
    };
    TraverseStorage(visitor);
}

void ProjectStorage::TraverseStorage(const StorageVisitor &visitor)
{
    for (auto &id : fs::directory_iterator(storage_dir_)) {
        for (auto &proj : fs::directory_iterator(id)) {
            visitor(id.path(), proj);
        }
    }
}

ProjectStorage::Id ProjectStorage::GenerateId(const std::string &src)
{
    int64_t cur_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::size_t h1 = std::hash<std::string> {}(src);
    std::size_t h2 = std::hash<int64_t> {}(cur_time);
    return h1 ^ (h2 << 1);
}

}  // namespace shagit
