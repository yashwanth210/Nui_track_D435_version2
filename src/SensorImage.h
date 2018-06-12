#pragma once

#include <nuitrack/Nuitrack.h>
#include <Windows.h>

class SensorImage
{
public:
	SensorImage(std::mutex* coutMutex);
	~SensorImage();
private:
	std::mutex* coutMutex;
	tdv::nuitrack::ColorSensor::Ptr _colorSensor;

	void onNewRGBFrame(tdv::nuitrack::RGBFrame::Ptr frame);
};

