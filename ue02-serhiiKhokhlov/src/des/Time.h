#include <iostream>
#include <string>
#include <cmath>

//the whole part is the day count and the fractional part is the daytime

class TimeConverter {
public:
	static std::string toTimeString(double t) {
		int day = (int)t;
		int hour = int((t - day) * 24);
		int minute = int(((t - day) * 24 - hour) * 60);
		int second = int((((t - day) * 24 - hour) * 60 - minute) * 60);
		return std::to_string(day) + "d " + std::to_string(hour) + "h " + std::to_string(minute) + "m " + std::to_string(second) + "s";
	}

	static std::string toCSVTimeString(double t) {
		int day = int(t);
		int hour = int((t - day) * 24);
		int minute = int(((t - day) * 24 - hour) * 60);
		int second = int((((t - day) * 24 - hour) * 60 - minute) * 60);
		// HH:MM:SS

		return (hour < 10 ? "0" : "") + std::to_string(hour) + ":" + 
			   (minute < 10 ? "0" : "") + std::to_string(minute) + ":" + 
			   (second < 10 ? "0" : "") + std::to_string(second);
	}

	static double toTime(int day, int hour, int minute, int second) {
		return double(day) + double(hour) / 24.0 + double(minute) / (24.0 * 60) + double(second) / (24.0 * 60 * 60);
	}
};