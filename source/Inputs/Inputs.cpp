#include "Inputs.h"

#include "Helpers/HLog.h"

const std::unordered_map<harmony::Inputs::Action, harmony::Inputs::Key> Inputs::sInputBindings = {
	{ harmony::Inputs::Action::eINVALID_ACTION, harmony::Inputs::Key::eINVALID_KEY },
	{ harmony::Inputs::Action::eMOVE_RIGHT, harmony::Inputs::Key::eKEY_ARROW_RIGHT },
	{ harmony::Inputs::Action::eMOVE_LEFT, harmony::Inputs::Key::eKEY_ARROW_LEFT },
	{ harmony::Inputs::Action::eMOVE_BACKWARD, harmony::Inputs::Key::eKEY_ARROW_DOWN },
	{ harmony::Inputs::Action::eMOVE_FORWARD, harmony::Inputs::Key::eKEY_ARROW_UP },

	{ harmony::Inputs::Action::eMOVE_DOWN, harmony::Inputs::Key::eKEY_RIGHT_CONTROL },
	{ harmony::Inputs::Action::eMOVE_UP, harmony::Inputs::Key::eKEY_RIGHT_SHIFT },

	{ harmony::Inputs::Action::eYAW_RIGHT, harmony::Inputs::Key::eMOUSE_RIGHT },
	{ harmony::Inputs::Action::eYAW_LEFT, harmony::Inputs::Key::eMOUSE_LEFT },
	{ harmony::Inputs::Action::ePITCH_DOWN, harmony::Inputs::Key::eMOUSE_DOWN },
	{ harmony::Inputs::Action::ePITCH_UP, harmony::Inputs::Key::eMOUSE_UP },
};


Inputs::Inputs(const harmony::Inputs::Description& description)
	: mPreviousKeyState{false}, mCurrentKeyState{false}, 
	mPreviousMousePosition{0.0}, mCurrentMousePosition{0.0}, mCurrentMouseDelta{0.0}
{
	HLOG_INFO("Created.");
}

Inputs::~Inputs()
{
	HLOG_INFO("Destroyed.");
}

const harmony::Inputs::Description& Inputs::getDescription() const
{
	return mDescription;
}

void Inputs::initialize()
{
	reset();
	HLOG_DEBUG("Initialized.");
}

void Inputs::update()
{
	if (mCurrentMousePosition[0] > mPreviousMousePosition[0])
	{
		mCurrentKeyState[Key::eMOUSE_RIGHT] = true;
	}
	else
	{
		mCurrentKeyState[Key::eMOUSE_RIGHT] = false;
	}

	if (mCurrentMousePosition[0] < mPreviousMousePosition[0])
	{
		mCurrentKeyState[Key::eMOUSE_LEFT] = true;
	}
	else
	{
		mCurrentKeyState[Key::eMOUSE_LEFT] = false;
	}

	if (mCurrentMousePosition[0] < mPreviousMousePosition[0])
	{
		mCurrentKeyState[Key::eMOUSE_UP] = true;
	}
	else
	{
		mCurrentKeyState[Key::eMOUSE_UP] = false;
	}

	if (mCurrentMousePosition[0] > mPreviousMousePosition[0])
	{
		mCurrentKeyState[Key::eMOUSE_DOWN] = true;
	}
	else
	{
		mCurrentKeyState[Key::eMOUSE_DOWN] = false;
	}

	mPreviousMousePosition[0] = mCurrentMousePosition[0];
	mPreviousMousePosition[1] = mCurrentMousePosition[1];

	for (unsigned int i = 0; i < eMAX_KEYS; ++i)
	{
		mPreviousKeyState[i] = mCurrentKeyState[i];
	}
}

void Inputs::reset()
{
	for (unsigned int i = 0; i < eMAX_KEYS; ++i)
	{
		mPreviousKeyState[i] = false;
		mCurrentKeyState[i] = false;
	}

	mPreviousMousePosition[0] = 0.0f;
	mPreviousMousePosition[1] = 0.0f;

	mCurrentMousePosition[0] = 0.0f;
	mCurrentMousePosition[1] = 0.0f;

	//HLOG_DEBUG("Reset.");
}

void Inputs::deinitialize()
{
	reset();
	HLOG_DEBUG("Deinitialized.");
}

void Inputs::keyCallback(const Key key, const Event e)
{
	if (e == Event::ePRESSED)
		mCurrentKeyState[key] = true;

	if (e == Event::eRELEASED)
		mCurrentKeyState[key] = false;
}

void Inputs::mouseCallback(const float x, const float y, const float dx, const float dy)
{
	mCurrentMousePosition[0] = x;
	mCurrentMousePosition[1] = y;

	mCurrentMouseDelta[0] = dx;
	mCurrentMouseDelta[1] = dy;
}

bool Inputs::actionOccured(const Action action, const Event e) const
{
	switch (e)
	{
	default:
		HSOFT_ASSERT(false, "Invalid event polled for action %d", action);
		break;

	case eJUST_PRESSED:
		return (mCurrentKeyState[sInputBindings.at(action)] && !mPreviousKeyState[sInputBindings.at(action)]);

	case ePRESSED:
		return (mCurrentKeyState[sInputBindings.at(action)] && mPreviousKeyState[sInputBindings.at(action)]);

	case eJUST_RELEASED:
		return (!mCurrentKeyState[sInputBindings.at(action)] && mPreviousKeyState[sInputBindings.at(action)]);

	case eRELEASED:
		return (!mCurrentKeyState[sInputBindings.at(action)] && !mPreviousKeyState[sInputBindings.at(action)]);
	}
	return false;
}
