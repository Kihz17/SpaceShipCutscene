#include "GameObject.h"

#include "UUID.h"

#include <sstream>

std::string GameObject::GenerateFriendlyName()
{
	std::stringstream ss;
	ss << "Name" << UUID();
	return ss.str();
}