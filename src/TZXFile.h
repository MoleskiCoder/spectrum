#pragma once

#include <string>

#include "Loader.h"

class TZXFile : public Loader {
public:
	TZXFile(std::string path);

	void load(Board& board) override;
};

