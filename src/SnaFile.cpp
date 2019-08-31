#include "stdafx.h"
#include "SnaFile.h"

SnaFile::SnaFile(const std::string& path)
: m_path(path) {
}

void SnaFile::load() {
	m_rom.load(m_path);
}
