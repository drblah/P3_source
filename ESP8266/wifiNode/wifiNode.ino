#define DEVICE_PROFILE 123
#define COMS_PORT 55056
#define COMS_TIMEOUT 2000
#define MESSAGESIZE 255
#define IPADDRSIZE 13

// Apparatus properties
#define PROGRAM_0 0
#define PROGRAM_1 1
#define PROGRAM_2 2
#define PRIORITY false

#include <time.h>

#include "Network.hpp"
#include "Registration.hpp"
#include "Apparatus.hpp"
#include "Sampler.hpp"

Network net;
Registration reg;
Apparatus app;
Sampler sampler;


float counter = 0;

bool requestRegistration()
{
  char request[MESSAGESIZE];
  char reply[MESSAGESIZE];
  char broadcastAddr[IPADDRSIZE];

  Serial.println("Beginning registration.");

  reg.genRequest(request, MESSAGESIZE, DEVICE_PROFILE, WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());

  net.calcBroadcast(broadcastAddr);

  Serial.printf("Calculated broadcast address: %s\n", broadcastAddr);

  net.udpSend(broadcastAddr, COMS_PORT, request, reply, COMS_TIMEOUT);

  Serial.printf("Reply packet contents: %s\n", reply);

  int result = reg.saveRegistration(reply);

  switch(result) 
  {
    case 1:
      Serial.println("Registration successful:");
      Serial.printf("\tDEVICE_ID: %d\n", reg.getDeviceId());
      Serial.printf("\tSERVER_IP: %s\n", reg.getServerIp());
      Serial.printf("\tSERVER_PORT: %d\n", reg.getServerPort());
      return true;
    case -1:
      Serial.println("Registration packet malformed and un-parseable");
      return false;
    case -2:
      Serial.println("Recieved wrong reply type. Cannot register.");
      return false;
    default:
      Serial.println("Unhandled registration error. We should not be here.");
      return false;
  }
}

bool requestStart()
{
  char request[MESSAGESIZE];
  char reply[MESSAGESIZE];

  Serial.println("Beginning start request.");

  app.genRequest(request, reg.getDeviceId(), PROGRAM_1, PRIORITY);

  net.tcpSend(reg.getServerIp(), reg.getServerPort(), request, reply);

  Serial.println(reply);

  int result = app.saveStartTime(reply);

  switch(result) 
  {
    case 1:
      Serial.println("Start request successful:");
      Serial.printf("\tstart time: %s\n", ctime(app.getStartTime() ) );
      return true;
    case -1:
      Serial.println("Start reply packet malformed and un-parseable");
      return false;
    case -2:
      Serial.println("Recieved wrong reply type. Cannot request start.");
      return false;
    default:
      Serial.println("Unhandled start request error. We should not be here.");
      return false;
  }
}

/*
*
* Emulates a measurement.
*
*/

void doMeasurement()
{
  //Serial.printf("\tSaving sample. DATA: %f - TIMESTAMP: %lu\n", samples[i].data, samples[i].timeStamp);
  int mResult = sampler.measure(time(nullptr), app.getSample());

  switch(mResult)
  {
    case 1:
      Serial.printf("Measurement successful.\n");
      break;
    case -1:
      Serial.printf("Failed Measurement. Invalid timestamp.\n" );
      break;
    case -2:
      Serial.printf("Failed Measurement. Not enough sample space.\n" );
      break;
  }
}

void sendMeasurements()
{
  int sampleCount = sampler.getNrOfSamples();

  if (sampleCount > 0)
  {
    for (int i = 0; i < sampleCount; ++i)
    {
      char message[MESSAGESIZE];
      char reply[MESSAGESIZE];
      const int BUFFER_SIZE = JSON_OBJECT_SIZE(1);
      StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

      int sampleNr = sampler.getNextSample();
      sampler.sampleToJSON(sampleNr, message, reg.getDeviceId(), app.getProgram());
      net.tcpSend(reg.getServerIp(), reg.getServerPort(), message, reply);

      JsonObject &root = jsonBuffer.parseObject(reply);

      if (root.success())
      {
        Serial.println("Sample request parsable.");

        if (strcmp(root["type"], "sampleOK") == 0)
        {
          Serial.println("Sample request recieved by server.\nRemoving sample from local memory");
          sampler.clearSample(sampleNr);
        }
      }

    }
  }
}

void setup() {
  Serial.begin(115200);
  net.begin("Wifizor", "Vi kan lide bananer");
  
  while( !requestRegistration() )
  {
  	delay(1000);
  }

  requestStart();

  configTime(2, 0, "pool.ntp.org", "pool.ntp.org", "pool.ntp.org");
  // First time always returns 0. So it is run once to initialize it for first use.
  time(nullptr);
  delay(1000);

  /* Mass measurements.
  for (int i = 0; i < 5; ++i)
  {
    doMeasurement();
  }
  */
}


void loop() {
  Serial.println("Looping...");

  time_t theTime = time(nullptr);
  Serial.println(ctime(&theTime));
  delay(2000);


  if (*app.getStartTime() < time(nullptr) && app.isScheduled())
  {
    Serial.println("\tIt is time to start.");

    app.startApparatus(app.getProgram());

    while( !app.finished() )
    {
      doMeasurement();
      sendMeasurements();
      delay(1000);
    }

    app.clearSchedule();
    
  }
  else
  {
    sendMeasurements();

    Serial.println("\tNot time to start yet.");
  }

}
