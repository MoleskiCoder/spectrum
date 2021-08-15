#include "stdafx.h"

#include "TZXFile.h"

TZXFile::TZXFile(std::string path)
: Loader(path) {}

void TZXFile::load(Board& board) {
	throw std::runtime_error("Whoops: not implemented!");
}
