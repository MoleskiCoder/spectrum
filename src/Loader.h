#pragma once

#include <string>

class Board;

class Loader {
private:
	std::string m_path;

protected:
	Loader(std::string path) : m_path(path) {}

public:
	[[nodiscard]] auto path() const { return m_path; }

	virtual void load(Board& board) = 0;
};

