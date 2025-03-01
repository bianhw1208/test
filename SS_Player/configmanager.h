#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static ConfigManager* getInstance();
    
    // HTTP配置
    QString getHttpIP() const;
    int getHttpPort() const;
    int getHttpThreadCount() const;
    
    // 数据库配置
    QString getDbIP() const;
    int getDbPort() const;
    QString getDbUser() const;
    QString getDbPassword() const;
    QString getDbName() const;
    
    // 播放配置
    int getPlayId() const;
    int getPlayType() const;
    
    // 保存播放配置
    void setPlayId(int id);
    void setPlayType(int type);
    
    // 重新加载配置
    void reload();

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();
    
    void loadConfig();
    
    static ConfigManager* instance;
    QSettings* m_settings;
    
    // 缓存的配置值
    QString m_httpIP;
    int m_httpPort;
    int m_httpThreadCount;
    
    QString m_dbIP;
    int m_dbPort;
    QString m_dbUser;
    QString m_dbPassword;
    QString m_dbName;
    
    int m_playId;
    int m_playType;
};

#endif // CONFIGMANAGER_H 