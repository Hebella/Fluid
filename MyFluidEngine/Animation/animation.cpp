#include "../Utils/pch.h"
#include "animation.h"
#include "../Utils/timer.h"

Animation::Animation()
{

}

Animation::~Animation()
{

}

void Animation::update(const Frame& frame)
{
	Timer timer;

	onUpdate(frame);

}