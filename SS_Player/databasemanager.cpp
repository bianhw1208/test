#include "databasemanager.h"
#include "configmanager.h"
#include <QDebug>

DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager* DatabaseManager::getInstance()
{
    if (!instance) {
        instance = new DatabaseManager();
    }
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

bool DatabaseManager::connect(const QString& host, int port, const QString& dbName,
                            const QString& user, const QString& password)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_db.isOpen()) {
        return true;
    }

    // 检查可用的数据库驱动
    qDebug() << "Available database drivers:" << QSqlDatabase::drivers();

    // 检查 MYSQL 驱动是否可用
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        setLastError("MySQL driver not available. Please install MySQL driver for Qt.");
        qDebug() << "MySQL driver not found. Available drivers:" << QSqlDatabase::drivers();
        return false;
    }
    
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);
    
    if (!m_db.open()) {
        setLastError(m_db.lastError().text());
        qDebug() << "Failed to connect to database:" << m_lastError;
        return false;
    }
    
    qDebug() << "Successfully connected to database, host=" << host <<", port=" << port;
    return true;
}

void DatabaseManager::disconnect()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

void DatabaseManager::setLastError(const QString& error)
{
    m_lastError = error;
}

bool DatabaseManager::beginTransaction()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        setLastError("Database is not connected");
        return false;
    }
    
    if (!m_db.transaction()) {
        setLastError(m_db.lastError().text());
        return false;
    }
    
    return true;
}

bool DatabaseManager::commitTransaction()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        setLastError("Database is not connected");
        return false;
    }
    
    if (!m_db.commit()) {
        setLastError(m_db.lastError().text());
        return false;
    }
    
    return true;
}

bool DatabaseManager::rollbackTransaction()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_db.isOpen()) {
        setLastError("Database is not connected");
        return false;
    }
    
    if (!m_db.rollback()) {
        setLastError(m_db.lastError().text());
        return false;
    }
    
    return true;
}

QSqlQuery DatabaseManager::executeQuery(const QString& sql)
{
    QMutexLocker locker(&m_mutex);
    
    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        setLastError(query.lastError().text());
        qDebug() << "Query failed:" << sql << "\nError:" << m_lastError;
    }
    
    return query;
}

bool DatabaseManager::executeNonQuery(const QString& sql)
{
    QMutexLocker locker(&m_mutex);
    
    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        setLastError(query.lastError().text());
        qDebug() << "Query failed:" << sql << "\nError:" << m_lastError;
        return false;
    }
    
    return true;
} 
