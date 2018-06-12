#include <nuitrack/Nuitrack.h>
#include <Windows.h>
#include <iomanip>
#include <iostream>

#include "SensorImage.h"

using namespace tdv::nuitrack;
int Direction_of_sound = 120;


void showHelpInfo()
{
	//std::cout << "Usage: nuitrack_console_sample [path/to/nuitrack.config]" << std::endl;
}

void printHandDataAsJSON(Hand::Ptr hand) {
	//Should print {"coords": [xReal, yReal, zReal]}
	float xReal = hand->xReal;
	float yReal = hand->yReal;
	float zReal = hand->zReal;
	

	///////////////
	/*
	float x = handRight.X - spineBase.X - 0.15f;
	float y = spineBase.Y - handRight.Y + 0.3f;
	Point curPos = MouseControl.GetCursorPosition();
	float smoothing = 1 - cursorSmoothing;
	int MousePosX = (int)(curPos.X + (x * mouseSensitivity * screenWidth - curPos.X) * smoothing);
	int MousePosY = (int)(curPos.Y + ((y + 0.25f) * mouseSensitivity * screenHeight - curPos.Y) * smoothing);
	*/
	//////////////



	if (xReal != 0.0f) {
		//std::cout << "{\"coords\": [" << xReal << ", " << yReal << ", " << zReal << "]}" << std::endl;
		std::cout << "{\"coords\": [" << hand->x << ", " << hand->y << "]}" << std::endl;
	}
}
/*
// Callback for the hand data update event
void onSkeletonUpdate(SkeletonTracker::Ptr SkeletonData)
{
	return;
	if (!SkeletonData)
	{
		return;
	}

	auto skeletonsData = SkeletonData->getSkeletons();
/*
	if (skeletonsData.use_count == 0)
	{
		return;
	}
	

}
*/


std::mutex coutMutex;

SensorImage sensorImg;

bool needToUpdateSelectedSkeleton = false;
double doaProjected = -1;  // direction of arrival mapped to the camera bounds on a scale of 0.0 to 1.0
int selectedSkeletonId = -1;
Joint selectedSpineJoint;

void controlMouse(Joint hand, Joint spine) {
	Vector3 handPos = hand.real;
	Vector3 spinePos = spine.real;

	float x = (spinePos.x - handPos.x) / 250 + 0.3f;
	float y = (spinePos.y - handPos.y) / 300 + 0.85f;
	POINT curPos;
	BOOL result = GetCursorPos(&curPos);

	//Point curPos = MouseControl.GetCursorPosition();
	int screenWidth = 1920;
	int screenHeight = 1080;
	int MousePosX = (int)(curPos.x + (x  * screenWidth - curPos.x));
	int MousePosY = (int)(curPos.y + (y * screenHeight - curPos.y));

	if (MousePosX>0 && MousePosY<1920 && MousePosY>0 && MousePosY<1080)
		SetCursorPos(MousePosX, MousePosY);
}


/**
* Listens on stdin for the direction of arrival mapped to the camera bounds on a scale of 0.0 to 1.0
* If it recieved -1, it deselects the skeleton
*/
void listenOnStdIn() {
	for (std::string line; std::getline(std::cin, line); ) {
		//selectedSkeletonId = std::stoi(line);
		doaProjected = std::stod(line);
		std::cout << "got input: " << doaProjected << std::endl;
		if (doaProjected == -1) {
			selectedSkeletonId = -1;
		}
		else {
			needToUpdateSelectedSkeleton = true;
		}
	}
}


void printVector3ToJson(Vector3 vector) {
	std::printf("[%f, %f, %f]", vector.x, vector.y, vector.z);
}

void printDataAsJson(Joint torso, Joint leftHand, Joint rightHand) {
	coutMutex.lock();
	std::cout << "{" << "\"type\": \"JointData\"";

	std::cout << "\"torso\": ";
	printVector3ToJson(torso.real);
	std::cout << "\"leftHand\": ";
	printVector3ToJson(leftHand.real);
	std::cout << "\"rightHand\": ";
	printVector3ToJson(rightHand.real);

	std::cout << "}" << std::endl;
	coutMutex.unlock();
}

Skeleton* getSelectedSkeleton(SkeletonData::Ptr skeletonData) {
	// If the wake word was activated, select skeleton closest to the projected doa
	if (needToUpdateSelectedSkeleton)
	{
		std::cout << "selecting new skeleton" << std::endl;
		double smallestOffset = 100;
		for each (Skeleton skeleton in skeletonData->getSkeletons())
		{
			Joint head = skeleton.joints[JOINT_HEAD];
			double headProjX = head.proj.x;
			double offset = abs(headProjX - doaProjected);
			if (offset < smallestOffset) {
				smallestOffset = offset;
				selectedSkeletonId = skeleton.id;
			}
		}

		needToUpdateSelectedSkeleton = false;
		std::cout << "selected skeleton " << selectedSkeletonId << std::endl;
	}

	// Return the selected skeleton
	for each (Skeleton skeleton in skeletonData->getSkeletons())
	{
		if (skeleton.id == selectedSkeletonId) {
			return &skeleton;
		}
	}

	return NULL;
}

void OnSkeletonUpdate(SkeletonData::Ptr skeletonData)
{
	//std::cout << "running" << std::endl;

	Skeleton* skeletonPtr = getSelectedSkeleton(skeletonData);
	if (skeletonPtr == NULL) {
		return;
	}

	auto userskeleton = skeletonData->getSkeletons(); //*skeletonPtr;
	if (skeletonData->getNumSkeletons() == 0) {
		return;
	}



	//std::cout << "skeletons" << userskeleton.size() << std::endl;
	//Skeleton skeleton = userskeleton[0];
	Skeleton skeleton = *skeletonPtr;
	/*Joint torso = skeleton.joints[JOINT_TORSO];
	Joint leftHand = skeleton.joints[JOINT_LEFT_HAND];
	Joint rightHand = skeleton.joints[JOINT_RIGHT_HAND];

	printDataAsJson(torso, leftHand, rightHand);

	*/
	
	Joint spine = skeleton.joints[JOINT_TORSO];
	selectedSpineJoint = spine;
	/*Joint handRight = skeleton.joints[JOINT_RIGHT_HAND];
	Joint handLeft = skeleton.joints[JOINT_LEFT_HAND];

	Vector3 spinePos = spine.real;
	Vector3 handRightPos = handRight.real;
	Vector3 handLeftPos = handLeft.real;

	bool UseRightHand = true;

	std::cout << "Spine Pos : "<<spinePos.x;
	//std::cout << "layer 2";
	if (true || UseRightHand)
	{
		//float x = spinePos.x - handRightPos.x + 0.3f;
		//float y = handRightPos.x - spinePos.x + 0.4f;
		float x = (spinePos.x - handRightPos.x) / 250 + 0.3f;
		float y = (-handRightPos.y + spinePos.y) / 300 + 0.6f;
		POINT curPos;
		BOOL result = GetCursorPos(&curPos);

		//Point curPos = MouseControl.GetCursorPosition();
		float cursorSmoothing = 0.5f;
		float smoothing = 1 - cursorSmoothing;
		float mouseSensitivity = 0.5f;
		int screenWidth = 1920;
		int screenHeight = 1080;
		int MousePosX = (int)(curPos.x + (x * mouseSensitivity * screenWidth - curPos.x) * smoothing);
		int MousePosY = (int)(curPos.y + ((y + 0.25f) * mouseSensitivity * screenHeight - curPos.y) * smoothing);

		if (MousePosX>0 && MousePosY<1920 && MousePosY>0 && MousePosY<1080)
			SetCursorPos(MousePosX, MousePosY);

		std::cout << "X: " << MousePosX << " Y: " << MousePosY << std::endl;
		//coutMutex.lock();
		//std::cout << "{" << "\"type\": \"MouseCoords\"";
		//std::cout << "[" << MousePosX << ", " << MousePosY << "]";
		//std::cout << "}" << std::endl;
		//coutMutex.unlock();*/
	}
}

void onHandUpdate(HandTrackerData::Ptr handData)
{
    if (!handData)
    {
        return;
	}

    auto userHands = handData->getUsersHands();

	for each (UserHands hands in userHands) {
		if (hands.userId == selectedSkeletonId) {
			Hand::Ptr rightHand = userHands[0].rightHand;
			if (!rightHand)
			{
				return;
			}

			controlMouse(rightHand, spine);

			return;
		}
	}

	//printHandDataAsJSON(rightHand);
}

int main(int argc, char* argv[])
{
    showHelpInfo();

    std::string configPath = "";
    if (argc > 1)
        configPath = argv[1];

    // Initialize Nuitrack
    try
    {
        Nuitrack::init(configPath);
    }
    catch (const Exception& e)
    {
        std::cerr << "Can not initialize Nuitrack (ExceptionType: " << e.type() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Create HandTracker module, other required modules will be
    // created automatically
    auto handTracker = HandTracker::create();
	auto skeletonTracker = SkeletonTracker::create();
    // Connect onHandUpdate callback to receive hand tracking data
    //handTracker->connectOnUpdate(onHandUpdate);
	skeletonTracker->connectOnUpdate(OnSkeletonUpdate);
	
	//skeletonTracker->connectOnUpdate(onSkeletonUpdate);
    // Start Nuitrack
    try
    {
        Nuitrack::run();
    }
    catch (const Exception& e)
    {
        std::cerr << "Can not start Nuitrack (ExceptionType: " << e.type() << ")" << std::endl;
        return EXIT_FAILURE;
    }

    int errorCode = EXIT_SUCCESS;
    while (true)
    {
        try
        {
            // Wait for new hand tracking data
            Nuitrack::waitUpdate(handTracker);
        }
        catch (LicenseNotAcquiredException& e)
        {
            std::cerr << "LicenseNotAcquired exception (ExceptionType: " << e.type() << ")" << std::endl;
            errorCode = EXIT_FAILURE;
            break;
        }
        catch (const Exception& e)
        {
            std::cerr << "Nuitrack update failed (ExceptionType: " << e.type() << ")" << std::endl;
            errorCode = EXIT_FAILURE;
        }
    }

    // Release Nuitrack
    try
    {
        Nuitrack::release();
    }
    catch (const Exception& e)
    {
        std::cerr << "Nuitrack release failed (ExceptionType: " << e.type() << ")" << std::endl;
        errorCode = EXIT_FAILURE;
    }

    return errorCode;
}

 
 
	