#pragma once
#include "animation.h"

using namespace std;

class PhysicsAnimation : public Animation
{
private:
	Frame _currentFrame;
	bool _isUsingFixedSubTimeSteps = true;
	unsigned int _numberOfFixedSubTimeSteps = 1;
	double _currentTime = 0.0;

	void onUpdate(const Frame& frame) final;

	void advanceTimeStep(double timeIntervalInSeconds);

	void initialize();

public:
	PhysicsAnimation();
	virtual ~PhysicsAnimation();

	Frame currentFrame() const;

	void setCurrentFrame(const Frame& frame);


	// True if using fixed sub time steps, false otherwise.
	bool isUsingFixedSubTimeSteps() const;

	void setIsUsingFixedSubTimeSteps(bool isUsing);

	unsigned int numberOfFixedSubTimeSteps() const;

	void setNumberOfFixedSubTimeSteps(unsigned int numberOfSteps);

	void advanceSingleFrame();

	double currentTimeInSeconds() const;

protected:
	virtual void onAdvanceTimeStep(double timeIntervalInSeconds) = 0;

	virtual unsigned int numberOfSubTimeSteps(double timeIntervalInSeconds) const;

	virtual void onInitialize();
};