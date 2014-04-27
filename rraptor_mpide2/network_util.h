#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

/**
 * Подключиться к открытой сети WiFi.
 */
int connectWifiOpen(const char* ssid, DNETcK::STATUS *netStatus);
/**
 * Подключиться к сети WiFi, защищенной WPA2 с паролем.
 */
int connectWifiWPA2Passphrase(const char* ssid, const char* passphrase, DNETcK::STATUS *netStatus);

void printIPAddress(IPv4 *ipAddress);

void printNetworkStatus();

void printTcpClientStatus(TcpClient *tcpClient);

void printStatus(DNETcK::STATUS status);

#endif // NETWORK_UTIL_H

