#pragma once
#include <UnigineComponentSystem.h>
#include "../../Framework/Components/Players/VRPlayer.h"
#include "../../Framework/Components/Objects/ObjMovable.h"
#include "../../Framework/Components/Players/VRPlayerVR.h"
#include "../../Framework/Components/Players/VRPlayerPC.h"
#include "Inventory.h"


class InventoryItem : public Unigine::ComponentBase
{
public:
	COMPONENT(InventoryItem, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_POST_UPDATE(render);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_NAME("inventory_item");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, movable_node);
	PROP_PARAM(Node, inventory_node);
	PROP_PARAM(Switch, alignment_axis, 0, "X,Y,Z,NX,NY,NZ");

	void releasePoint();
	void grabPoint();

protected:
	bool is_hold;
	VRPlayer *vr_player;
	int grab_hand_num;
	ObjMovable *movable_obj;

private:
	void init();
	void render();
	void shutdown();

	void on_grab_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);
	void on_throw_handler(VRInteractable *const obj, VRPlayer *player, int hand_num);

	const float speed_reaction = 7.5f;

	int grab_handler_id;
	int throw_handler_id;

	Inventory *inventory;

	int point_id;
	Vec3 point;
};