#pragma once

#include <string>

class Configuration final {
public:
	Configuration() = default;

	bool isDebugMode() const {
		return m_debugMode;
	}

	void setDebugMode(bool value) {
		m_debugMode = value;
	}

	bool isProfileMode() const {
		return m_profileMode;
	}

	void setProfileMode(bool value) {
		m_profileMode = value;
	}

	bool isDrawGraphics() const {
		return m_drawGraphics;
	}

	void setDrawGraphics(bool value) {
		m_drawGraphics = value;
	}

	bool getVsyncLocked() const {
		return m_vsyncLocked;
	}

	void setVsyncLocked(bool value) {
		m_vsyncLocked = value;
	}

	std::string getRomDirectory() const {
		return m_romDirectory;
	}

private:
	bool m_debugMode = false;
	bool m_profileMode = false;
	bool m_drawGraphics = true;
	bool m_vsyncLocked = true;
	std::string m_romDirectory = "roms";
};
