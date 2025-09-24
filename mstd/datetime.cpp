
#include "datetime.h"
#include "platform.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <stdexcept>


namespace mstd
{
	// 验证日期时间有效性
	void datetime::validate() const {
		if (month_ < 1 || month_ > 12) {
			throw std::invalid_argument("Invalid month");
		}
		if (day_ < 1 || day_ > days_in_month(year_, month_)) {
			throw std::invalid_argument("Invalid day");
		}
		if (hour_ < 0 || hour_ > 23) {
			throw std::invalid_argument("Invalid hour");
		}
		if (minute_ < 0 || minute_ > 59) {
			throw std::invalid_argument("Invalid minute");
		}
		if (second_ < 0 || second_ > 59) {
			throw std::invalid_argument("Invalid second");
		}
	}

	datetime datetime::now() {
		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
#ifdef MSTD_WINDOWS
		std::tm tm;
		localtime_s(&tm, &time);
		return datetime(
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec
		);
#else
		std::tm* tm = std::localtime(&time);
		return datetime(
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec
		);
#endif
	}

	// 返回周几 (0=周日, 1=周一, ..., 6=周六)
	int datetime::day_of_week() const {
		// Zeller's congruence algorithm
		int y = year_;
		int m = month_;
		if (m < 3) {
			m += 12;
			y -= 1;
		}
		int k = y % 100; // year of the century
		int j = y / 100; // zero-based century
		int h = (day_ + 13 * (m + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
		return (h + 5) % 7; // convert to 0=Sunday, 1=Monday, etc.
	}

	std::time_t datetime::to_time_t() const {
		std::tm tm = {};
		tm.tm_year = year_ - 1900;
		tm.tm_mon = month_ - 1;
		tm.tm_mday = day_;
		tm.tm_hour = hour_;
		tm.tm_min = minute_;
		tm.tm_sec = second_;
		return std::mktime(&tm);
	}

	// 从时间戳创建 DateTime
	datetime datetime::from_time_t(std::time_t time) {
#ifdef MSTD_WINDOWS
		std::tm tm;
		localtime_s(&tm, &time);
		return datetime(
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec
		);
#else
		std::tm* tm = std::localtime(&time);
		return datetime(
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec
		);
#endif
	}

	datetime datetime::operator-(duration d) const {
		std::time_t time = to_time_t();
		time -= d.get_total_seconds(); // 减去秒数
		return from_time_t(time);
	}

	datetime datetime::operator+(duration d) const {
		std::time_t time = to_time_t();
		time += d.get_total_seconds(); // 加上秒数
		return from_time_t(time);
	}

	bool datetime::operator==(const datetime& other) const {
		return year_ == other.year_ && month_ == other.month_ && day_ == other.day_ &&
			hour_ == other.hour_ && minute_ == other.minute_ && second_ == other.second_;
	}

	bool datetime::operator!=(const datetime& other) const {
		return !(*this == other);
	}

	bool datetime::operator<(const datetime& other) const {
		if (year_ != other.year_) return year_ < other.year_;
		if (month_ != other.month_) return month_ < other.month_;
		if (day_ != other.day_) return day_ < other.day_;
		if (hour_ != other.hour_) return hour_ < other.hour_;
		if (minute_ != other.minute_) return minute_ < other.minute_;
		return second_ < other.second_;
	}

	bool datetime::operator<=(const datetime& other) const {
		return !(other < *this);
	}

	bool datetime::operator>(const datetime& other) const {
		return other < *this;
	}

	bool datetime::operator>=(const datetime& other) const {
		return !(*this < other);
	}

	std::string datetime::to_string() const {
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
			year_, month_, day_, hour_, minute_, second_);
		return std::string(buffer);
	}

	// 判断是否为闰年
	bool datetime::is_leap_year(int year) {
		return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	}

	// 获取某年某月的天数
	int datetime::days_in_month(int year, int month) {
		static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		if (month == 2 && is_leap_year(year)) {
			return 29;
		}
		return days[month - 1];
	}

}//end namespace mstd
