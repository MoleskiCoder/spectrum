#include "stdafx.h"
#include "Configuration.h"

Configuration::Configuration()
: m_debugMode(false),
  m_profileMode(false),
  m_drawGraphics(true),
  m_vsyncLocked(true),
  m_romDirectory("roms") {
}
