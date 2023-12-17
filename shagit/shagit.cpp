#include "server/client.h"

int main(int argc, char **argv)
{
    shagit::Client client;
    client.Connect();

    client.SendRequest(shagit::Request::LIST_HUBS);

    return 0;
}
