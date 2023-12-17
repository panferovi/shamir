#ifndef STORAGE_H
#define STORAGE_H

#include <sstream>
#include <fstream>
#include <string_view>
#include <filesystem>
#include <functional>

namespace shagit {

namespace fs = std::filesystem;

/**
 * Storage structure:
 *
 *  Storage Dir:
 * |
 * |     (Proj ID)            (Proj Name)
 * ----- 983912839182391283 - Shamir - Project src tree
 * |                          |
 * |                          |
 * |                          ----- METAFILE
 * |                          |
 * |                          |
 * |                          ----- ChangeRequests
 * |                          |     |
 * |                          |     |
 * |                          |     ----- cr_1 - CR src tree
 * |                          |     |
 * |                          |     |
 * |                          |     ----- cr_2
 * |                          |
 * |                          ----- ParticipantRequests
 * |                                |
 * |                                |
 * |                                ----- pr_1
 * |                                |
 * |                                |
 * |                                ----- pr_2
 * |
 * ----- 912391293123812312 - proj2
 * |
 * |
 * ----- 123123123412412323 - proj3
 *
 *
 *
 * METAFILE:
 *      Project name: Shamir
 *      Project Id: 983912839182391283
 *      Secret: 78123
 *      Access number: 2
 *      Owner:
 *          Name: Panferov Ivan
 *          Mail: panferov.ia@phystech.edu
 *      Participants:
 *          Name: Vyazovtsev Andrey
 *          Mail: vyazovtsev.av@phystech.edu
 *
 *          Name: Kasimov Anvar
 *          Mail: kasimov.ar@phystech.edu
 *
 *
 *
 * PR_ID:
 *      Participant:
 *          Name: Grigoryev Ivan
 *          Mail: gr.ivan.alex@yandex.ru
 */

struct ProjectInfo {
    std::string proj_name;
    int64_t secret;
    size_t access_number;
    std::string owner_name;
    std::string owner_mail;
};

class ProjectStorage {
public:
    using Id = uint64_t;
    using DirEntry = fs::directory_entry;
    using StorageVisitor = std::function<void(fs::path, const DirEntry &)>;

    static constexpr std::string_view STORAGE_DIR_VAR = "STORAGE_DIR";
    static constexpr std::string_view CHANGE_REQ = "ChangeRequests";
    static constexpr std::string_view PARTICIPANTS_REQ = "ParticipantsRequests";
    static constexpr std::string_view METAFILE = "METAINFO";

    explicit ProjectStorage();

    Id CreateProject(const ProjectInfo &proj_info);
    Id CreatePR(Id proj_id, std::string name, std::string mail);
    void ApprovePR(Id proj_id, Id pr_id);
    // Id CreateCR(Id proj_id, ...);
    void ApproveCR(Id cr_id);
    void ListProjects(std::stringstream &ss);

    void TraverseStorage(const StorageVisitor &visitor);

private:
    Id GenerateId(const std::string &src);

    DirEntry storage_dir_;
};

}  // namespace shagit

#endif  // STORAGE_H
