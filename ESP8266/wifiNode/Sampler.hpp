#ifndef SAMPLER_H
#define SAMPLER_H

#include <ArduinoJson.h>
#include <time.h>

#define MAX_SAMPLES 10
#define MESSAGESIZE 255

class Sampler
{
public:
	Sampler();
	int measure(time_t theTime, float data);
	int getNextSample();
	void sampleToJSON(int index, char jsonString[], unsigned int device_id, unsigned int program);
	void clearSample(int index);
	int getNrOfSamples();
private:
	struct DataPoint
	{
		float data = 0;
		time_t timeStamp = 0;
	};
	DataPoint samples[MAX_SAMPLES];
	int getFreeIndex();
};

#endif