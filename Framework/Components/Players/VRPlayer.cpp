#include "VRPlayer.h"

#include <UnigineGame.h>
#include <UnigineEditor.h>
#include <UnigineEngine.h>
#include <UnigineApp.h>
#include <UnigineInput.h>

#include "../Objects/ObjSwitch.h"
#include "../Objects/ObjHandle.h"

using namespace Unigine;
using namespace Math;

VRPlayer* VRPlayer::instance;

int VRPlayer::isVRPluginLoaded()
{
	return (Engine::get()->findPlugin("AppVive") != -1 || Engine::get()->findPlugin("AppOculus") != -1 || Engine::get()->findPlugin("AppVarjo") != -1);
}

VRPlayer* VRPlayer::get()
{
	return instance;
}

void VRPlayer::init()
{
	// callbacks
	use_callback = nullptr;
	grab_callback = nullptr;
	hold_callback = nullptr;
	throw_callback = nullptr;

	// run application even when unfocused
	App::setBackgroundUpdate(true);

	instance = this;
}

void VRPlayer::update()
{
	for (int i = 0; i < getNumHands(); i++)
	{
		auto &components = getGrabComponents(i);

		// use holded object
		if (getHandState(i) == HAND_HOLD && getUseButtonDown(i))
		{
			for (int j = 0; j < components.size(); j++)
				if (components[j]->isEnabled())
					components[j]->useIt(this, i);
			if (use_callback) use_callback->run(getGrabNode(i));
		}
	}
}

void VRPlayer::post_update()
{
	for (int i = 0; i < getNumHands(); i++)
	{
		auto &selected_components = getSelectedComponents(i);
		for (int j = 0; j < selected_components.size(); j++)
			if (selected_components[j]->isEnabled())
			{
				selected_components[j]->selectIt(this, i);

				if (!dynamic_cast<ObjSwitch *>(selected_components[j]))
					continue;

				if (getUseButtonDown(i) || Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
					selected_components[j]->useIt(this, i);
			}

		int hand_state = getHandState(i);
		if (hand_state != HAND_FREE)
		{
			auto &components = getGrabComponents(i);

			// use holded object
			if (getHandState(i) == HAND_HOLD && (Input::isKeyDown(Input::KEY_F) || getUseButtonDown(i)))
			{
				for (int j = 0; j < components.size(); j++)
					if (components[j]->isEnabled())
						components[j]->useIt(this, i);
				if (use_callback) use_callback->run(getGrabNode(i));
			}

			switch (hand_state)
			{
			case HAND_GRAB:
				for (int j = 0; j < components.size(); j++)
					if (components[j]->isEnabled())
						components[j]->grabIt(this, i);
				if (grab_callback) grab_callback->run(getGrabNode(i));
				break;
			case HAND_HOLD:
				for (int j = 0; j < components.size(); j++)
					if (components[j]->isEnabled())
						components[j]->holdIt(this, i);
				if (hold_callback) hold_callback->run(getGrabNode(i));
				break;
			case HAND_THROW:
				for (int j = 0; j < components.size(); j++)
					if (components[j]->isEnabled())
						components[j]->throwIt(this, i);
				if (throw_callback) throw_callback->run(getGrabNode(i));
				break;
			default:
				break;
			}
		}
	}
	update_button_states();
}

void VRPlayer::shutdown()
{
	// callbacks
	delete use_callback;
	delete grab_callback;
	delete hold_callback;
	delete throw_callback;

	instance = nullptr;
}

void VRPlayer::setPlayerPosition(const Vec3 &pos)
{
	getPlayer()->setPosition(pos);
}

void VRPlayer::landPlayerTo(const Vec3 &position, const vec3 &direction)
{
	Vec3 pos1 = position;
	Vec3 pos2 = position + Vec3(0, 0, -1) * getPlayer()->getZFar();
	WorldIntersectionPtr intersection = WorldIntersection::create();
	ObjectPtr hitObj = World::getIntersection(pos1, pos2, 1, intersection);
	if (hitObj)
		getPlayer()->setWorldTransform(setTo(intersection->getPoint(), intersection->getPoint() + Vec3(direction), vec3::UP));
}

int VRPlayer::can_i_grab_it(const NodePtr &grab_node)
{
	return ComponentSystem::get()->getComponent<VRInteractable>(grab_node) != nullptr;
}

int VRPlayer::is_it_switcher(const NodePtr &grab_node)
{
	return ComponentSystem::get()->getComponent<ObjSwitch>(grab_node) != nullptr;
}

int VRPlayer::is_it_handle(const NodePtr &grab_node)
{
	return ComponentSystem::get()->getComponent<ObjHandle>(grab_node) != nullptr;
}