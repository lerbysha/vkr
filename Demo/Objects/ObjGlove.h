#pragma once
#include <UnigineComponentSystem.h>
#include "../../Framework/Components/Players/VRPlayerVR.h"
#include <UnigineSounds.h>


class ObjGlove : public Unigine::ComponentBase
{
public:
	COMPONENT(ObjGlove, Unigine::ComponentBase);
	COMPONENT_INIT(init, 1);
	COMPONENT_POST_UPDATE(render, 2);

	PROP_NAME("glove");
	PROP_AUTOSAVE(1);

	PROP_PARAM(Switch, glove_side, 0, "left,right");
	PROP_PARAM(Node, player_spawn_node);
	PROP_PARAM(File, default_animation);
	PROP_PARAM(Node, checklist);
	PROP_PARAM(Node, tablet_1);
	PROP_PARAM(Node, tablet_2);
	PROP_PARAM(File, checklist_animation);
	PROP_PARAM(Node, indicator);
	PROP_PARAM(File, indicator_animation);
	PROP_PARAM(Node, stopwatch);
	PROP_PARAM(File, stopwatch_animation);
	PROP_PARAM(Node, cabel);
	PROP_PARAM(File, cabel_animation);
	PROP_PARAM(File, sound_file);
	PROP_PARAM(Float, volume);

	struct BoneShape : public ComponentStruct
	{
		PROP_PARAM(Node, bone)
		PROP_PARAM(Node, shape)
	};

	PROP_ARRAY_STRUCT(BoneShape, bones_shapes)

private:
	void init();
	void render();

	enum LAYERS
	{
		DEFAULT = 0,
		CHECKLIST,
		INDICATOR,
		STOPWATCH,
		CABEL,
		LAYERS_NUM
	};

	void set_layer(LAYERS layer);

	VRPlayerVR *vr_player = nullptr;
	ObjectMeshSkinnedPtr mesh_skinned;
	LAYERS current_layer = LAYERS::DEFAULT;

	Mat4 transform;
	NodePtr hand;

	SoundSourcePtr sound_source;
	float last_axis;
	void init_sound(const char *file_name, int is_loop = 0);
	void stop_sound();

	const float min_axis_value = 0.001f;
	const float sound_axis_delta = 0.02f;
};