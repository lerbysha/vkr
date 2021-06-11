#include "ObjGlove.h"
#include "../../Framework/Components/VRPlayerSpawner.h"
#include "../../Demo/Inventory/ChecklistInvertoryItem.h"
#include "../../Framework/SoundManager.h"
#include "../../Framework/Components/Objects/ObjSwitch.h"


REGISTER_COMPONENT(ObjGlove);


void ObjGlove::init()
{
	VRPlayerSpawner *spawner = ComponentSystem::get()->getComponent<VRPlayerSpawner>(player_spawn_node.get());
	if (!spawner)
	{
#if DEBUG
		Log::error("GLOVE ERROR: player spawn node not contain VRPlayerSpawner component, please check %s\n", node->getName());
#endif
		return;
	}

	vr_player = dynamic_cast<VRPlayerVR *>(spawner->getSpawnedPlayer());
	if (!vr_player)
	{
		node->setEnabled(0);
		return;
	}

	mesh_skinned = checked_ptr_cast<ObjectMeshSkinned>(node->getChild(0));
	mesh_skinned->setNumLayers(LAYERS_NUM);
	mesh_skinned->setAnimation(LAYERS::DEFAULT, default_animation.get());
	mesh_skinned->setAnimation(LAYERS::CHECKLIST, checklist_animation.get());
	mesh_skinned->setAnimation(LAYERS::INDICATOR, indicator_animation.get());
	mesh_skinned->setAnimation(LAYERS::STOPWATCH, stopwatch_animation.get());
	mesh_skinned->setAnimation(LAYERS::CABEL, cabel_animation.get());

	current_layer = LAYERS::DEFAULT;
	set_layer(current_layer);

	if (!VRPlayer::isVRPluginLoaded())
		node->setEnabled(0);
	else
	{
		hand = vr_player->getHandNode(glove_side.get());
		transform = vr_player->getHandyIWorldTransform(glove_side.get()) * node->getWorldTransform();

		setTransformPosition(transform, Vec3::ZERO);
		setTransformRotation(transform, quat(0, 0, 0));

		hand->setEnabled(0);
	}

	sound_source = SoundSourcePtr();
	init_sound(sound_file.get(), 0);
	last_axis = 0;
}

void ObjGlove::render()
{
	if (!vr_player)
		return;

	node->setWorldTransform(vr_player->getHandyWorldTransform(glove_side.get()) * transform);

	int size = bones_shapes.size();
	for (int i = 0; i < size; i++)
	{
		NodePtr bone = bones_shapes[i]->bone.get();
		NodePtr shape = bones_shapes[i]->shape.get();

		if (bone && shape)
			shape->setTransform(bone->getTransform());
	}

	int hand_state = vr_player->getHandState(glove_side.get());
	NodePtr grabbed_node = vr_player->getGrabNode(glove_side.get());

	if (hand_state != VRPlayerVR::HAND_FREE)
	{
		LAYERS target_layer = LAYERS::DEFAULT;

		if ((grabbed_node == checklist.get() || grabbed_node == tablet_1.get() || grabbed_node == tablet_2.get()))
			target_layer = LAYERS::CHECKLIST;
		else if (grabbed_node == indicator.get())
			target_layer = LAYERS::INDICATOR;
		else if (grabbed_node == stopwatch.get())
			target_layer = LAYERS::STOPWATCH;
		else if (grabbed_node == cabel.get())
			target_layer = LAYERS::CABEL;

		if (target_layer != current_layer)
		{
			current_layer = target_layer;
			set_layer(current_layer);
		}

		mesh_skinned->setFrame(current_layer, 27.5f);
	}
	else
	{
		if (current_layer != DEFAULT)
		{
			current_layer = LAYERS::DEFAULT;
			set_layer(current_layer);
		}

		if (!grabbed_node && (!vr_player->getUseButtonState(glove_side.get()) || !vr_player->getSelectedComponents(glove_side.get()).size()))
		{
			float axis = vr_player->getControllerAxis(glove_side.get(), vr_player->grab_button);
			mesh_skinned->setFrame(LAYERS::DEFAULT, 27.5f * saturate(axis));

			if (sound_source)
			{
				if (Math::abs(axis - last_axis) > sound_axis_delta)
				{
					sound_source->setWorldPosition(node->getWorldPosition());
					sound_source->setPitch(Game::getRandomFloat(0.8f, 1.0f));
					sound_source->play();
					last_axis = axis;
				}
				else
					sound_source->stop();
			}
		}
	}
}

void ObjGlove::init_sound(const char *file_name, int is_loop)
{
	if (FileSystem::isFileExist(file_name))
	{
		sound_source = SoundSource::create(file_name);
		sound_source->setLoop(is_loop);
		sound_source->setOcclusion(0);
		sound_source->setMinDistance(2.0f);
		sound_source->setMaxDistance(15.0f);
		sound_source->setParent(node);

		sound_source->setGain(volume.get());
		sound_source->setPlayOnEnable(0);
	}
}

void ObjGlove::stop_sound()
{
	if (sound_source)
	{
		sound_source->stop();
		sound_source->setTime(0);
	}
}

void ObjGlove::set_layer(LAYERS layer)
{
	mesh_skinned->setLayerEnabled(LAYERS::DEFAULT, false);
	mesh_skinned->setLayerEnabled(LAYERS::CHECKLIST, false);
	mesh_skinned->setLayerEnabled(LAYERS::INDICATOR, false);
	mesh_skinned->setLayerEnabled(LAYERS::STOPWATCH, false);
	mesh_skinned->setLayerEnabled(LAYERS::CABEL, false);

	mesh_skinned->setLayer(layer, true, 1.0f);
}