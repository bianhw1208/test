#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QVariant>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager* getInstance();
    
    // 数据库连接管理
    bool connect(const QString& host, int port, const QString& dbName,
                const QString& user, const QString& password);
    void disconnect();
    bool isConnected() const;
    QString lastError() const;
    
    // 事务管理
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    // 查询执行
    QSqlQuery executeQuery(const QString& sql);
    bool executeNonQuery(const QString& sql);

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    static DatabaseManager* instance;
    QSqlDatabase m_db;
    QMutex m_mutex;
    QString m_lastError;
    
    void setLastError(const QString& error);
};

#endif // DATABASEMANAGER_H 
