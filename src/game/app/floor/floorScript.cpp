#include <floor.h>
#include <GSscript.h>

static long _floorScriputResult;

namespace {
enum ScriptMode {
    ScriptModeFloor = 0,
    ScriptModeCommon = 2,
    ScriptModeModule = 3
};
const int CurrentScriptModule = 1;
const unsigned long ScriptGroupMask = 0xffff0000;
const unsigned long CommonScriptGroupID = 1430;
}

long floorExecScriptIntegerByNameThread(const char* name, int parameter1, int parameter2, int parameter3, int parameter4)
{
    GSscript::Tiga* script = floorGetCurentScript();
    if (!script)
        return 0;
    if (!script->isMapping())
        return 0;
    int functionID = script->getNameToFuncId(name);
    if (functionID == 0) {
        return 0;
    } else {
        return floorExecScriptIntegerThread(functionID, parameter1, parameter2, parameter3, parameter4);
    }
}

int floorExecScriptIntegerByName(const char* name, int parameter1, int parameter2, int parameter3, int parameter4)
{
    GSscript::Tiga* script = floorGetCurentScript();
    if (!script)
        return 0;
    if (!script->isMapping())
        return 0;
    int functionID = script->getNameToFuncId(name);
    if (functionID == 0) {
        return 0;
    } else {
        return floorExecScriptInteger(functionID, parameter1, parameter2, parameter3, parameter4);
    }
}

int floorExecScriptRes(int scriptID, unsigned long groupID, unsigned long resourceID)
{
    if (!floorGetCurentScript())
        return 0;
    int mode = floorGetScriptMode(scriptID);
    GSresID* people = floorGetCurentScript()->searchResID(groupID, resourceID);
    if (!people)
        return 0;
    GSscript::tigaVariant parameter;
    parameter.setPeople(people);
    int success;
    if (mode == ScriptModeCommon) {
        success = floorGetCommonScript()->runParamsWait(scriptID, &parameter, 1, 0);
        _floorScriputResult = floorGetCommonScript()->getResultValue(success);
    } else {
        success = floorGetCurentScript()->runParamsWait(scriptID, &parameter, 1, 0);
        _floorScriputResult = floorGetCurentScript()->getResultValue(success);
    }
    return success;
}

long floorExecScriptResThread(int scriptID, unsigned long groupID, unsigned long resourceID)
{
    if (!floorGetCurentScript())
        return 0;
    int mode = floorGetScriptMode(scriptID);
    GSresID* people = floorGetCurentScript()->searchResID(groupID, resourceID);
    if (!people)
        return 0;
    GSscript::tigaVariant parameter;
    parameter.setPeople(people);
    if (mode == ScriptModeCommon)
        return floorGetCommonScript()->runParams(scriptID, &parameter, 1, true);
    return floorGetCurentScript()->runParams(scriptID, &parameter, 1, true);
}

int floorGetScriptResult(void)
{
    return _floorScriputResult;
}

int floorExecScriptInteger(int scriptID, int parameter1, int parameter2, int parameter3, int parameter4)
{
    if (!floorGetCurentScript())
        return 0;
    int mode = floorGetScriptMode(scriptID);
    GSscript::tigaVariant parameters[4];
    parameters[0].setInteger(parameter1);
    parameters[1].setInteger(parameter2);
    parameters[2].setInteger(parameter3);
    parameters[3].setInteger(parameter4);
    int success;
    if (mode == ScriptModeCommon) {
        success = floorGetCommonScript()->runParamsWait(scriptID, parameters, 4, 0);
        _floorScriputResult = floorGetCommonScript()->getResultValue(success);
    } else {
        success = floorGetCurentScript()->runParamsWait(scriptID, parameters, 4, 0);
        _floorScriputResult = floorGetCurentScript()->getResultValue(success);
    }
    return success;
}

long floorExecScriptIntegerThread(int scriptID, int parameter1, int parameter2, int parameter3, int parameter4)
{
    int mode = floorGetScriptMode(scriptID);
    if (!floorGetCurentScript())
        return 0;
    GSscript::tigaVariant parameters[4];
    parameters[0].setInteger(parameter1);
    parameters[1].setInteger(parameter2);
    parameters[2].setInteger(parameter3);
    parameters[3].setInteger(parameter4);
    if (mode == ScriptModeCommon)
        return floorGetCommonScript()->runParams(scriptID, parameters, 4, true);
    return floorGetCurentScript()->runParams(scriptID, parameters, 4, true);
}

bool floorIsRunningThread(long threadHandle)
{
    GSscript::Tiga* script = floorGetCurentScript();
    if (script == 0) {
        return false;
    } else {
        return script->isRunningThread(threadHandle);
    }
}

void floorBlockScriptThread(long threadHandle)
{
    if (floorGetCurentScript())
        floorGetCurentScript()->blockThread(threadHandle);
}

void floorUnblockScriptThread(long threadHandle)
{
    if (floorGetCurentScript())
        floorGetCurentScript()->unblockThread(threadHandle);
}

int floorGetScriptMode(int scriptID)
{
    if ((scriptID & ScriptGroupMask) == (CommonScriptGroupID << 16))
        return ScriptModeCommon;
    if (floorGetCurentScriptMode() == CurrentScriptModule)
        return ScriptModeModule;
    return ScriptModeFloor;
}
