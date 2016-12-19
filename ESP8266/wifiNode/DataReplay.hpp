#ifndef DATAREPLAY_H
#define DATAREPLAY_H

#define MAXDATASIZE 5

#include <pgmspace.h>
#include "Data.hpp"

class DataReplay
{
public:

	DataReplay();
	void useDataSet(int set);
	float getNextData();
	bool start();
	bool isFinished();
private:
	int currentDataSet;
	unsigned long dataCounter = 0;
	bool finished = false;
};

#endif
