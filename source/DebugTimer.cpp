#include "DebugTimer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

DebugTimer::DebugTimer()
:
_measured(),
_current_start_time()
{
	begin();
}

DebugTimer::~DebugTimer()
{
	// NIX
}

void DebugTimer::begin()
{
	QueryPerformanceCounter(&_current_start_time);
}

void DebugTimer::end(const std::string& str)
{
	LARGE_INTEGER current_end_time;
	QueryPerformanceCounter(&current_end_time);

	std::map<std::string, std::pair<long long, unsigned int> >::iterator it = _measured.find(str);
	if (it == _measured.end())
	{
		_measured[str] = make_pair(current_end_time.QuadPart - _current_start_time.QuadPart, 1);
	}
	else
	{
		it->second.first += current_end_time.QuadPart - _current_start_time.QuadPart;
		++(it->second.second);
	}
}

void DebugTimer::print() const
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// determine length of longest string entry
	const string MARK_TITLE = "Interval";
	size_t max_mark_length = MARK_TITLE.size();
	double max_diff_time = 0;
	double total_time = 0;
	for (map<string, pair<long long, unsigned int> >::const_iterator it = _measured.begin(); it != _measured.end(); ++it)
	{
		max_mark_length = max(it->first.size(), max_mark_length);

		double diff_time = it->second.first * 1000 / double(frequency.QuadPart);
		total_time += diff_time;
		max_diff_time = max(diff_time, max_diff_time);
	}
	stringstream str_stream;
	str_stream << fixed << setprecision(3) << max_diff_time;
	string max_diff_time_str;
	str_stream >> max_diff_time_str;
	max_diff_time_str += " ms";
	const string TIME_TITLE = "Time";
	const size_t max_time_length = max(TIME_TITLE.size(), max_diff_time_str.size());

	// print
	cout << endl;
	cout << appendToMatchLength(MARK_TITLE, max_mark_length, " ") << " | " << appendToMatchLength(TIME_TITLE, max_time_length, " ") << " | #Calls" << endl;
	cout << appendToMatchLength("", max_mark_length, "-")        << "---" << appendToMatchLength("", max_time_length, "-")     << "---------" << endl;
	
	for (map<string, pair<long long, unsigned int> >::const_iterator it = _measured.begin(); it != _measured.end(); ++it)
	{
		double diff_time = it->second.first * 1000 / double(frequency.QuadPart);

		stringstream str_stream;
		str_stream << fixed << setprecision(3) << diff_time;
		string diff_time_str;
		str_stream >> diff_time_str;
		diff_time_str += " ms";
		const size_t diff_time_str_length = diff_time_str.size();

		cout << appendToMatchLength(it->first, max_mark_length, " ") << " | " << appendToMatchLength(diff_time_str, max_mark_length, " ") << " | " << it->second.second << endl;
	}
	cout << appendToMatchLength("", max_mark_length, "-") << "---" << appendToMatchLength("", max_time_length, "-") << "---------" << endl;
	cout << appendToMatchLength("Total", max_mark_length, " ") << " | " << fixed << setprecision(3) << total_time << " ms" << endl;
	cout << endl;
	if (total_time > 1)
		cout << "NOW" << endl;
}

string DebugTimer::appendToMatchLength(const string& str, size_t target_length, const string& chr)
{
	string result_str(str);
	while (result_str.size() < target_length)
		result_str += chr;
	return result_str;
}
