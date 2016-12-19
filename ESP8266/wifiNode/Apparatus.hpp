#ifndef APPARATUS_H
#define APPARATUS_H

#define MAX_SAMPLES 10
#define MESSAGESIZE 255

#include "Network.hpp"
#include "DataReplay.hpp"

#include <ArduinoJson.h>
#include <time.h>


class Apparatus
{
public:
	Apparatus();
	//int measure(time_t theTime);
	time_t *getStartTime();
	void startApparatus(unsigned int program);
	void genRequest(char request[], unsigned int device_id, unsigned int program, bool priority);
	int saveStartTime(char reply[]);
	//int getNextSample();
	//void sampleToJSON(int index, char jsonString[], unsigned int device_id, unsigned int program);
	//void clearSample(int index);
	//int getNrOfSamples();
	bool finished();
	bool isScheduled();
	void clearSchedule();
	unsigned int getProgram();
	float getSample();
private:

	struct Schedule
	{
		time_t startTime = 0;
		bool scheduled = 0;
		unsigned int program = 0;
	} schedule;

	struct DataPoint
	{
		float data = 0;
		time_t timeStamp = 0;
	};
	DataPoint samples[MAX_SAMPLES];
	DataReplay dat;
	
	
};

#endif