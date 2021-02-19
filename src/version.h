
#pragma once
#include <string>

#define CEDARLOGIC_REVISION "B"

// Get version major, minor, and build date in lexicographically sortable order.
// It may seem silly to use date instead of version number, but I know we'll forget
// to manually update the version number.
// Besides, the version number is still the primary sort because it comes first in the
// VERSION_NUMBER_STRING anyway.
std::string VERSION_NUMBER_STRING();

std::string VERSION_REVISION_STRING();

// Get title bar text.
std::string VERSION_TITLE();

// Get about dialog text.
std::string VERSION_ABOUT_TEXT();