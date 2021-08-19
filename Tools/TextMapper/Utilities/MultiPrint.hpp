#pragma once

#include <iostream>
#include <iomanip>

struct Multi {
	char ch;
	int count;
	bool append = true;
};

std::ostream& operator<<(std::ostream& out, const Multi& m) {
	out << std::setfill(m.ch) << std::setw(m.count);
	if (m.append) { out << ""; }
	return out;
}
