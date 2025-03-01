#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QMutex>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include "videomanager.h" // 包含VideoManager头文件以使用其枚举类型

class MainWindow;
class QJsonObject;
class VideoManager;

// 定义窗口状态结构
struct WindowStatus {
    bool isPlaying;
    QString source;
    QString type;  // "rtsp" 或 "hik"
};

// 处理请求的任务类
class HttpRequestHandler : public QRunnable
{
public:
    HttpRequestHandler(QTcpSocket* socket, MainWindow* mainWindow, VideoManager* videoManager);
    void run() override;

private:
    void handleRequest(const QByteArray& requestData);
    void sendJsonResponse(const QJsonObject& jsonResponse);
    void sendXmlResponse(const QString& xmlResponse);
    
    // GET 请求处理函数
    void handleSingleSwitchCamera(const QStringList& params);
    void handleGroupSwitchCamera(const QStringList& params);
    void handleCameraMax(const QStringList& params);
    void handleGetGroupInfo();
    void handlePTZCtrlCamera(const QStringList& params);
    void handlePlayBackByTime(const QStringList& params);
    void handlePlayBackCtrl(const QStringList& params);
    void handleGetPlayBackPos();
    void handleSetPlayBackPos(const QStringList& params);
    void handleFindFileByTime(const QStringList& params);
    void handlePlayBackByFile(const QStringList& params);
    void handleGetPlayBackTime();
    void handleGroupInfo(const QStringList& params);
    
    // POST 请求处理函数
    void handlePlayRequest(const QJsonObject& requestJson);
    void handleStopRequest(const QJsonObject& requestJson);
    void handleControlRequest(const QJsonObject& requestJson);
    
    // GET 请求处理函数
    void handleGetStatus(const QStringList& params);
    void handleGetWindows(const QStringList& params);
    
    // 辅助函数
    QStringList parseQueryParams(const QString& queryString);
    QDateTime parseDateTime(const QString& dateTimeStr);
    QString generateGroupListXml();
    QString generatePlaybackPosXml(int pos);
    QString generatePlaybackTimeXml(int playTime, int allTime);
    QString generateHistoryFilesXml(const QVector<QPair<QString, QPair<QDateTime, QDateTime>>>& files);
    QString generateGroupInfoXml(int groupId);
    QJsonObject createWindowStatusJson(const WindowStatus& status, int index);

    QTcpSocket* m_socket;
    MainWindow* m_mainWindow;
    VideoManager* m_videoManager;
    QMutex m_mutex;
};

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(QObject* parent = nullptr, VideoManager* videoManager = nullptr, MainWindow* mainWindow = nullptr);
    ~HttpServer();

    bool start(int port);
    void stop();

private:
    QTcpServer* m_server;
    QThreadPool* m_threadPool;
    VideoManager* m_videoManager;
    MainWindow* m_mainWindow;
};

#endif // HTTPSERVER_H 
