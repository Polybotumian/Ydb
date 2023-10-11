#pragma once

#include "Ylog_Logger_Interfaces.h"

namespace Ylog
{
	class ConsoleLogger : public Ylog::Interfaces::ILogger
	{
	public:
		ConsoleLogger(
			std::uint8_t logLevel,
			const char* timestamp_format = "%Y-%m-%d %H:%M:%S"
		);
		~ConsoleLogger();
		void Log(std::uint8_t loglevel, std::string log_message) override;
	};

	class FileLogger : public Ylog::Interfaces::IFileLogger
	{
	public:
		FileLogger(
			const char* file_path,
			std::uint8_t logLevel,
			const char* timestamp_format = "%Y-%m-%d %H:%M:%S"
		);
		~FileLogger();
		void Log(std::uint8_t loglevel, std::string log_message) override;
	};

	class RFileLogger : public Ylog::Interfaces::IRFLogger
	{
	public:
		RFileLogger(
			const char* file_path,
			std::uint8_t logLevel,
			const char* timestamp_format = "%Y-%m-%d %H:%M:%S"
		);
		~RFileLogger();
		void Log(std::uint8_t loglevel, std::string log_message) override;
	};

	class CRFLogger : public Ylog::Interfaces::ICRFLog
	{
	public:
		CRFLogger(
			const char* file_path,
			std::uint8_t logLevel,
			const char* new_log_format = "[{0}] - ({1}) - {2} : {3}",
			const char* timestamp_format = "%Y-%m-%d %H:%M:%S"
		);
		~CRFLogger();
		void Log(std::uint8_t loglevel, std::string log_message) override;
	};
}