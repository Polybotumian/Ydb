#pragma once
// General
#include "Ylog_Enums.h"
#include <string>
//	ILog
#include <vector>
#include <mutex>
//	ILogFile
#include <bitset>
#include <filesystem>
#include <fstream>
#include <sstream>
//	ILogPrint

namespace Ylog
{
	namespace Interfaces
	{
		class ILogger
		{
		public:
			virtual void Log(std::uint8_t loglevel, std::string log_message) = 0;
			void SetLogLevel(std::uint8_t loglevel);
			void SetTimestampFormat(std::string format);
		protected:
			std::mutex _mutex;
			std::uint8_t _loglevel;
			std::string _timestampFormat;
			const std::string _logLevels[Ylog::Enums::Levels::LEVEL_COUNT]
			{
				"TRACE", "DEBUG", "VERBOSE", "INFO", "WARNING", "ERROR", "CRITICAL", "FATAL"
			};
			std::string GetTimestamp();
		};
		class IFileLogger : public ILogger
		{
		public:
			void Configure(
				const std::string& file_path,
				const std::uint8_t& new_loglevel,
				const std::string& new_timestamp_format
			);
		protected:
			std::ofstream _file;
			std::filesystem::path _path;
			void RemoveLogs(const std::filesystem::path& path, const std::string& extension);
		};
		class IRFLogger : public IFileLogger
		{
		public:
			enum Modes
			{
				ROTATE,
				MODE_COUNT
			};

			void SetRotationSize(const std::uint32_t& new_size);
			void Set(const std::uint8_t& mode, bool status);
			bool IsRotates();
		protected:
			std::uint32_t _max_size{ 1073741824 };
			std::uint32_t _rotated_file_count{ 0 };
			std::bitset<Ylog::Interfaces::IRFLogger::Modes::MODE_COUNT> _modes;
			void Rotate();
		};
		class ICRFLog : public IRFLogger
		{
		public:
			void SetLogFormat(std::string format);

		protected:
			std::string _log_format;
			void FormatLog(std::ostringstream& ss, std::uint8_t& loglevel, std::string& log_message);
		};
	}
}