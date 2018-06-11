#include <nuitrack/Nuitrack.h>

#include <iomanip>
#include <iostream>

using namespace tdv::nuitrack;

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

// Callback for the hand data update event
void onSkeletonUpdate(SkeletonTracker::Ptr SkeletonData)
{
	if (!SkeletonData)
	{
		return;
	}

	auto skeletonsData = SkeletonData->getSkeletons();

	if (skeletonsData.use_count == 0)
	{
		return;
	}
	

}

void onHandUpdate(HandTrackerData::Ptr handData)
{
    if (!handData)
    {
        // No hand data
        //std::cout << "No hand data" << std::endl;
        return;
	}
	else
	{
		//std::cout << handData << std::endl;
	}

    auto userHands = handData->getUsersHands();
    if (userHands.empty())
    {
		std::cout << "empty hand data" << std::endl;
        // No user hands
        return;
	}
	else
	{
		//std::cout << userHands[0].userId << std::endl;
	}

	Hand::Ptr rightHand = userHands[0].rightHand;
    if (!rightHand)
    {
        // No right hand
        //std::cout << "Right hand of the first user is not found" << std::endl;
        return;
    }

    /*std::cout << std::fixed << std::setprecision(3);
    std::cout << "Right hand position: "
                 "x = " << rightHand->xReal << ", "
                 "y = " << rightHand->yReal << ", "
                 "z = " << rightHand->zReal << std::endl;
	std::cout << "Hand click: " << rightHand->click << std::endl;
	std::cout << "Hand pressure: " << rightHand->pressure << std::endl;*/
	printHandDataAsJSON(rightHand);
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
    handTracker->connectOnUpdate(onHandUpdate);
	skeletonTracker->connectOnUpdate(onSkeletonUpdate);
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
