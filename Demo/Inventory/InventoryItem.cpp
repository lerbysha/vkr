#include "InventoryItem.h"
#include "../../Framework/Components/VRPlayerSpawner.h"


REGISTER_COMPONENT(InventoryItem);


void InventoryItem::on_grab_handler(VRInteractable *const obj, VRPlayer* player, int hand_num)
{
	if (is_hold)
		return;

	inventory->releasePoint(point_id);
	point_id = -1;

	is_hold = true;
	grab_hand_num = hand_num;

	if (!VRPlayer::isVRPluginLoaded())
	{
		ObjectPtr selected_object = checked_ptr_cast<Object>(node);
		if (!selected_object)
			return;

		for (int i = 0; i < selected_object->getNumSurfaces(); i++)
			selected_object->setMaterialState("auxiliary", 0, i);
	}
}

void InventoryItem::on_throw_handler(VRInteractable *const obj, VRPlayer* player, int hand_num)
{
	if (!is_hold)
		return;

	if (!VRPlayer::isVRPluginLoaded())
	{
		point_id = inventory->grabPoint(node->getPosition(), point);
		is_hold = false;
		return;
	}

	// don't use point and id to avoid false transition in case of transfer from hand-to-hand 
	Vec3 new_point;
	int new_point_id;

	new_point_id = inventory->grabPoint(node->getPosition(), new_point);

	Vec3 dist = node->getPosition() - new_point;
	if (dist.length() > 0.2)
	{
		inventory->releasePoint(new_point_id);

		VRPlayerVR *vr = dynamic_cast<VRPlayerVR *>(player);
		if (vr)
		{
			is_hold = false;
			ObjectPtr grab_obj = checked_ptr_cast<Object>(obj->getNode());
			vr->grabObject(hand_num, grab_obj, 0);
		}
	}
	else
	{
		inventory->releasePoint(new_point_id);

		point_id = inventory->grabPoint(node->getPosition(), point);
		is_hold = false;
	}
}

void InventoryItem::init()
{
	if (!movable_node)
	{
#if DEBUG
		Log::error("INVENTORY ITEM ERROR: movable node is null, please check %s\n", node->getName());
#endif
		return;
	}

	movable_obj = ComponentSystem::get()->getComponent<ObjMovable>(movable_node.get());
	if (!movable_obj)
	{
#if DEBUG
		Log::error("INVENTORY ITEM ERROR: movable node not contain ObjMovable component, please check %s\n", node->getName());
#endif
		return;
	}

	grab_handler_id = movable_obj->setOnGrabItListener(std::bind(&InventoryItem::on_grab_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	throw_handler_id = movable_obj->setOnThrowItListener(std::bind(&InventoryItem::on_throw_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	inventory = ComponentSystem::get()->getComponent<Inventory>(inventory_node.get());

	NodePtr spawn_point = World::getNodeByName("spawn_point");
	if (!spawn_point)
	{
#if DEBUG
		Log::error("INVENTORY ITEM ERROR: spawn point not found\n");
#endif
		return;
	}

	VRPlayerSpawner *spawner = ComponentSystem::get()->getComponent<VRPlayerSpawner>(spawn_point);
	if (!spawner)
	{
#if DEBUG
		Log::error("INVENTORY ITEM ERROR: spawn point not contain VRPlayerSpawner component\n");
#endif
		return;
	}

	vr_player = spawner->getSpawnedPlayer();

	is_hold = false;
	point_id = inventory->grabPoint(node->getPosition(), point);

	grab_hand_num = 0;
}

void InventoryItem::grabPoint()
{
	if (point_id == -1)
	{
		point_id = inventory->grabPoint(node->getPosition(), point);
		is_hold = false;
	}
}

void InventoryItem::releasePoint()
{
	if (point_id != -1)
	{
		inventory->releasePoint(point_id);
		point_id = -1;
		is_hold = true;
	}
}

void InventoryItem::render()
{
	if (is_hold)
		return;

	float k = saturate(speed_reaction * Game::getIFps());
	node->setPosition(lerp(node->getPosition(), point, k));

	Vec3 direction = node->getWorldPosition() - vr_player->getHead()->getWorldPosition();
	direction.z = 0;
	node->setWorldDirection(lerp(node->getWorldDirection(static_cast<Math::AXIS>(alignment_axis.get())), (vec3)direction.normalize(), k), vec3::UP, static_cast<Math::AXIS>(alignment_axis.get()));
}

void InventoryItem::shutdown()
{
	if (movable_obj)
	{
		movable_obj->removeOnGrabItListener(grab_handler_id);
		movable_obj->removeOnThrowItListener(throw_handler_id);
	}
}