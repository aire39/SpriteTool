#include "NetworkServices.h"

int NetworkServices::sendMessage(SOCKET cSocket, char * message, int messageSize)
{
	return send(cSocket, message, messageSize, 0);
}

int NetworkServices::recieveMessage(SOCKET cSocket, char * buffer, int bufSize)
{
	return recv(cSocket, buffer, bufSize, 0);
}