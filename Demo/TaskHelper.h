#pragma once
#include "../LogicComponents/Observers/Observer.h"
#include "Tooltip.h"


class TaskHelper : public Observer
{
public:
	COMPONENT(TaskHelper, Observer);

	COMPONENT_UPDATE(update);

	PROP_NAME("task_helper");
	PROP_AUTOSAVE(0);

	struct ObjectAndSurface : public ComponentStruct
	{
		virtual ~ObjectAndSurface() = default;

		PROP_PARAM(Node, object);
		PROP_PARAM(Int, surface_number);
	};

	PROP_PARAM(Node, tooltip);
	PROP_ARRAY_STRUCT(ObjectAndSurface, glowing_objects);

	void setTrainingMode(int state);
	void disable() override;

private:
	void update();
	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;
	void on_shutdown() override;

	Tooltip *tooltip_component;
	Vector<ObjectPtr> objects;

	int is_training_mode;
	bool need_glow;
	float time;
};