#ifndef Logger_hpp
#define Logger_hpp

#include <memory>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Logger {

template<typename... Args>
void trace(const char* fmt, const Args &... args)
{
    Logger::get().trace(fmt, args...);
}

template<typename... Args>
void debug(const char* fmt, const Args &... args)
{
    Logger::get().debug(fmt, args...);
}

template<typename... Args>
void info(const char* fmt, const Args &... args)
{
    Logger::get().info(fmt, args...);
}

template<typename... Args>
void error(const char* fmt, const Args &... args)
{
    Logger::get().error(fmt, args...);
}


class Logger {
public:
    static Logger& get() {
        static Logger instance;
        return instance;
    }

    void setLogDirectory(const std::string& dir) {
        m_directory = dir;
    }

    template<typename... Args>
    void trace(const char* fmt, const Args&... args) { m_logger->trace(fmt, args...); }
    
    template<typename... Args>
    void debug(const char* fmt, const Args&... args) { m_logger->debug(fmt, args...); }
    
    template<typename... Args>
    void info(const char* fmt, const Args&... args) { m_logger->info(fmt, args...); }
    
    template<typename... Args>
    void warning(const char* fmt, const Args&... args) { m_logger->warning(fmt, args...); }

    template<typename... Args>
    void error(const char* fmt, const Args&... args) { m_logger->error(fmt, args...); }

protected:
    void initFilepath() {
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

    void initLogger() {
        initFilepath();
        std::vector<spdlog::sink_ptr> sinks;
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%Y-%m-%d %T]%^[%l]%$ %v");
        sinks.push_back(console_sink);
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_full_filepath.c_str(), true);
        file_sink->set_pattern("[%Y-%m-%d %T]%^%^[%l]%$%$ %v");
        sinks.push_back(file_sink);
        m_logger = std::make_shared<spdlog::logger>("multi_sink", begin(sinks), end(sinks));
        spdlog::register_logger(m_logger);
    }

private:
    Logger() = default;
    std::shared_ptr<spdlog::logger> m_logger;
    std::string m_directory;
    std::string m_full_filepath;
};

} // namespace Logger

#endif