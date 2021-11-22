#pragma once

#ifndef INCLUDE_JET_ANIMATION_H_
#define INCLUDE_JET_ANIMATION_H_

#include "../Utils/macros.h"
#include "../Utils/private_helpers.h"
#include <memory>


struct Frame final
{
	int index;
	// time interval between frames
	double timeIntervalInSeconds = 1.0 / 60.0;

	Frame()
	{
		index = 0;
	}

	Frame(int newIndex, double newTimeIntervalInSeconds) : index(newIndex), timeIntervalInSeconds(newTimeIntervalInSeconds)
	{

	}

	double timeInSeconds() const
	{
		return index * timeIntervalInSeconds;
	}

	void advance()
	{
		++index;
	}

	void advance(unsigned int delta)
	{
		index += delta;
	}

	Frame& operator++()
	{
		advance();
		return *this;
	}

	Frame operator++(int i)
	{
		UNUSED_VARIABLE(i);

		Frame result = *this;
		advance();
		return result;
	}
};

class Animation
{
public:
	Animation();

	virtual ~Animation();

	void update(const Frame& frame);

protected:
	// update the animation state
	virtual void onUpdate(const Frame& frame) = 0;
};

#endif