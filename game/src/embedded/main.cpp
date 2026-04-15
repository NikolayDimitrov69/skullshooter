#include "client/pch.h"
#include "server/ServerApp.h"
#include "client/ClientApp.h"

int main()
{
	// this is not really how the server and client should be run, but for testing purposes it is fine
	ServerApp serverApp;
	serverApp.run();

	ClientApp clientApp;
	clientApp.run();
    return 0;
}