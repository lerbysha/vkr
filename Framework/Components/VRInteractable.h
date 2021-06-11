#pragma once
#include <UnigineNode.h>
#include <UniginePhysics.h>
#include <UnigineComponentSystem.h>
#include "Players/VRPlayer.h"
#include "../Utils.h"

class VRPlayer;

class VRInteractable : public Unigine::ComponentBase
{
public:
	COMPONENT(VRInteractable, Unigine::ComponentBase);

	// interact methods
	virtual void selectIt(VRPlayer *player, int hand_num) { on_select_it(this, player, hand_num); }
	virtual void inspectIt(VRPlayer *player, int status) { on_inspect_it(this, player, status); }
	virtual void grabIt(VRPlayer *player, int hand_num) { on_grab_it(this, player, hand_num); }
	virtual void holdIt(VRPlayer *player, int hand_num) { on_hold_it(this, player, hand_num); }
	virtual void useIt(VRPlayer *player, int hand_num) { on_use_it(this, player, hand_num); }
	virtual void throwIt(VRPlayer *player, int hand_num) { on_throw_it(this, player, hand_num); }

	bool use_hand_to_hand = true;

	// subscribe methods
	template<typename Functor>
	int setOnSelectItListener(Functor &&f)
	{
		return on_select_it.addListener(std::forward<Functor>(f));
	}

	template<typename Functor>
	int setOnInspectItListener(Functor &&f)
	{
		return on_inspect_it.addListener(std::forward<Functor>(f));
	}

	template<typename Functor>
	int setOnGrabItListener(Functor &&f)
	{
		return on_grab_it.addListener(std::forward<Functor>(f));
	}

	template<typename Functor>
	int setOnHoldItListener(Functor &&f)
	{
		return on_hold_it.addListener(std::forward<Functor>(f));
	}

	template<typename Functor>
	int setOnUseItListener(Functor &&f)
	{
		return on_use_it.addListener(std::forward<Functor>(f));
	}

	template<typename Functor>
	int setOnThrowItListener(Functor &&f)
	{
		return on_throw_it.addListener(std::forward<Functor>(f));
	}

	// unsubscribe methods
	void removeOnSelectItListener(int id)
	{
		on_select_it.removeListener(id);
	}

	void removeOnInspectItListener(int id)
	{
		on_inspect_it.removeListener(id);
	}

	void removeOnGrabItListener(int id)
	{
		on_grab_it.removeListener(id);
	}

	void removeOnHoldItListener(int id)
	{
		on_hold_it.removeListener(id);
	}

	void removeOnUseItListener(int id)
	{
		on_use_it.removeListener(id);
	}

	void removeOnThrowItListener(int id)
	{
		on_throw_it.removeListener(id);
	}

private:
	Action<void(VRInteractable *const obj, VRPlayer *player, int hand_num)> on_select_it;
	Action<void(VRInteractable *const obj, VRPlayer *player, int status)> on_inspect_it;
	Action<void(VRInteractable *const obj, VRPlayer *player, int hand_num)> on_grab_it;
	Action<void(VRInteractable *const obj, VRPlayer *player, int hand_num)> on_hold_it;
	Action<void(VRInteractable *const obj, VRPlayer *player, int hand_num)> on_use_it;
	Action<void(VRInteractable *const obj, VRPlayer *player, int hand_num)> on_throw_it;
};