
#pragma once
#include <stdexcept>

class CedarLogicException : public std::runtime_error {
public:
	CedarLogicException(const std::string &message) :
		std::runtime_error("CedarLogic Exception:" + message) { }
};