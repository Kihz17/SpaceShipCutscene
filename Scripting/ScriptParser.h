#pragma once 

#include "InstructionScheduler.h"

#include <string>

class ScriptParser
{
public:
	static InstructionScheduler* ParseScript(const std::string& path);
};