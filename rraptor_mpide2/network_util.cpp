#include "WProgram.h"

#include <DNETcK.h>
#include <DWIFIcK.h>

#include "rraptor_config.h"

/**
 * Конвертировать строковое представление адреса IPv4
 * в объект DNETcK::IPv4
 * @param ipv4_str срока с адресом ipv4 в формате "192.168.1.27".
 * @return адрес DNETcK::IPv4; DNETcK::zIPv4 (IPv4-адрес, заполенный нулями) 
 *     в случае ошибки парсинга строки.
 */
IPv4 parseIPAddress(char* ipv4_str) {
    char* ip_elem_str[4];
    int ip_elem[4];
    
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    
    // разобьем строку на куски по символу '.'
    if(ipv4_str != NULL) {
        ip_elem_str[0] = strtok_r(ipv4_str, ".", &last);
        ip_elem_str[1] = strtok_r(NULL, ".", &last);
        ip_elem_str[2] = strtok_r(NULL, ".", &last);
        ip_elem_str[3] = strtok_r(NULL, ".", &last);
    }
    IPv4 res;
    if(ipv4_str == NULL || ip_elem_str[0] == NULL || ip_elem_str[1] == NULL || 
            ip_elem_str[2] == NULL || ip_elem_str[3] == NULL) {
        // адрес некорректный, вернем нули
        res = DNETcK::zIPv4;
    } else {
        // конвертируем куски строки в целые числа
        ip_elem[0] = atoi(ip_elem_str[0]);
        ip_elem[1] = atoi(ip_elem_str[1]);
        ip_elem[2] = atoi(ip_elem_str[2]);
        ip_elem[3] = atoi(ip_elem_str[3]);

        // и склеим из них ip-адрес
        IPv4 ipv4 = {ip_elem[0],ip_elem[1],ip_elem[2],ip_elem[3]};
        res = ipv4;
    }    
    return res;
}

/**
 * Получить строковое представление адреса DNETcK::IPv4
 *
 * @param dest строка-назначение, не меньше 16 символов (включая 0 на конце)
 * @param ipv4_addr ip-адрес
 */
void sprintf_ip_address(char* dest, IPv4 *ipv4_addr) {
    sprintf(dest, "%d.%d.%d.%d", 
        ipv4_addr->rgbIP[0], ipv4_addr->rgbIP[1], 
        ipv4_addr->rgbIP[2], ipv4_addr->rgbIP[3]);
}


/**
 * Вывести текущий статус сети в строку.
 */
void sprintf_network_status(char* dest) {
    IPv4 ip_addr;
    char ip_str[16];
    
    if( DNETcK::getMyIP(&ip_addr) ) {
        sprintf_ip_address(ip_str, &ip_addr);
    } else {
        sprintf(ip_str, "not_assigned");
    }
    sprintf(dest,"ipv4_address=%s", ip_str);
    
    if( DNETcK::getDns1(&ip_addr) ) {
        sprintf_ip_address(ip_str, &ip_addr);
    } else {
        sprintf(ip_str, "not_assigned");
    }
    sprintf(dest + strlen(dest)," dns1=%s", ip_str);
    
    if( DNETcK::getDns2(&ip_addr) ) { 
        sprintf_ip_address(ip_str, &ip_addr);
    } else {
        sprintf(ip_str, "not_assigned");
    }
    sprintf(dest + strlen(dest)," dns2=%s", ip_str);
    
    if( DNETcK::getGateway(&ip_addr) ) {
        sprintf_ip_address(ip_str,&ip_addr);
    } else {
        sprintf(ip_str, "not_assigned");
    }
    sprintf(dest + strlen(dest)," gateway=%s", ip_str);
        
    if( DNETcK::getSubnetMask(&ip_addr) ) {
        sprintf_ip_address(ip_str,&ip_addr);
    } else {
        sprintf(ip_str, "not_assigned");
    }
    sprintf(dest + strlen(dest)," subnet_mask=%s", ip_str);
}

/**
 * Напечатать строковое представление адреса DNETcK::IPv4 
 * в отладочный порт.
 */
void printIPAddress(IPv4 *ipAddress) {
    Serial.print(ipAddress->rgbIP[0], DEC);
    Serial.print(".");
    Serial.print(ipAddress->rgbIP[1], DEC);
    Serial.print(".");
    Serial.print(ipAddress->rgbIP[2], DEC);
    Serial.print(".");
    Serial.print(ipAddress->rgbIP[3], DEC);
}

/**
 * Вывести текущий статус сети.
 */
void printNetworkStatus() {
    IPv4 ipAddress;
    
    if( DNETcK::getMyIP(&ipAddress) ) { 
        Serial.print("IPv4 Address: ");
        printIPAddress(&ipAddress);
        Serial.println();
    } else {
        Serial.println("IP not assigned");
    }
    
    if( DNETcK::getDns1(&ipAddress) ) { 
        Serial.print("DNS1: ");
        printIPAddress(&ipAddress);
        Serial.println();
    } else {
        Serial.println("DNS1 not assigned");
    }
    
    if( DNETcK::getDns2(&ipAddress) ) { 
        Serial.print("DNS2: ");
        printIPAddress(&ipAddress);
        Serial.println();
    } else {
        Serial.println("DNS2 not assigned");
    }
    
    if( DNETcK::getGateway(&ipAddress) ) { 
        Serial.print("Gateway: ");
        printIPAddress(&ipAddress);
        Serial.println();
    } else {
        Serial.println("Gateway not assigned");
    }
        
    if( DNETcK::getSubnetMask(&ipAddress) ) { 
        Serial.print("Subnet mask: ");
        printIPAddress(&ipAddress);
        Serial.println();
    } else {
        Serial.println("Subnet mask not assigned");
    }
}

/**
 * Вывести статус клиента TCP.
 */
void printTcpClientStatus(TcpClient *tcpClient) {
    IPEndPoint remoteEndPoint;
    if(tcpClient->getRemoteEndPoint(&remoteEndPoint)) {
        Serial.print("Remote host: ");
        printIPAddress(&remoteEndPoint.ip);
        Serial.print(":");
        Serial.println(remoteEndPoint.port);
    } else {
        Serial.println("TCP client not connected");
    }
}

/**
 * Вывести название статуса DNETcK::STATUS в виде строки.
 */
void printDNETcKStatus(DNETcK::STATUS status) {
    switch(status) {
        case DNETcK::None:                           // = 0,
            Serial.print("None");
            break;
        case DNETcK::Success:                        // = 1,
            Serial.print("Success");
            break;
        case DNETcK::UDPCacheToSmall:                // = 2,
            Serial.print("UDPCacheToSmall");
            break;
        // Initialization status
        case DNETcK::NetworkNotInitialized:          // = 10,
            Serial.print("NetworkNotInitialized");
            break;
        case DNETcK::NetworkInitialized:             // = 11,
            Serial.print("NetworkInitialized");
            break;
        case DNETcK::DHCPNotBound:                   // = 12,
            Serial.print("DHCPNotBound");
            break;
        // Epoch status
        case DNETcK::TimeSincePowerUp:               // = 20,
            Serial.print("TimeSincePowerUp");
            break;
        case DNETcK::TimeSinceEpoch:                 // = 21,
            Serial.print("TimeSinceEpoch");
            break;
        // DNS status
        case DNETcK::DNSIsBusy:                      // = 30,
            Serial.print("DNSIsBusy");
            break;
        case DNETcK::DNSResolving:                   // = 31,
            Serial.print("DNSResolving");
            break;
        case DNETcK::DNSLookupSuccess:               // = 32,
            Serial.print("DNSLookupSuccess");
            break;
        case DNETcK::DNSUninitialized:               // = 33,
            Serial.print("DNSUninitialized");
            break;
        case DNETcK::DNSResolutionFailed:            // = 34,
            Serial.print("DNSResolutionFailed");
            break;
        case DNETcK::DNSHostNameIsNULL:              // = 35,
            Serial.print("DNSHostNameIsNULL");
            break;
        case DNETcK::DNSRecursiveExit:               // = 36,
            Serial.print("DNSRecursiveExit");
            break;
        // TCP connect state machine states
        case DNETcK::NotConnected:                   // = 40,
            Serial.print("NotConnected");
            break;
        case DNETcK::WaitingConnect:                 // = 41,
            Serial.print("WaitingConnect");
            break;
        case DNETcK::WaitingReConnect:               // = 42,
            Serial.print("WaitingReConnect");
            break;
        case DNETcK::Connected:                      // = 43,
            Serial.print("Connected");
            break;
        // other connection status
        case DNETcK::LostConnect:                    // = 50,
            Serial.print("LostConnect");
            break;
        case DNETcK::ConnectionAlreadyDefined:       // = 51,
            Serial.print("ConnectionAlreadyDefined");
            break;
        case DNETcK::SocketError:                    // = 52,
            Serial.print("SocketError");
            break;
        case DNETcK::WaitingMACLinkage:              // = 53,
            Serial.print("WaitingMACLinkage");
            break;
        case DNETcK::LostMACLinkage:                 // = 54,
            Serial.print("LostMACLinkage");
            break;
        // write status
        case DNETcK::WriteTimeout:                   // = 60,
            Serial.print("WriteTimeout");
            break;
        // read status
        case DNETcK::NoDataToRead:                   // = 70,
            Serial.print("NoDataToRead");
            break;
        // Listening status
        case DNETcK::NeedToCallStartListening:       // = 80,
            Serial.print("NeedToCallStartListening");
            break;
        case DNETcK::NeedToResumeListening:          // = 81,
            Serial.print("NeedToResumeListening");
            break;
        case DNETcK::AlreadyStarted:                 // = 82,
            Serial.print("AlreadyStarted");
            break;
        case DNETcK::AlreadyListening:               // = 83,
            Serial.print("AlreadyListening");
            break;
        case DNETcK::Listening:                      // = 84,
            Serial.print("Listening");
            break;
        case DNETcK::ExceededMaxPendingAllowed:      // = 85,
            Serial.print("ExceededMaxPendingAllowed");
            break;
        case DNETcK::MoreCurrentlyPendingThanAllowed: // = 86,
            Serial.print("MoreCurrentlyPendingThanAllowed");
            break;
        case DNETcK::ClientPointerIsNULL:            // = 87,
            Serial.print("ClientPointerIsNULL");
            break;
        case DNETcK::SocketAlreadyAssignedToClient:  // = 88,
            Serial.print("SocketAlreadyAssignedToClient");
            break;
        case DNETcK::NoPendingClients:               // = 89,
            Serial.print("NoPendingClients");
            break;
        case DNETcK::IndexOutOfBounds:               // = 90,
            Serial.print("IndexOutOfBounds");
            break;
        // UDP endpoint resolve state machine
        case DNETcK::EndPointNotSet:                 // = 100,
            Serial.print("EndPointNotSet");
            break;
        // DNSResolving
        case DNETcK::ARPResolving:                   // = 110,
            Serial.print("ARPResolving");
            break;
        case DNETcK::AcquiringSocket:                // = 111,
            Serial.print("AcquiringSocket");
            break;
        case DNETcK::Finalizing:                     // = 112,
            Serial.print("Finalizing");
            break;
        case DNETcK::EndPointResolved:               // = 113,
            Serial.print("EndPointResolved");
            break;
        // DNSResolutionFailed
        case DNETcK::ARPResolutionFailed:            // = 120,
            Serial.print("ARPResolutionFailed");
            break;
        // SocketError
        // WiFi Stuff below here
        case DNETcK::WFStillScanning:                // = 130,
            Serial.print("WFStillScanning");
            break;
        case DNETcK::WFUnableToGetConnectionID:      // = 131,
            Serial.print("WFUnableToGetConnectionID");
            break;
        case DNETcK::WFInvalideConnectionID:         // = 132,
            Serial.print("WFInvalideConnectionID");
            break;
        case DNETcK::WFAssertHit:                    // = 133,
            Serial.print("WFAssertHit");
            break;
        default:
            Serial.print("Status unknown");
            break;
    }
}


