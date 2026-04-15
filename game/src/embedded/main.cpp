#include "client/pch.h"
#include "server/ServerApp.h"
#include "client/ClientApp.h"

int main()
{
	ServerApp serverApp;
	serverApp.run();

	ClientApp clientApp;
	clientApp.run();
    return 0;
}