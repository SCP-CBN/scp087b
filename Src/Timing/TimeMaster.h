#ifndef B_TIMEMASTER_H_INCLUDED
#define B_TIMEMASTER_H_INCLUDED

#include "Time.h"
#include "Timer.h"

class TimeMaster {
	friend Timer;

	public:
		PGE::String print() const;

	private:
		Time top;
		Time* current = &top;
};

#endif // B_TIMEMASTER_H_INCLUDED
