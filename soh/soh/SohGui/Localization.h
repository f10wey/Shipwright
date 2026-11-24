#pragma once

// Russian translation layer for the port menu.
// Menu widgets keep their English names internally (they are used as keys, CVar paths and ImGui IDs);
// these helpers only change what is drawn on screen.

#include <string>

namespace SohLoc {

// Returns the Russian text for an English menu string, or the original text when there is no translation.
const char* Text(const char* english);
const char* Text(const std::string& english);

// Returns "Русский##English" so the visible text is translated while the ImGui ID stays the same.
std::string Label(const std::string& english);

} // namespace SohLoc
