#include "pch.h"
#include "animation.h"
#include "timer.h"
#include "logging.h"

Animation::Animation()
{

}

Animation::~Animation()
{

}

void Animation::update(const Frame& frame)
{
	Timer timer;

	/*JET_INFO << "Begin updating frame: " << frame.index
		<< " timeIntervalInSeconds: " << frame.timeIntervalInSeconds
		<< " (1/" << 1.0 / frame.timeIntervalInSeconds
		<< ") seconds";*/

	onUpdate(frame);

	/*JET_INFO << "End updating frame (took " << timer.durationInSeconds()
		<< " seconds)";*/
}