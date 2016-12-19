#ifndef NETWORK_H
#define NETWORK_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define DEVICE_PORT 55056
#define MESSAGESIZE 255

class Network
{
public:
	Network();
	void begin(const char bssid[], const char pass[]);
	void udpSend(const char destination[], unsigned int port, char request[], char reply[], unsigned int replyTimeout);
	void tcpSend(const char destination[], unsigned int port, char request[], char reply[]);
	void calcBroadcast(char broadcastAddr[]);
};

#endif