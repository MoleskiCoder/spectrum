#pragma once

#include <string>

class Configuration final {
public:
	Configuration() = default;

	[[nodiscard]] bool isDebugMode() const { return m_debugMode; }
	void setDebugMode(bool value) { m_debugMode = value; }

	[[nodiscard]] bool isProfileMode() const { return m_profileMode; }
	void setProfileMode(bool value) { m_profileMode = value; }

	[[nodiscard]] bool isDrawGraphics() const { return m_drawGraphics; }
	void setDrawGraphics(bool value) { m_drawGraphics = value; }

	[[nodiscard]] std::string getRomDirectory() const { return m_romDirectory; }
	[[nodiscard]] std::string getProgramDirectory() const { return m_programDirectory; }

private:
	bool m_debugMode = false;
	bool m_profileMode = false;
	bool m_drawGraphics = true;
	std::string m_romDirectory = "roms";
	std::string m_programDirectory = "programs";
};
