#include "Network.hpp"



Network::Network()
{

}

/*
 *	Initialize the network conneciton. 
 *	Takes the network name and password as parameters.
 *	The method will wait forever until the connection is up.
*/

void Network::begin(const char bssid[], const char pass[])
{
	WiFi.begin(bssid, pass);

	Serial.print("Connecting");

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
}

void Network::udpSend(const char destination[], unsigned int port, char request[MESSAGESIZE], char reply[MESSAGESIZE], unsigned int replyTimeout)
{
	WiFiUDP Udp;
	
	while( !Udp.begin(DEVICE_PORT) )
	{
		Serial.println("Failed to start UDP socket... Retrying.");
	}

	Serial.println("UDP socket opened.");

    Udp.beginPacket(destination, port);
    Udp.write(request);
    Udp.endPacket();

    unsigned long time = millis();

    while(millis() - time < replyTimeout) {
	    int packetSize = Udp.parsePacket();
	    if (packetSize)
	    {
	    	int length = Udp.read(reply, MESSAGESIZE);
	    	if (length > 0)
	    	{
	    		reply[length] = 0;
	    		break;
	    	}
	    }
	}
	Udp.stop();
}

void Network::tcpSend(const char destination[], unsigned int port, char request[], char reply[])
{
	WiFiClient client;

	if (client.connect(destination, port))
	{
		Serial.printf("Connected to: %s:%d\n", destination, port);

		client.printf("%s\n", request);

		while(client.connected())
		{
			if (client.available())
			{
				int length = client.available();

				Serial.printf("TCP message length: %d\n", length);
				
				client.read((uint8_t*)reply, length);
				reply[length] = 0;
			}
		}
		client.stop();
		Serial.printf("Recieved: %s\n", reply);
	}
	else
	{
		Serial.printf("Failed to connect to %s:%u\n", destination, port);
	}
}

void Network::calcBroadcast(char broadcastAddr[])
{
	IPAddress ip = WiFi.localIP();
	IPAddress netmask = WiFi.subnetMask();
	IPAddress broadcast;

	for (int i = 0; i < 4; ++i)
	{
		broadcast[i] = (ip[i] | ~netmask[i] );
	}

	strcpy(broadcastAddr, broadcast.toString().c_str());
}