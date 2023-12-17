#include "server/client.h"

int main(int argc, char **argv)
{
    shagit::Client client;
    client.Connect();

    client.SendRequest(shagit::Request::APPROVE_JOIN);

    return 0;
}
