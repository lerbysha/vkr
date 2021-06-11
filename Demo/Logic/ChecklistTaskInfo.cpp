#include "ChecklistTaskInfo.h"


REGISTER_COMPONENT(ChecklistTaskInfo);


void ChecklistTaskInfo::init()
{
	if (!text)
	{
#if DEBUG
		Log::error("Checklist Task Info error: text is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!background)
	{
#if DEBUG
		Log::error("Checklist Task Info error: background is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!mark)
	{
#if DEBUG
		Log::error("Checklist Task Info error: mark is null, please check %s\n", node->getName());
#endif
		return;
	}

	text_object = checked_ptr_cast<ObjectText>(text.get());
	if (!text_object)
	{
#if DEBUG
		Log::error("Checklist Task Info error: text can't cast to ObjectText, please check %s\n", node->getName());
#endif
		return;
	}

	background_object = checked_ptr_cast<Object>(background.get());
	if (!background_object)
	{
#if DEBUG
		Log::error("Checklist Task Info error: background can't cast to Object, please check %s\n", node->getName());
#endif
		return;
	}

	mark_object = checked_ptr_cast<Object>(mark.get());
	if (!mark_object)
	{
#if DEBUG
		Log::error("Checklist Task Info error: mark can't cast to Object, please check %s\n", node->getName());
#endif
		return;
	}
}