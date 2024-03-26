#ifndef Logger_hpp
#define Logger_hpp

#include "spdlog/spdlog.h"
#include <spdlog/common.h>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Logger {
public:
	enum OutputMode {
		opm_None,
		opm_File = 1,
		opm_Console = 2,
	};

	static Logger& get() {
		static Logger& logger = createLogger(OutputMode::opm_Console);
		return logger;
	}

	static Logger& createLogger(int output);

	void setLogDirectory(const std::string& dir);

	void setLevel(spdlog::level::level_enum level);

	template<typename... Args>
	void trace(spdlog::string_view_t fmt, const Args &... args)
	{
		m_logger->trace(fmt, args...);
	}

	template<typename... Args>
	void debug(spdlog::string_view_t fmt, const Args &... args)
	{
		m_logger->debug(fmt, args...);
	}

	template<typename... Args>
	void info(spdlog::string_view_t fmt, const Args &... args)
	{
		m_logger->info(fmt, args...);
	}

	template<typename... Args>
	void error(spdlog::string_view_t fmt, const Args &... args)
	{
		m_logger->error(fmt, args...);
	}

protected:
	virtual void create() = 0;

	void init_filename();

	std::shared_ptr<spdlog::logger> m_logger;
	std::string m_directory;
	std::string m_full_filename;
	OutputMode m_output_mode;
};

class FileLogger : public Logger {
public:
	FileLogger();

protected:
	void create() override;
};

class ConsoleLogger : public Logger {
public:
	ConsoleLogger();

protected:
	void create() override;
};

class MultiLogger : public Logger {
public:
	MultiLogger();

protected:
	void create() override;
};

#endif