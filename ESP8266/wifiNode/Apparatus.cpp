

#include "Apparatus.hpp"


Apparatus::Apparatus()
{
}

void Apparatus::genRequest(char request[], unsigned int device_id, unsigned int program, bool priority)
{
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(4);
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject &root = jsonBuffer.createObject();

	root["type"] = "startreq";
	root["device_id"] = device_id;
	root["program"] = program;
	root["priority"] = priority;

	root.printTo(request, MESSAGESIZE);
}

int Apparatus::saveStartTime(char reply[])
{
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(4);
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject &root = jsonBuffer.parseObject(reply);

	if (root.success())
	{
		if (strcmp(root["type"], "startok") == 0)
		{
			schedule.startTime= root["startTime"];
			schedule.program= root["program"];
			schedule.scheduled = true;

			return 1;
		}
		else
		{
			return -2;
		}
	}
	else
	{
		return -1;
	}
}

time_t* Apparatus::getStartTime()
{
	return &schedule.startTime;
}

// Send start signal to applicance
void Apparatus::startApparatus(unsigned int program)
{
	dat.useDataSet(program);
	dat.start();
}


// Check if applicance is done
bool Apparatus::finished()
{
	return dat.isFinished();
}

bool Apparatus::isScheduled()
{
	return schedule.scheduled;
}

void Apparatus::clearSchedule()
{
	schedule.scheduled = false;
}

unsigned int Apparatus::getProgram()
{
	return schedule.program;
}

float Apparatus::getSample()
{
	return dat.getNextData();
}