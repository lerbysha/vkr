#include "Accident.h"


REGISTER_COMPONENT(Accident);


const char *Accident::getDescription()
{
	return description.get();
}