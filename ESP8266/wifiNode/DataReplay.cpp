#include "DataReplay.hpp"

DataReplay::DataReplay()
{
	currentDataSet = 0;
}

void DataReplay::useDataSet(int set)
{
	currentDataSet = set;
}

float DataReplay::getNextData()
{
	float data = pgm_read_float_near(&dataSet1[currentDataSet][dataCounter]);

	if (dataCounter + 1 < MAXDATASIZE)
	{
		dataCounter = dataCounter + 1;
	}
	else
	{
		finished = true;
	}

	return data;
}

bool DataReplay::start()
{
	finished = false;
	dataCounter = 0;
	return finished;
}

bool DataReplay::isFinished()
{
	return finished;
}