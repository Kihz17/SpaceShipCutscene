#include "ScriptParser.h"
#include "InstructionSet.h"
#include "RotateTimeInstruction.h"
#include "RotateSpeedInstruction.h"
#include "SerialInstructionSet.h"
#include "MoveToTimeInstruction.h"
#include "MoveToSpeedInstruction.h"
#include "FollowInstruction.h"
#include "AABBTriggerInstruction.h"
#include "FollowCircleInstruction.h"
#include "ParallelInstructionSet.h"
#include "FollowWaypointsInstruction.h"

#include <fstream>
#include <iostream>

static const std::string instructions[8] =
{
	"MoveToSpeed",
	"MoveToTime",
	"RotateSpeed",
	"RotateTime",
	"Follow",
	"FollowCircle",
	"AABBTrigger",
	"FollowWaypoints",
};

static const std::string instructionSetTypes[3] =
{
	"Serial",
	"Parallel",
	"Async"
};

static const std::string instructionSetStart = "Start";
static const std::string instructionSetEnd = "End";

static const char instructionEnd = '|';
static const char argumentDivider = ',';

static bool IsInstruction(const std::string& instruction)
{
	for (const std::string& type : instructions)
	{
		if (type == instruction) return true;
	}

	return false;
}

static bool IsInstructionSet(const std::string& instructionSet, bool start)
{
	for (const std::string& type : instructionSetTypes)
	{
		std::string compare(type + (start ? instructionSetStart : instructionSetEnd));
		if (compare == instructionSet) return true;
	}

	return false;
}

static std::vector<std::string> Split(const std::string& value, char delimiter, size_t npos = 0)
{
	std::vector<std::string> results;

	size_t index = value.find_first_of(delimiter, npos);
	if (index == std::string::npos)
	{
		results.push_back(value);
		return results;
	}

	size_t subLen = index - npos;
	std::string sub = value.substr(npos, index);
	results.push_back(sub);

	for (std::string& s : Split(value.substr(index + 1), delimiter, npos))
	{
		results.push_back(s);
	}

	return results;
}

static std::string Trim(const std::string& value)
{
	const size_t strBegin = value.find_first_not_of(" \t");
	if (strBegin == std::string::npos) return "";

	const size_t strEnd = value.find_last_not_of(" \t");
	const size_t strRange = strEnd - strBegin + 1;

	return value.substr(strBegin, strRange);
}

InstructionSetType GetInstructionSetType(const std::string& value)
{
	size_t index = value.find(instructionSetStart); // Trim off "start"

	if (index == std::string::npos) // Start wasn't there, trim off "End"
	{
		index = value.find(instructionSetEnd);
	}
	
	if (index == std::string::npos)
	{
		std::cout << "Could not get InstructionType from value " << value << std::endl;
		return InstructionSetType::None;
	}
		
	std::string trimmedString = value.substr(0, index);
	if (trimmedString == instructionSetTypes[0])
	{
		return InstructionSetType::Serial;
	}
	else if (trimmedString == instructionSetTypes[1])
	{
		return InstructionSetType::Parallel;
	}
	else if (trimmedString == instructionSetTypes[2])
	{
		return InstructionSetType::Async;
	}

	std::cout << "Could not find InstructionType from value " << value << std::endl;
	return InstructionSetType::None;
}

static Instruction* ParseInstruction(const std::string& line)
{
	std::vector<std::string> split = Split(line, instructionEnd);
	if (split.size() <= 1) // No arguments
	{
		std::cout << "No arguments for instructon " << split[0] << std::endl;
		return nullptr;
	}

	std::string instruction = Trim(split[0]);
	if (!IsInstruction(instruction)) // Invalid instructon 
	{
		std::cout << "Instruction " << instruction << " unrecognized." << std::endl;
		return nullptr;
	}

	std::vector<std::string> argSplit = Split(split[1], argumentDivider);

	if (instruction == instructions[0])
	{
		if (argSplit.size() != 5)
		{
			std::cout << "Could not construct MoveToSpeedInstruction." << std::endl;
			return nullptr;
		}

		return new MoveToSpeedInstruction(Trim(argSplit[0]), glm::vec3(std::stof(Trim(argSplit[1])), std::stof(Trim(argSplit[2])), std::stof(Trim(argSplit[3]))), std::stof(Trim(argSplit[4])));
;	}
	else if (instruction == instructions[1])
	{
		if (argSplit.size() != 5)
		{
			std::cout << "Could not construct MoveToTimeInstruction." << std::endl;
			return nullptr;
		}

		return new MoveToTimeInstruction(Trim(argSplit[0]), glm::vec3(std::stof(Trim(argSplit[1])), std::stof(Trim(argSplit[2])), std::stof(Trim(argSplit[3]))), std::stof(Trim(argSplit[4])));
	}
	else if (instruction == instructions[2])
	{
		if (argSplit.size() != 7)
		{
			std::cout << "Could not construct RotateSpeedInstruction." << std::endl;
			return nullptr;
		}

		return new RotateSpeedInstruction(Trim(argSplit[0]),
			glm::vec3(std::stof(Trim(argSplit[1])), std::stof(Trim(argSplit[2])), std::stof(Trim(argSplit[3]))),
			glm::vec3(std::stof(Trim(argSplit[4])), std::stof(Trim(argSplit[5])), std::stof(Trim(argSplit[6]))));
	}
	else if (instruction == instructions[3])
	{
		return new RotateTimeInstruction(Trim(argSplit[0]),	glm::vec3(std::stof(Trim(argSplit[1])), std::stof(Trim(argSplit[2])), std::stof(Trim(argSplit[3]))), std::stof(Trim(argSplit[4])));
	}
	else if (instruction == instructions[4])
	{
		if (argSplit.size() != 8)
		{
			std::cout << "Could not construct FollowInstruction." << std::endl;
			return nullptr;
		}
		return new FollowInstruction(Trim(argSplit[0]), Trim(argSplit[1]), std::stof(Trim(argSplit[2])), std::stof(Trim(argSplit[3])),
			glm::vec3(std::stof(Trim(argSplit[4])), std::stof(Trim(argSplit[5])), std::stof(Trim(argSplit[6]))), std::stof(Trim(argSplit[7])));
	}
	else if (instruction == instructions[5])
	{
		if (argSplit.size() != 9)
		{
			std::cout << "Could not construct FollowCircleInstruction." << std::endl;
			return nullptr;
		}

		return new FollowCircleInstruction(Trim(argSplit[0]), glm::vec3(std::stof(Trim(argSplit[1])), std::stof(Trim(argSplit[2])), std::stof(Trim(argSplit[3]))),
			std::stof(Trim(argSplit[4])), std::stof(Trim(argSplit[5])), std::stoi(Trim(argSplit[6])), std::stoi(Trim(argSplit[7])), (Trim(argSplit[8])) == "true" ? true : false);
	}
	else if (instruction == instructions[6])
	{
		if (argSplit.size() != 3)
		{
			std::cout << "Could not construct AABBTriggerInstruction." << std::endl;
			return nullptr;
		}

		std::string instructionToRun(argSplit[2] + instructionEnd + split[2]);
		return new AABBTriggerInstruction(Trim(argSplit[0]), Trim(argSplit[1]), ParseInstruction(Trim(instructionToRun)));
	}
	else if (instruction == instructions[7])
	{
		int wayPointArgs = argSplit.size() - 2;
		if (wayPointArgs % 3 != 0)
		{
			std::cout << "Waypoint arguments must be a multiple of 3. " << instruction << std::endl;
			return nullptr;
		}

		std::vector<glm::vec3> waypoints;
		int wayPointCount = wayPointArgs / 3;
		int argIndex = 2;
		for (int i = 0; i < wayPointCount; i++)
		{
			float x = std::stof(Trim(argSplit[argIndex++]));
			float y = std::stof(Trim(argSplit[argIndex++]));
			float z = std::stof(Trim(argSplit[argIndex++]));
			glm::vec3 point(x, y, z);
			waypoints.push_back(point);
		}
			
		return new FollowWaypointsInstruction(Trim(argSplit[0]), std::stof(Trim(argSplit[1])), waypoints);
	}

	std::cout << "No parser for instruction " << instruction << std::endl;
	return nullptr;
}

static void CleanUpFaliure(InstructionScheduler* scheduler, std::vector<Instruction*> instructions)
{
	delete scheduler;
	for (Instruction* i : instructions)
	{
		delete i;
	}
}

InstructionScheduler* ScriptParser::ParseScript(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.good())
	{
		std::cout << "Could not find file " << path << "." << std::endl;
		return nullptr;
	}

	std::vector<std::string> lines;
	for (std::string line; std::getline(ifs, line);)
	{
		if (line.rfind("//", 0) && !line.empty()) // Don't count commented out lines
		{
			lines.push_back(line);
		}
	}

	InstructionScheduler* scheduler = new InstructionScheduler();
	std::vector<Instruction*> currentInstructions;

	InstructionSetType currentSetType = InstructionSetType::None;
	int repeats = 0;
	int currentInstructionSetStartIndex = -1;
	int currentInstructionSetEndIndex = -1;
	for (int i = 0; i < lines.size(); i++)
	{
		std::string line = lines[i];
		if (currentInstructionSetStartIndex == -1) // Look for an instruction set start identifer
		{
			std::vector<std::string> split = Split(line, '=');

			std::string instructionSetIndicator = split[0];
			if (split.size() > 1) // We have repeat value
			{
				repeats = std::stoi(Trim(split[1]));
			}

			std::string trimmed = Trim(instructionSetIndicator);
			if (IsInstructionSet(trimmed, true))
			{
				currentInstructionSetStartIndex = i;
				currentSetType = GetInstructionSetType(trimmed);
				continue;
			}
			else
			{
				std::cout << "Could not find InstructionSet starting point!" << std::endl;
				CleanUpFaliure(scheduler, currentInstructions);
				return nullptr;
			}
		}
		else if (currentInstructionSetStartIndex != -1 && currentInstructionSetEndIndex == -1) // Look for instruction set end identifier
		{
			std::string trimmed = Trim(line);
			if (IsInstructionSet(trimmed, false))
			{
				if (GetInstructionSetType(trimmed) == currentSetType)
				{
					currentInstructionSetEndIndex = i;
					i = currentInstructionSetStartIndex; // Move back to where the instruction set begins
				}
				else
				{
					std::cout << "Tried to end " << lines[currentInstructionSetStartIndex] << " with " << trimmed;
					CleanUpFaliure(scheduler, currentInstructions);
					return nullptr;
				}
			}
			continue;
		}
		
		// We have an instruction set, start pushing the instructions in between

		if (i == currentInstructionSetEndIndex) // We have reached the end of our instruction set, reset and move on
		{
			InstructionSet* instructionSet;
			if (currentSetType == InstructionSetType::Serial)
			{
				instructionSet = new SerialInstructionSet(repeats, currentInstructions);
			}
			else if (currentSetType == InstructionSetType::Parallel)
			{
				instructionSet = new ParallelInstructionSet(currentInstructions);
			}
			else if (currentSetType == InstructionSetType::Async)
			{
				instructionSet = new ParallelInstructionSet(currentInstructions, true);
			}
			else
			{
				std::cout << "No instruction type for " << (int) currentSetType << std::endl;
				CleanUpFaliure(scheduler, currentInstructions);
				return nullptr;
			}

			scheduler->PushInstructionSet(instructionSet);

			currentInstructions.clear();
			currentSetType = InstructionSetType::None;
			repeats = 0;
			currentInstructionSetStartIndex = -1;
			currentInstructionSetEndIndex = -1;
			continue;
		}

		Instruction* instruction = ParseInstruction(line);
		if (instruction == nullptr) // Failed to parse
		{
			CleanUpFaliure(scheduler, currentInstructions);
			return nullptr; 
		}

		currentInstructions.push_back(instruction);
	}

	if (currentInstructionSetStartIndex != -1 && currentInstructionSetEndIndex == -1) // We started an instruction set but never closed it
	{
		std::cout << "Could not find InstructionSet ending point for InstructionSet " << lines[currentInstructionSetStartIndex] << std::endl;
		CleanUpFaliure(scheduler, currentInstructions);
		return nullptr;
	}

	return scheduler;
}