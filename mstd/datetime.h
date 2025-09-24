
#ifndef __DATETIME_H_
#define __DATETIME_H_
#include <ctime>
#include <string>

namespace mstd {
	class datetime {
	public:
		class duration {
		public:
			// 各种时间单位的构造函数
			duration() : total_seconds(0) {}
			explicit duration(int seconds) : total_seconds(seconds) {}

			static duration seconds(int s) { return duration(s); }
			static duration minutes(int m) { return duration(m * 60); }
			static duration hours(int h) { return duration(h * 3600); }
			static duration days(int d) { return duration(d * 86400); }
			static duration weeks(int w) { return duration(w * 7 * 86400); }
			// 考虑月份和年的复杂性（近似值）
			static duration months(int m) { return duration(m * 30 * 86400); } // 近似：每月30天
			static duration years(int y) { return duration(y * 365 * 86400); } // 近似：每年365天

			// 获取各种单位的数值
			int get_total_seconds() const { return total_seconds; }
			int get_seconds() const { return total_seconds % 60; }
			int get_minutes() const { return (total_seconds / 60) % 60; }
			int get_hours() const { return (total_seconds / 3600) % 24; }
			int get_days() const { return total_seconds / 86400; }

			// 算术运算符
			duration operator+(const duration& other) const {
				return duration(total_seconds + other.total_seconds);
			}

			duration operator-(const duration& other) const {
				return duration(total_seconds - other.total_seconds);
			}

			duration operator*(int factor) const {
				return duration(total_seconds * factor);
			}

			// 比较运算符
			bool operator==(const duration& other) const {
				return total_seconds == other.total_seconds;
			}

			bool operator!=(const duration& other) const {
				return !(*this == other);
			}

			bool operator<(const duration& other) const {
				return total_seconds < other.total_seconds;
			}

			bool operator>(const duration& other) const {
				return other < *this;
			}

			bool operator<=(const duration& other) const {
				return !(other < *this);
			}

			bool operator>=(const duration& other) const {
				return !(*this < other);
			}

			// 输出字符串表示
			std::string to_string() const {
				if (total_seconds == 0) return "0 seconds";

				int days = get_days();
				int hours = get_hours();
				int minutes = get_minutes();
				int seconds = get_seconds();

				std::string result;
				if (days > 0) result += std::to_string(days) + " days ";
				if (hours > 0) result += std::to_string(hours) + " hours ";
				if (minutes > 0) result += std::to_string(minutes) + " minutes ";
				if (seconds > 0) result += std::to_string(seconds) + " seconds";

				return result.empty() ? "0 seconds" : result;
			}

		private:
			int total_seconds; // 内部统一存储为秒数
		};

		// 默认构造函数
		datetime() : year_(1970), month_(1), day_(1), hour_(0), minute_(0), second_(0) {}

		// 参数构造函数
		datetime(int y, int mo, int d, int h = 0, int mi = 0, int s = 0)
			: year_(y), month_(mo), day_(d), hour_(h), minute_(mi), second_(s) {
			validate();
		}

		// 验证日期时间有效性
		void validate() const;
		// 返回当前日期时间
		static datetime now();
		// 返回周几 (0=周日, 1=周一, ..., 6=周六)
		int day_of_week() const;
		// 转换为时间戳（秒 since 1970-01-01 00:00:00）
		std::time_t to_time_t() const;
		// 从时间戳创建 DateTime
		static datetime from_time_t(std::time_t time);

		// 获取duration前d秒的日期时间
		datetime operator-(duration d) const;
		// 获取duration后d秒的日期时间
		datetime operator+(duration d) const;
		// 比较运算符
		bool operator==(const datetime& other) const;
		bool operator!=(const datetime& other) const;
		bool operator<(const datetime& other) const;
		bool operator>(const datetime& other) const;
		bool operator<=(const datetime& other) const;
		bool operator>=(const datetime& other) const;
		// 输出格式化字符串
		std::string to_string() const;
	private:
		// 判断是否为闰年
		static bool is_leap_year(int year);
		// 获取某年某月的天数
		static int days_in_month(int year, int month);
	private:
		int year_;
		int month_;
		int day_;
		int hour_;
		int minute_;
		int second_;
	};


}//end namespace mstd

#endif //__DATETIME_H_
