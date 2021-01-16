#ifndef DEBUGTIMER_H
#define DEBUGTIMER_H

#include <string>
#include <map>
#include <windows.h>

class DebugTimer
{
public:
	DebugTimer();
	~DebugTimer();

	void begin();
	void end(const std::string& str);
	void print() const;

private:
	static std::string appendToMatchLength(const std::string& str, size_t target_length, const std::string& chr);

	std::map<std::string, std::pair<long long, unsigned int> > _measured; // <name, <total_time, ncalls>>
	LARGE_INTEGER _current_start_time;
};

#endif
