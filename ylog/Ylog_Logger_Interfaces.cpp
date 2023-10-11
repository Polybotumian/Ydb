#include "Ylog_Logger_Interfaces.h"

void Ylog::Interfaces::ILogger::SetLogLevel(std::uint8_t loglevel)
{
    _loglevel = loglevel;
};
void Ylog::Interfaces::ILogger::SetTimestampFormat(std::string new_format)
{
    _timestampFormat = new_format;
};
std::string Ylog::Interfaces::ILogger::GetTimestamp()
{
    try
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        struct tm timeInfo;
        if (localtime_s(&timeInfo, &currentTime) == 0)
        {
            char timeStr[100];
            std::strftime(timeStr, sizeof(timeStr), _timestampFormat.c_str(), &timeInfo);
            return timeStr;
        }
        else
        {
            throw std::runtime_error("Ylog::Interfaces::ILog::GetTimestamp(): Failed to get timestamp!");
        }
    }
    catch (const std::exception& ex)
    {
        return ex.what();
    }
};
//************************************************************************************************
void Ylog::Interfaces::IFileLogger::RemoveLogs(const std::filesystem::path& path, const std::string& extension)
{
    std::filesystem::directory_iterator it(path.parent_path());
    for (const auto& entry : it) 
    {
        if (std::filesystem::is_regular_file(entry)
            && entry.path().extension() == extension
            && entry.path().filename().string()._Starts_with(_path.stem().string())
            ) 
        {
            std::filesystem::remove(entry);
        }
    }
};
void Ylog::Interfaces::IFileLogger::Configure(
    const std::string& file_path,
    const std::uint8_t& new_loglevel,
    const std::string& new_timestamp_format
)
{
    try
    {
        if (_file.is_open() == true)
        {
            _file.close();
        }
        if (std::rename((_path.filename().string()).c_str(), (_path.filename().string()).c_str()))
        {
            throw std::runtime_error("Ylog::FileLogger::Configure(): Failed to rename with new configuration!");
        }
        _loglevel = new_loglevel;
        _timestampFormat = new_timestamp_format;

        _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);

        if (!_file.is_open()) {
            throw std::runtime_error("Ylog::FileLogger::Configure(): Failed to reopen with new configuration!");
        }
    }
    catch (const std::exception& ex)
    {
        _file.close();
        _file.open("FileLogger.log", std::ios::out | std::ios::trunc | std::ios::binary);
        std::string err_message = ex.what();
        Log(Ylog::Enums::Levels::ERROR, err_message);
        _file.close();
    }
};
//************************************************************************************************
bool Ylog::Interfaces::IRFLogger::IsRotates()
{
    return _modes[Ylog::Interfaces::IRFLogger::Modes::ROTATE];
};
void Ylog::Interfaces::IRFLogger::Rotate()
{
    _file.flush();
    if (_modes[Modes::ROTATE] == 1 && std::filesystem::file_size(_path) >= _max_size)
    {
        try
        {
            _file.close();
            if (std::rename((_path.filename().string()).c_str(), (std::to_string(_rotated_file_count) + "_" + _path.filename().string()).c_str()) != 0)
            {
                throw std::runtime_error("Ylog::Logger::Rotate(): Failed to rotate " + _path.filename().string());
            }
            else
            {
                _rotated_file_count++;
                _file.open(_path.filename().string(), std::ios::out | std::ios::trunc | std::ios::binary);
            }
        }
        catch (const std::exception& ex)
        {
            _file.close();
            _file.open("FileLogger.log", std::ios::out | std::ios::trunc | std::ios::binary);
            std::string err_message = ex.what();
            Log(Ylog::Enums::Levels::ERROR, err_message);
            _file.close();
        }
    }
};
void Ylog::Interfaces::IRFLogger::Set(const std::uint8_t& mode, bool status)
{
    if (status)
    {
        _modes.set(mode);
    }
    else
    {
        _modes.reset(mode);
    }
};
void Ylog::Interfaces::IRFLogger::SetRotationSize(const std::uint32_t& new_size)
{
    _max_size = new_size;
};
//************************************************************************************************
void Ylog::Interfaces::ICRFLog::SetLogFormat(std::string format)
{
    _log_format = format;
};
void Ylog::Interfaces::ICRFLog::FormatLog(
    std::ostringstream& ss,
    std::uint8_t& loglevel,
    std::string& log_message
)
{
    for (size_t i{ 0 }; i < _log_format.length(); i++)
    {
        if (_log_format[i] == '{')
        {
            i++;
            switch (_log_format[i])
            {
            case Ylog::Enums::MessageFormatFlags::THREAD_ID:
                ss << std::this_thread::get_id();
                break;
            case Ylog::Enums::MessageFormatFlags::TIMESTAMP:
                ss << GetTimestamp();
                break;
            case Ylog::Enums::MessageFormatFlags::LOG_LEVEL:
                ss << _logLevels[_loglevel];
                break;
            case Ylog::Enums::MessageFormatFlags::MESSAGE:
                ss << log_message;
                break;
            }
        }
        else if (_log_format[i] != '}')
        {
            ss << _log_format[i];
        }
    }
};