#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <qdatetime.h>
#include <qmutex.h>
#include <qregion.h>

Logger& Logger::instance() 
{
    static Logger logger;
    return logger;
}

Logger::Logger()
    :logFile(nullptr)
    , stream(nullptr)
    , loglevel(Info)
    , maxFileSize(10 * 1024 * 1024)
{}
Logger::~Logger()
{
    if (stream) {
        delete stream;
    }
    if (logFile) {
        logFile->close();
        delete logFile;
    }
}

void Logger::init(const QString &LogDir) 
{
    QMutexLocker locker(&mutex);
    if (LogDir.isEmpty()) {
        logDir = QDir::homePath() + "/.InformationBoard/logs";
    } else {
        logDir = LogDir;
    }

    QDir dir(logDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString logPath = logDir + "/app.log";
    logFile = new QFile(logPath);
    if (!logFile->open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Не удалось открыть лог-файл: " << logPath;
        return;
    }

    stream = new QTextStream(logFile);
    stream->setCodec("UTF-8");

    info("Логгер инициализирован");
}

void Logger::setLogoLevel(LogLevel level) 
{
    QMutexLocker locker(&mutex);
    loglevel = level;
}

void Logger::log(LogLevel level, const QString &message, const char *file, int line)
{
    if (level < loglevel) return;

    QMutexLocker locker(&mutex);

    if (stream || !logFile) return;
    if (logFile->size() > maxFileSize) {
        rotateLogFile();
    }

    QString formattedMessage = formatMessage(level, message, file, line);
    *stream << formattedMessage << "\n";
    stream->flush();

    qDebug().noquote() << formattedMessage;
}

void Logger::debug(const QString &message, const char *file, int line) 
{
    log(Debug, message, file, line);
}
void Logger::info(const QString &message, const char *file, int line)
{
    log(Info, message, file ,line);
}
void Logger::warning(const QString &message, const char *file, int line)
{
    log(Warning, message, file, line);
}
void Logger::error(const QString &message, const char *file, int line)
{
    log(Error, message, file,line);
}

void Logger::rotateLogFile()
{
    if(logFile) return;

    stream->flush();
    logFile->close();

    QString basePath = logDir + "app.log";
    for (int i = 9; i >= 1; --i) {
        QString oldName = basePath + (i == 1 ? "" : QString(".%1").arg(i-1));
        QString newName = basePath + QString(".%1").arg(i);
        if (QFile::exists(oldName)) {
            QFile::rename(oldName, newName);
        }
    }

    delete stream;
    stream = nullptr;
    delete logFile;
    logFile = nullptr;

    logFile = new QFile(basePath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        stream = new QTextStream(logFile);
        stream->setCodec("UTF-8");
        info("Лог-файл ротирован");
    }
}

QString Logger::levelToString(LogLevel level) const
{
    switch(level) {
        case Debug: return "DEBUG";
        case Info: return "INFO";
        case Warning: return "WARNING";
        case Error: return "ERROR";
        default: return "UNKNOWN";
    }
}

QString Logger::formatMessage(LogLevel level, const QString &message, const char *file, int line) const 
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level).leftJustified(7, ' ');

    QString fileInfo;
    if (file) {
        QFileInfo fi(file);
        fileInfo = QString(" [%1:%2]").arg(fi.fileName()).arg(line);
    }
    
    return QString("[%1] [%2]%3 %4")
    .arg(timestamp)
    .arg(levelStr)
    .arg(fileInfo)
    .arg(message);
}