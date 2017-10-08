#pragma once

#include <string>

class Configuration {
public:
	Configuration();

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
	bool m_debugMode;
	bool m_profileMode;
	bool m_drawGraphics;
	bool m_vsyncLocked;
	std::string m_romDirectory;
};
