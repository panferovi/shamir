#include "server/client.h"

int main()
{
    shamir::Client client;
    client.Connect();

    client.SendRequest(shamir::Request::CREATE_HUB);

    return 0;
}
