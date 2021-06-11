#pragma once
#include "../VRInteractable.h"


class ObjSelectable : public VRInteractable
{
public:
	COMPONENT(ObjSelectable, VRInteractable);

	PROP_NAME("selectable");
	PROP_AUTOSAVE(0);
};
