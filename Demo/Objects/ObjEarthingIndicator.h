#pragma once
#include <UnigineComponentSystem.h>
#include "../../Framework/Components/Objects/ObjMovable.h"
#include "../../Framework/Utils.h"
#include "../../LogicComponents/LogicObject.h"
#include "../../Framework/Components/Players/VRPlayer.h"


class ObjEarthingIndicator : public Unigine::ComponentBase
{
public:
	COMPONENT(ObjEarthingIndicator, Unigine::ComponentBase);
	COMPONENT_INIT(earthing_init);
	COMPONENT_SHUTDOWN(earthing_shutdown);

	PROP_NAME("earthing_indicator");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, broken_earthing_node);

	template<typename Functor>
	int setOnEarthingTouchListener(Functor &&f)
	{
		return on_earthing_touch.addListener(std::forward<Functor>(f));
	}

	void removeOnEarthingTouchListener(int id)
	{
		on_earthing_touch.removeListener(id);
	}

private:
	void earthing_init();
	void earthing_shutdown();
	void check_touch();
	void set_emission(bool value);

	void on_hold_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);
	void on_throw_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	int is_hold;

	int hold_id;
	int throw_id;
	int hold_hand_num = -1;
	VRPlayer *vr_player = nullptr;


	Vector<NodePtr> intersections;
	int node_id;

	LogicObject *broken;
	ObjMovable *obj;

	MaterialPtr material_0;
	MaterialPtr material_1;

	Action<void()> on_earthing_touch;
};