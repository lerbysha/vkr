#include <UnigineEngine.h>

#include "AppSystemLogic.h"
#include "AppWorldLogic.h"

#include <UnigineInterface.h>
#include <UnigineInterpreter.h>

#include "CameraCinematic.h"

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif

	Unigine::EnginePtr engine(UNIGINE_VERSION, argc, argv);

	AppSystemLogic system_logic;
	AppWorldLogic world_logic;

	// add functions from C++ to UnigineScript
	Unigine::Interpreter::addExternDefine("CPP_CODE_ENABLED");
	Unigine::Interpreter::addExternFunction("setGeneratorInteractiveMode", MakeExternObjectFunction(&world_logic, &AppWorldLogic::setGeneratorInteractiveMode));
	Unigine::Interpreter::addExternFunction("setUdhInteractiveMode", MakeExternObjectFunction(&world_logic, &AppWorldLogic::setUdhInteractiveMode));
	Unigine::Interpreter::addExternFunction("setFreeMode", MakeExternObjectFunction(&world_logic, &AppWorldLogic::setFreeMode));
	Unigine::Interpreter::addExternFunction("setTelemetryMode", MakeExternObjectFunction(&world_logic, &AppWorldLogic::setTelemetryMode));
	Unigine::Interpreter::addExternFunction("setCinematicMode", MakeExternFunction(&CameraCinematic::setCinematicMode));
	Unigine::Interpreter::addExternFunction("changedQualitySettings", MakeExternFunction(&CameraCinematic::changedQualitySettings));

	engine->main(&system_logic, &world_logic);

	return 0;
}
