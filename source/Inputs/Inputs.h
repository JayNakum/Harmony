#pragma once

#include "Inputs/HInputs.h"

#include <unordered_map>

class Inputs : public harmony::Inputs
{
private:
	static const std::unordered_map<harmony::Inputs::Action, harmony::Inputs::Key> sInputBindings;

public:
	Inputs(const harmony::Inputs::Description& description);
	~Inputs();

	const harmony::Inputs::Description& getDescription() const;

	void initialize();
	void update();
	void reset();
	void deinitialize();

	void keyCallback(const Key key, const Event e);
	void mouseCallback(const float x, const float y, const float dx = 0.0f, const float dy = 0.0f);

	bool actionOccured(const Action action, const Event e) const;

private:
	bool mPreviousKeyState[Key::eMAX_KEYS];
	bool mCurrentKeyState[Key::eMAX_KEYS];

	float mPreviousMousePosition[2];
	float mCurrentMousePosition[2];
	float mCurrentMouseDelta[2];

	const harmony::Inputs::Description mDescription;
};