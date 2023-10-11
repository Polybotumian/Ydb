#include "Ylog_Logger_Predefs.h"
#include <iostream>

Ylog::ConsoleLogger::ConsoleLogger
(
    std::uint8_t logLevel,
    const char* timestamp_format
)
{
    _loglevel = logLevel;
    _timestampFormat = timestamp_format;
}
Ylog::ConsoleLogger::~ConsoleLogger()
{

}
void Ylog::ConsoleLogger::Log(std::uint8_t loglevel, std::string log_message)
{
    std::ostringstream logEntry;
    logEntry << '[' << std::this_thread::get_id()
        << "] - [" << GetTimestamp()
        << "] - [" << _logLevels[_loglevel]
        << "] : " << log_message << '\n';

    // Output the log entry to the console in one operation
    std::cout << logEntry.str() << std::endl;
}
//************************************************************************************************
Ylog::FileLogger::FileLogger(
    const char* file_path,
    std::uint8_t logLevel,
    const char* timestamp_format
)
{
    try
    {
        _path = file_path;
        RemoveLogs(_path, _path.extension().string());
        _loglevel = logLevel;
        _timestampFormat = timestamp_format;
        _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
        if (!_file.is_open())
        {
            throw std::runtime_error("Ylog::FileLogger::FileLogger(): Failed to open \"" + _path.filename().string() + "\".");
        }
    }
    catch (const std::exception& ex)
    {
        if (!_file.is_open())
        {
            _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
        }
        std::string err_message = ex.what();
        Log(Ylog::Enums::Levels::ERROR, err_message);
        _file.close();
    }
};
Ylog::FileLogger::~FileLogger()
{

};
void Ylog::FileLogger::Log(std::uint8_t loglevel, std::string log_message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (loglevel >= _loglevel)
    {
        _file << '[' << std::this_thread::get_id()
           << "] - [" << GetTimestamp()
           << "] - [" << _logLevels[_loglevel]
           << "] : " << log_message << '\n';
    }
};
//************************************************************************************************
Ylog::RFileLogger::RFileLogger(
    const char* file_path,
    std::uint8_t logLevel,
    const char* timestamp_format
)
{
    try
    {
        _path = file_path;
        RemoveLogs(_path, _path.extension().string());
        _loglevel = logLevel;
        _timestampFormat = timestamp_format;
        _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
        if (!_file.is_open())
        {
            throw std::runtime_error("Ylog::RFileLogger::RFileLogger(): Failed to open \"" + _path.filename().string() + "\".");
        }
    }
    catch (const std::exception& ex)
    {
        if (!_file.is_open())
        {
            _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
        }
        std::string err_message = ex.what();
        Log(Ylog::Enums::Levels::ERROR, err_message);
        _file.close();
    }
};
Ylog::RFileLogger::~RFileLogger()
{
    try
    {
        _file.close();
        if (std::rename(
            (_path.filename().string()).c_str(),
            (_path.stem().string()
                + '_' +
                std::to_string(_rotated_file_count)
                + _path.extension().string()).c_str()
        ) != 0)
        {
            throw std::runtime_error("Ylog::RFileLogger::~RFileLogger(): Failed to rename \"" + _path.filename().string() + "\".");
        }
    }
    catch (const std::exception& ex)
    {
        _file.close();
        _file.open("Ylog_RFileLogger.log", std::ios::out | std::ios::trunc | std::ios::binary);
        std::string err_message = ex.what();
        Log(Ylog::Enums::Levels::ERROR, err_message);
        _file.close();
    }
};
void Ylog::RFileLogger::Log(std::uint8_t loglevel, std::string log_message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (loglevel >= _loglevel)
    {
        Rotate();
        _file << '[' << std::this_thread::get_id()
            << "] - [" << GetTimestamp()
            << "] - [" << _logLevels[_loglevel]
            << "] : " << log_message << '\n';
    }
};
//************************************************************************************************
Ylog::CRFLogger::CRFLogger(
    const char* file_path,
    std::uint8_t logLevel,
    const char* new_log_format,
    const char* timestamp_format
)
{
    try
    {
        _path = file_path;
        RemoveLogs(_path, _path.extension().string());
        _loglevel = logLevel;
        _log_format = new_log_format;
        _timestampFormat = timestamp_format;
        _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
        if (!_file.is_open())
        {
            throw std::runtime_error("Ylog::CRFLogger::CRFLogger(): Failed to open \"" + _path.filename().string() + "\".");
        }
    }
    catch (const std::exception& ex)
    {
        if (!_file.is_open())
        {
            _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
        }
        std::string err_message = ex.what();
        Log(Ylog::Enums::Levels::ERROR, err_message);
        _file.close();
    }
};
Ylog::CRFLogger::~CRFLogger()
{
    try
    {
        _file.close();
        if (std::rename(
            (_path.filename().string()).c_str(),
            (_path.stem().string()
                        + '_' +
                        std::to_string(_rotated_file_count)
                        + _path.extension().string()).c_str()
        ) != 0)
        {
            throw std::runtime_error("Ylog::CRFLogger::~CRFLogger(): Failed to rename \"" + _path.filename().string() + "\".");
        }
    }
    catch (const std::exception& ex)
    {
        _file.close();
        _file.open("Ylog_CRFLogger.log", std::ios::out | std::ios::trunc | std::ios::binary);
        std::string err_message = ex.what();
        Log(Ylog::Enums::Levels::ERROR, err_message);
        _file.close();
    }
};
void Ylog::CRFLogger::Log(std::uint8_t loglevel, std::string log_message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (loglevel >= _loglevel)
    {
        Rotate();
        std::ostringstream ss(_log_format);
        FormatLog(ss, loglevel, log_message);
        _file << ss.str() << '\n';
    }
};
//************************************************************************************************