#pragma once
#include "../VRInteractable.h"


class ObjInspectable : public VRInteractable
{
public:
	COMPONENT(ObjInspectable, VRInteractable);

	PROP_NAME("inspectable");
	PROP_AUTOSAVE(0);
};
