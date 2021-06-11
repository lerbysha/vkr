#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>

using namespace Unigine;

class ChecklistTaskInfo : public Unigine::ComponentBase
{
public:
	COMPONENT(ChecklistTaskInfo, Unigine::ComponentBase);
	COMPONENT_INIT(init, -1);

	PROP_NAME("checklist_task_info");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, text);
	PROP_PARAM(Node, background);
	PROP_PARAM(Node, mark);

	ObjectTextPtr getText() { return text_object; }
	ObjectPtr getBackground() { return background_object; }
	ObjectPtr getMark() { return mark_object; }

private:
	void init();

	ObjectTextPtr text_object;
	ObjectPtr background_object;
	ObjectPtr mark_object;
};
