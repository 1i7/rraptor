#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

#include <DNETcK.h>
#include <DWIFIcK.h>

void printIPAddress(IPv4 *ipAddress);

void printNetworkStatus();

void printTcpClientStatus(TcpClient *tcpClient);

void printDNETcKStatus(DNETcK::STATUS status);

#endif // NETWORK_UTIL_H

