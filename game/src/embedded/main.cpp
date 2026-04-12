#include "pch.h"
#include "ServerApp.h"
#include "ClientApp.h"

int main()
{
	ServerApp serverApp;
	serverApp.run();

	ClientApp clientApp;
	clientApp.run();
    return 0;
}