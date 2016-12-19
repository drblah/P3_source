#include "Sampler.hpp"

Sampler::Sampler()
{
	
}

int Sampler::measure(time_t theTime, float data)
{
	int i = this->getFreeIndex();

	if (theTime != 0 && i != -1)
	{
		samples[i].data = data;
		samples[i].timeStamp = theTime;

		return 1;
	}
	else if (theTime == 0)
	{
		return -1;
	}
	else
	{
		return -2;
	}
}

void Sampler::sampleToJSON(int index, char jsonString[], unsigned int device_id, unsigned int program)
{
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(5);
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject &root = jsonBuffer.createObject();

	root["type"] = "sample";
	root["device_id"] = device_id;
	root["program"] = program;
	root["Data"] = samples[index].data;
	root["timeStamp"] = samples[index].timeStamp;

	root.printTo(jsonString, MESSAGESIZE);
}

void Sampler::clearSample(int index)
{
	samples[index].data = 0;
	samples[index].timeStamp = 0;
}

// Returns the index of the first sample containing data.
// Returns -1 if no data was found.
int Sampler::getNextSample()
{
	for (int i = 0; i < MAX_SAMPLES; ++i)
	{
		if (samples[i].timeStamp != 0)
		{
			return i;
		}
	}
	return -1;
}

int Sampler::getNrOfSamples()
{
	int sampleCount = 0;
	for (int i = 0; i < MAX_SAMPLES; ++i)
	{
		if (samples[i].timeStamp != 0)
		{
			sampleCount = sampleCount + 1;
		}
	}

	return sampleCount;
}

// Returns the index of the first free index in the sample space
// If no free space is found, returns -1
int Sampler::getFreeIndex()
{
	for (int i = 0; i < MAX_SAMPLES; ++i)
	{
		if (samples[i].timeStamp == 0)
		{
			return i;
		}
	}
	return -1;
}