#include "ObjEarthingIndicator.h"
#ifdef _WIN32
#include <plugins/UnigineAppOculus.h>
#endif // _WIN32
#include <plugins/UnigineAppVive.h>
#include <plugins/UnigineAppVarjo.h>
#include "../../Framework/Components/Players/VRPlayer.h"
#include "../../Framework/Components/VRPlayerSpawner.h"


REGISTER_COMPONENT(ObjEarthingIndicator);

using namespace Unigine::Plugins;


void ObjEarthingIndicator::on_hold_handler(VRInteractable *const interactable_obj, VRPlayer *player, int hand_num)
{
	if (!is_hold)
	{
		set_emission(false);
		is_hold = 1;
		hold_hand_num = hand_num;
		vr_player = player;
	}

	check_touch();
}

void ObjEarthingIndicator::on_throw_handler(VRInteractable *const interactable_obj, VRPlayer *player, int hand_num)
{
	set_emission(false);
	is_hold = 0;
	hand_num = -1;
	vr_player = nullptr;
}

void ObjEarthingIndicator::check_touch()
{
	intersections.clear();
	if (World::getIntersection(node->getWorldBoundBox(), Node::TYPE::OBJECT_MESH_STATIC, intersections))
	{
		for (int i = 0; i < intersections.size(); i++)
			if (!strcmp("udh_earthing", intersections[i]->getName()))
			{
				on_earthing_touch();

				if (broken->getState() == LogicObject::States::Broken)
				{
					set_emission(true);

					if (VRPlayer::isVRPluginLoaded())
						if (vr_player && hold_hand_num != -1)
							vr_player->vibrateController(hold_hand_num);
				}
				else
					set_emission(false);
			}
	}
}

void ObjEarthingIndicator::set_emission(bool value)
{
	if (!material_0 || !material_1)
		return;

	material_0->setState("emission", value ? 1 : 0);
	material_1->setState("emission", value ? 1 : 0);

	if (value)
	{
		material_0->setParameterFloat4("albedo_color", vec4::RED);
		material_1->setParameterFloat4("albedo_color", vec4::RED);
		material_0->setParameterFloat4("emission_color", vec4(1, 0, 0, 0));
		material_1->setParameterFloat4("emission_color", vec4(1, 0, 0, 0));
	}
	else
	{
		material_0->setParameterFloat4("albedo_color", vec4::WHITE);
		material_1->setParameterFloat4("albedo_color", vec4::WHITE);
		material_0->setParameterFloat4("emission_color", vec4(0, 0, 0, 0));
		material_1->setParameterFloat4("emission_color", vec4(0, 0, 0, 0));
	}
}

void ObjEarthingIndicator::earthing_init()
{
	hold_id = -1;
	throw_id = -1;

	if (!broken_earthing_node)
	{
#if DEBUG
		Log::error("Obj Earthing Endicator error: broken earthing node is null, please check %s\n", node->getName());
#endif
		return;
	}

	broken = getComponent<LogicObject>(broken_earthing_node.get());
	if (!broken)
	{
#if DEBUG
		Log::error("Obj Earthing Endicator error: broken earthing node not contain LogicObject component, please check %s\n", node->getName());
#endif
		return;
	}

	obj = getComponent<ObjMovable>(node);
	if (!obj)
	{
#if DEBUG
		Log::error("Obj Earthing Endicator error: node not contain ObjMovable component, please check %s\n", node->getName());
#endif
		return;
	}

	hold_id = obj->setOnHoldItListener(std::bind(&ObjEarthingIndicator::on_hold_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	throw_id = obj->setOnThrowItListener(std::bind(&ObjEarthingIndicator::on_throw_handler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	ObjectMeshStaticPtr mesh = checked_ptr_cast<ObjectMeshStatic>(node);

	material_0 = mesh->getMaterialInherit(0);
	material_1 = mesh->getMaterialInherit(1);

	material_0->setParameterFloat4("emission_color", vec4(1, 0, 0, 0));
	material_1->setParameterFloat4("emission_color", vec4(1, 0, 0, 0));

	is_hold = 0;
}

void ObjEarthingIndicator::earthing_shutdown()
{
	if (obj && hold_id != -1)
		obj->removeOnHoldItListener(hold_id);

	if (obj && throw_id != -1)
		obj->removeOnThrowItListener(throw_id);
}