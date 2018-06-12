#include "SensorImage.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace tdv::nuitrack;

SensorImage::SensorImage(std::mutex* coutMutex)
{
	this->coutMutex = coutMutex;

	_colorSensor = ColorSensor::create();
	// Bind to event new frame
	_colorSensor->connectOnNewFrame(std::bind(&SensorImage::onNewRGBFrame, this, std::placeholders::_1));
}


SensorImage::~SensorImage()
{
}



// Copy color frame data, received from Nuitrack, to texture to visualize
void SensorImage::onNewRGBFrame(RGBFrame::Ptr frame)
{
	coutMutex->lock();
	std::cout << "{" << "\"type\": \"ImageData\"";

	std::cout << "\"width\": " << frame->getCols();
	std::cout << "\"height\": " << frame->getRows();
	std::cout << "\"raw\": " << frame->getData();

	std::cout << "}" << std::endl;
	coutMutex->unlock();
}