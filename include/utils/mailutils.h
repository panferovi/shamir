#ifndef MAILUTILS_H
#define MAILUTILS_H

#include <string>
#include <cstdlib>

namespace shagit {

void SendMail(std::string header, std::string body, std::string mail)
{
    std::string command = "printf \"" + body + "\" | mail -s \"" + header + "\" " + mail;
    std::system(command.c_str());
}

}  // namespace shagit

#endif  // MAILUTILS_H
