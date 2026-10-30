#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>


class Logger {
public:
    enum LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    static Logger &instance();

    void init(const QString &logDir = QString());
    void setLogoLevel(LogLevel level);

    void log(LogLevel level, const QString &message, const char *file = nullptr, int line = 0);

    void debug(const QString &message, const char *file = nullptr, int line = 0);
    void info(const QString &message, const char *file = nullptr, int line = 0);
    void warning(const QString &message, const char *file = nullptr, int line = 0);
    void error(const QString &message, const char *file = nullptr, int line = 0);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger &operator =(const Logger&) = delete;

    void rotateLogFile();
    QString levelToString(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString &message, const char *file, int line)const;

    QFile *logFile;
    QTextStream *stream;
    QMutex mutex;
    LogLevel loglevel;
    QString logDir;
    qint64 maxFileSize;
};

#define LOG_DEBUG(msg) Logger::instance().debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Logger::instance().info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::instance().warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::instance().error(msg, __FILE__, __LINE__)

#endif