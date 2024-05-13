#include "Logger.hpp"

namespace Logger {

Logger& Logger::createLogger(int output)
{
    if (output == (OutputMode::opm_Console | OutputMode::opm_File))
    {
        return *(new MultiLogger());
    }
    else if (output == OutputMode::opm_Console)
    {
        return *(new ConsoleLogger());
    }
    else if ((output == OutputMode::opm_File))
    {
        return *(new FileLogger());
    }
    else {
        return *(new MultiLogger());
    }
}

void Logger::init_filepath()
{
    struct tm* cur_time;
    time_t local_time;
    time(&local_time);
    cur_time = localtime(&local_time);

    /* ͨ��ʱ��������־�ļ� */
    char filepath[256];
    strftime(filepath, 100, "%Y-%m-%d_%H-%M-%S.log", cur_time);

    /* log file name, does not contain file path */
    std::string log_filepath = std::string(filepath);

    /* ��־�ļ�ȫ·�� */
    m_full_filepath = m_directory + log_filepath;

    /* ��¼��־����ʱ�䣬��ͨ��ʱ�䳤�ȿ�����־�ļ���Сʱ���ó�Ա�����ᱻʹ�� */
    //logger_create_time = local_time;
}

void Logger::setLogDirectory(const std::string& dir)
{
    m_directory = dir;
}

void Logger::setLevel(spdlog::level::level_enum level)
{
    m_logger->set_level(level);
}


FileLogger::FileLogger()
{
    init_filepath();
    create();
}

void FileLogger::create()
{
    m_logger = spdlog::basic_logger_mt("basic_logger", m_full_filepath.c_str());
    spdlog::set_pattern("[%Y-%m-%d %T]%^[%l]%$ %v");
    m_logger->set_level(spdlog::level::trace);
}


ConsoleLogger::ConsoleLogger()
{
    create();
}

void ConsoleLogger::create()
{
    m_logger = spdlog::stdout_color_mt("console");
    spdlog::set_pattern("[%Y-%m-%d %T]%^[%l]%$ %v");
    m_logger->set_level(spdlog::level::trace);
}


MultiLogger::MultiLogger()
{
    init_filepath();
    create();
}

void MultiLogger::create()
{
    /* ͨ��multi-sink�ķ�ʽ��������logger��ʵ�ַ�ʽΪ���ȷֱ𴴽��ļ�sink�Ϳ���̨sink���������߷���sink �����У����һ������logger */
    /* file sink */
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_full_filepath.c_str(), true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %T]%^%^[%l]%$%$ %v");

    /* ����̨sink */
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    //console_sink->set_pattern("%+");
    console_sink->set_pattern("[%Y-%m-%d %T]%^[%l]%$ %v");

    /* Sink��� */
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(console_sink);
    sinks.push_back(file_sink);
    m_logger = std::make_shared<spdlog::logger>("multi-sink", begin(sinks), end(sinks));
}

}