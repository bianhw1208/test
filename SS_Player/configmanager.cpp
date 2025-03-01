#include "configmanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextCodec>

ConfigManager* ConfigManager::instance = nullptr;

ConfigManager* ConfigManager::getInstance()
{
    if (!instance) {
        instance = new ConfigManager();
    }
    return instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_httpPort(8090)
    , m_httpThreadCount(100)
    , m_dbPort(3306)
    , m_playId(-1)
    , m_playType(0)
{
    // 设置控制台输出编码为UTF-8或GBK
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    // Qt5及以上版本
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    
    QFileInfo fileInfo(configPath);
    if (fileInfo.exists()) {
    } else {
        qDebug() << "config.ini not exists";
    }
    
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    loadConfig();
}

ConfigManager::~ConfigManager()
{
    delete m_settings;
}

void ConfigManager::loadConfig()
{
    m_settings->beginGroup("HTTP");
    m_httpIP = m_settings->value("httpIP", "192.168.3.148").toString();
    m_httpPort = m_settings->value("httpPort", 8090).toInt();
    m_httpThreadCount = m_settings->value("httpThreadCount", 10).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("Database");
    m_dbIP = m_settings->value("DBIP", "192.168.3.148").toString();
    m_dbPort = m_settings->value("DBPort", 3306).toInt();
    m_dbUser = m_settings->value("DBUser", "root").toString();
    m_dbPassword = m_settings->value("DBPW", "root").toString();
    m_dbName = m_settings->value("DBName", "videosmall").toString();
    m_settings->endGroup();

    m_settings->beginGroup("PlayInfo");
    m_playId = m_settings->value("playid", -1).toInt();
    m_playType = m_settings->value("playtype", 0).toInt();
    m_settings->endGroup();
}

void ConfigManager::reload()
{
    loadConfig();
}

// Getters
QString ConfigManager::getHttpIP() const { return m_httpIP; }
int ConfigManager::getHttpPort() const { return m_httpPort; }
int ConfigManager::getHttpThreadCount() const { return m_httpThreadCount; }

QString ConfigManager::getDbIP() const
{
    return m_dbIP;
}

int ConfigManager::getDbPort() const
{
    return m_dbPort;
}

QString ConfigManager::getDbUser() const
{
    return m_dbUser;
}

QString ConfigManager::getDbPassword() const
{
    return m_dbPassword;
}

QString ConfigManager::getDbName() const
{
    return m_dbName;
}

int ConfigManager::getPlayId() const { return m_playId; }
int ConfigManager::getPlayType() const { return m_playType; }

// Setters
void ConfigManager::setPlayId(int id)
{
    m_playId = id;
    m_settings->beginGroup("PlayInfo");
    m_settings->setValue("playid", id);
    m_settings->endGroup();
    m_settings->sync();
}

void ConfigManager::setPlayType(int type)
{
    m_playType = type;
    m_settings->beginGroup("PlayInfo");
    m_settings->setValue("playtype", type);
    m_settings->endGroup();
    m_settings->sync();
} 
