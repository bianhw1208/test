#include "httpserver.h"
#include "mainwindow.h"
#include "configmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QHostAddress>

HttpRequestHandler::HttpRequestHandler(QTcpSocket* socket, MainWindow* mainWindow, VideoManager* videoManager)
    : m_socket(socket)
    , m_mainWindow(mainWindow)
    , m_videoManager(videoManager)
{
    // 设置为自动删除
    setAutoDelete(true);
}

void HttpRequestHandler::run()
{
    if (!m_socket) return;

    // 等待数据完全接收
    while (m_socket->waitForReadyRead(1000)) {
        QByteArray requestData = m_socket->readAll();
        handleRequest(requestData);
    }

    // 关闭连接
    m_socket->disconnectFromHost();
    m_socket->deleteLater();
}

void HttpRequestHandler::handleRequest(const QByteArray& requestData)
{
    QString request = QString::fromUtf8(requestData);
    QStringList lines = request.split("\r\n");
    
    if (lines.isEmpty()) return;

    QStringList requestLine = lines[0].split(" ");
    if (requestLine.size() < 2) return;

    QString method = requestLine[0];
    QString fullPath = requestLine[1];

    // 解析路径和查询参数
    QStringList pathParts = fullPath.split("?");
    QString path = pathParts[0];
    QStringList queryParams;
    if (pathParts.size() > 1) {
        queryParams = parseQueryParams(pathParts[1]);
    }

    // 处理各种请求
    if (path == "/SingleSwitchCamera") {
        handleSingleSwitchCamera(queryParams);
    }
    else if (path == "/GroupSwitchCamera") {
        handleGroupSwitchCamera(queryParams);
    }
    else if (path == "/CameraMax") {
        handleCameraMax(queryParams);
    }
    else if (path == "/GetGroupInfo") {
        handleGetGroupInfo();
    }
    else if (path == "/PTZCtrlCamera") {
        handlePTZCtrlCamera(queryParams);
    }
    else if (path == "/PlayBackByTime") {
        handlePlayBackByTime(queryParams);
    }
    else if (path == "/PlayBackCtrl") {
        handlePlayBackCtrl(queryParams);
    }
    else if (path == "/GetPlayBackPos") {
        handleGetPlayBackPos();
    }
    else if (path == "/SetPlayBackPos") {
        handleSetPlayBackPos(queryParams);
    }
    else if (path == "/FindFileByTime") {
        handleFindFileByTime(queryParams);
    }
    else if (path == "/PlayBackByFile") {
        handlePlayBackByFile(queryParams);
    }
    else if (path == "/GetPlayBackTime") {
        handleGetPlayBackTime();
    }
    else if (path == "/GroupInfo") {
        handleGroupInfo(queryParams);
    }
    else {
        QJsonObject response{
            {"status", "error"},
            {"message", "Invalid path"}
        };
        sendJsonResponse(response);
    }
}

void HttpRequestHandler::sendJsonResponse(const QJsonObject& jsonResponse)
{
    QMutexLocker locker(&m_mutex);  // 线程安全的响应发送

    QByteArray responseData = QJsonDocument(jsonResponse).toJson();
    
    QByteArray response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + QByteArray::number(responseData.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += responseData;

    m_socket->write(response);
    m_socket->flush();
}

void HttpRequestHandler::handlePlayRequest(const QJsonObject& requestJson)
{
    QString ip = requestJson["ip"].toString();
    QString username = requestJson["username"].toString();
    QString password = requestJson["password"].toString();
    int windowIndex = requestJson["windowIndex"].toInt();

    // TODO: 调用主窗口的播放功能
    // 注意：需要使用信号槽或其他线程安全的方式调用UI相关函数
    // QMetaObject::invokeMethod(m_mainWindow, "startHikCamera",
    //                          Qt::QueuedConnection,
    //                          Q_ARG(QString, ip),
    //                          Q_ARG(QString, username),
    //                          Q_ARG(QString, password),
    //                          Q_ARG(int, windowIndex));

    QJsonObject response{
        {"status", "success"},
        {"message", "Play request received"}
    };
    sendJsonResponse(response);
}

void HttpRequestHandler::handleStopRequest(const QJsonObject& requestJson)
{
    int windowIndex = requestJson["windowIndex"].toInt();

    // TODO: 调用主窗口的停止功能
    // m_mainWindow->stopPlayback(windowIndex);

    QJsonObject response{
        {"status", "success"},
        {"message", "Stop request received"}
    };
    sendJsonResponse(response);
}

void HttpRequestHandler::handleControlRequest(const QJsonObject& requestJson)
{
    // TODO: 处理控制命令

    QJsonObject response{
        {"status", "success"},
        {"message", "Control request received"}
    };
    sendJsonResponse(response);
}

QJsonObject HttpRequestHandler::createWindowStatusJson(const WindowStatus& status, int index)
{
    return QJsonObject{
        {"index", index},
        {"isPlaying", status.isPlaying},
        {"source", status.source},
        {"type", status.type}
    };
}

void HttpRequestHandler::handleGetStatus(const QStringList& params)
{
    int windowIndex = -1;
    for (const QString& param : params) {
        if (param.startsWith("window=")) {
            windowIndex = param.mid(7).toInt();
            break;
        }
    }

    QJsonObject response;
    if (windowIndex >= 0) {
        // 获取指定窗口的状态
        WindowStatus status = m_mainWindow->getWindowStatus(windowIndex);
        response = QJsonObject{
            {"status", "success"},
            {"window", createWindowStatusJson(status, windowIndex)}
        };
    } else {
        // 返回所有窗口的状态
        QJsonArray windows;
        QVector<WindowStatus> statuses = m_mainWindow->getAllWindowStatus();
        for (int i = 0; i < statuses.size(); ++i) {
            windows.append(createWindowStatusJson(statuses[i], i));
        }
        response = QJsonObject{
            {"status", "success"},
            {"windows", windows}
        };
    }
    
    sendJsonResponse(response);
}

void HttpRequestHandler::handleGetWindows(const QStringList& params)
{
    Q_UNUSED(params);
    
    int rows, cols;
    m_mainWindow->getWindowLayout(rows, cols);
    
    QJsonObject response{
        {"status", "success"},
        {"rows", rows},
        {"cols", cols},
        {"total", rows * cols}
    };
    
    sendJsonResponse(response);
}

QStringList HttpRequestHandler::parseQueryParams(const QString& queryString)
{
    return queryString.split("&", Qt::SkipEmptyParts);
}

HttpServer::HttpServer(QObject* parent, VideoManager* videoManager, MainWindow* mainWindow)
    : QObject(parent)
    , m_videoManager(videoManager)
    , m_mainWindow(mainWindow)
{
    m_server = new QTcpServer(this);
    
    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(ConfigManager::getInstance()->getHttpThreadCount());
    
    connect(m_server, &QTcpServer::newConnection, this, [this]() {
        while (m_server->hasPendingConnections()) {
            QTcpSocket* socket = m_server->nextPendingConnection();
            HttpRequestHandler* handler = new HttpRequestHandler(socket, m_mainWindow, m_videoManager);
            m_threadPool->start(handler);
        }
    });
}

bool HttpServer::start(int port)
{
    if (!m_server) {
        qDebug() << "Server not initialized";
        return false;
    }
    
    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Failed to start server on port" << port << ":" << m_server->errorString();
        return false;
    }
    
    qDebug() << "HTTP server started on port" << port;
    return true;
}

HttpServer::~HttpServer()
{
    stop();
}

void HttpServer::stop()
{
    if (m_server && m_server->isListening()) {
        m_server->close();
        qDebug() << "HTTP server stopped";
    }
    
    if (m_threadPool) {
        m_threadPool->waitForDone();
    }
}

void HttpRequestHandler::sendXmlResponse(const QString& xmlResponse)
{
    QMutexLocker locker(&m_mutex);  // 线程安全的响应发送

    QByteArray responseData = xmlResponse.toUtf8();
    
    QByteArray response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/xml; charset=utf-8\r\n";
    response += "Content-Length: " + QByteArray::number(responseData.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += responseData;

    m_socket->write(response);
    m_socket->flush();
}

// 1. 单点播放接口
void HttpRequestHandler::handleSingleSwitchCamera(const QStringList& params)
{
    int camId = -1;
    for (const QString& param : params) {
        if (param.startsWith("iCamID=")) {
            camId = param.mid(7).toInt();
            break;
        }
    }

    if (camId < 0) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // 1. 检查0号屏是否在播放历史
    WindowStatus status = m_mainWindow->getWindowStatus(0);
    if (status.type == "playback") {
        // 如果在播放历史，先停止播放
        m_mainWindow->stopPlayback(0);
    }

    // 2. 切换窗口模式为1个窗口
    m_mainWindow->setSplitScreen(1, 1);

    // 3. 更新配置文件中的播放信息
    ConfigManager::getInstance()->setPlayId(camId);
    ConfigManager::getInstance()->setPlayType(0);  // 0表示实时视频

    // 4. 开始播放
    QString result;
    bool success = m_mainWindow->startCamera(0, camId, result);

    QString response = QString("<?xml version=\"1.0\"?><Message><Result>%1</Result></Message>")
                          .arg(success ? "1" : "0");
    sendXmlResponse(response);
}

// 2. 分组播放接口
void HttpRequestHandler::handleGroupSwitchCamera(const QStringList& params)
{
    int groupId = -1;
    for (const QString& param : params) {
        if (param.startsWith("iGroupID=")) {
            groupId = param.mid(9).toInt();
            break;
        }
    }

    if (groupId < 0) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // TODO: 调用主窗口的分组播放功能
    // m_mainWindow->startGroupCameras(groupId);

    sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>1</Result></Message>");
}

// 3. 局部放大接口
void HttpRequestHandler::handleCameraMax(const QStringList& params)
{
    int windowIndex = -1;
    for (const QString& param : params) {
        if (param.startsWith("iNO=")) {
            windowIndex = param.mid(4).toInt();
            break;
        }
    }

    if (windowIndex < 0) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // TODO: 调用主窗口的放大功能
    // m_mainWindow->maximizeWindow(windowIndex);

    sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>1</Result></Message>");
}

// 4. 获取分组接口
void HttpRequestHandler::handleGetGroupInfo()
{
    // TODO: 从主窗口获取分组信息
    QString xml = "<?xml version=\"1.0\"?>\n"
                 "<Message>\n"
                 "<GroupCount>2</GroupCount>\n"
                 "<GroupInfo>\n"
                 "    <GroupID>1</GroupID>\n"
                 "    <GroupName>分组1</GroupName>\n"
                 "    <ParentID></ParentID>\n"
                 "    <GroupNum>分组1</GroupNum>\n"
                 "</GroupInfo>\n"
                 "<GroupInfo>\n"
                 "    <GroupID>2</GroupID>\n"
                 "    <GroupName>分组2</GroupName>\n"
                 "    <ParentID>1</ParentID>\n"
                 "    <GroupNum>分组2</GroupNum>\n"
                 "</GroupInfo>\n"
                 "</Message>";

    sendXmlResponse(xml);
}

// 5. 云台控制接口
void HttpRequestHandler::handlePTZCtrlCamera(const QStringList& params)
{
    int ctrlType = -1;
    for (const QString& param : params) {
        if (param.startsWith("iCtrlType=")) {
            ctrlType = param.mid(10).toInt();
            break;
        }
    }

    if (ctrlType < 0) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // 使用 VideoManager 来控制云台
    bool success = m_videoManager->controlPTZ(static_cast<PTZCtrlType>(ctrlType));
    
    QString result = success ? "1" : "0";
    sendXmlResponse(QString("<?xml version=\"1.0\"?><Message><Result>%1</Result></Message>").arg(result));
}

// 6. 按时间播放历史接口
void HttpRequestHandler::handlePlayBackByTime(const QStringList& params)
{
    int camId = -1;
    QString startTime, endTime;

    for (const QString& param : params) {
        if (param.startsWith("iCamID=")) {
            camId = param.mid(7).toInt();
        }
        else if (param.startsWith("startTime=")) {
            startTime = param.mid(10);
        }
        else if (param.startsWith("endTime=")) {
            endTime = param.mid(8);
        }
    }

    if (camId < 0 || startTime.isEmpty() || endTime.isEmpty()) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    QDateTime start = parseDateTime(startTime);
    QDateTime end = parseDateTime(endTime);

    // TODO: 调用主窗口的历史回放功能
    // m_mainWindow->playBackByTime(camId, start, end);

    sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>1</Result></Message>");
}

// 7. 历史播放控制接口
void HttpRequestHandler::handlePlayBackCtrl(const QStringList& params)
{
    int ctrlType = -1;
    for (const QString& param : params) {
        if (param.startsWith("iCtrlType=")) {
            ctrlType = param.mid(10).toInt();
            break;
        }
    }

    if (ctrlType < 0) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // 使用 VideoManager 的枚举类型
    bool success = m_videoManager->controlPlayBack(static_cast<PlaybackCtrlType>(ctrlType));
    
    QString result = success ? "1" : "0";
    sendXmlResponse(QString("<?xml version=\"1.0\"?><Message><Result>%1</Result></Message>").arg(result));
}

// 8. 获取历史播放进度接口
void HttpRequestHandler::handleGetPlayBackPos()
{
    // TODO: 从主窗口获取播放进度
    int pos = 66; // 示例进度

    QString xml = QString("<?xml version=\"1.0\"?>\n"
                        "<Message>\n"
                        "<pos>%1</pos>\n"
                        "</Message>").arg(pos);

    sendXmlResponse(xml);
}

// 9. 拖放接口
void HttpRequestHandler::handleSetPlayBackPos(const QStringList& params)
{
    int pos = -1;
    QString playTime;

    for (const QString& param : params) {
        if (param.startsWith("iPos=")) {
            pos = param.mid(5).toInt();
        }
        else if (param.startsWith("playtime=")) {
            playTime = param.mid(9);
        }
    }

    if (!playTime.isEmpty()) {
        // 按时间定位
        QDateTime time = parseDateTime(playTime);
        // TODO: 调用主窗口的定位功能
        // m_mainWindow->seekPlayBackByTime(time);
    }
    else if (pos >= 0) {
        // 按进度定位
        // TODO: 调用主窗口的定位功能
        // m_mainWindow->seekPlayBack(pos);
    }
    else {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>1</Result></Message>");
}

// 11. 历史查询接口
void HttpRequestHandler::handleFindFileByTime(const QStringList& params)
{
    int camId = -1;
    QString startTime, endTime;

    for (const QString& param : params) {
        if (param.startsWith("iCamID=")) {
            camId = param.mid(7).toInt();
        }
        else if (param.startsWith("startTime=")) {
            startTime = param.mid(10);
        }
        else if (param.startsWith("endTime=")) {
            endTime = param.mid(8);
        }
    }

    if (camId < 0 || startTime.isEmpty() || endTime.isEmpty()) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // TODO: 从主窗口获取历史文件列表
    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                 "<Message>\n"
                 "<HisFileCount>2</HisFileCount>\n"
                 "<HisFile>\n"
                 "<FileName>ch0007_00010000330000100</FileName>\n"
                 "<FileStartTime>2018-6-4T9:26:51</FileStartTime>\n"
                 "<FileEndTime>2018-6-4T10:1:21</FileEndTime>\n"
                 "</HisFile>\n"
                 "<HisFile>\n"
                 "<FileName>ch0007_00010000323000000</FileName>\n"
                 "<FileStartTime>2018-6-4T10:1:21</FileStartTime>\n"
                 "<FileEndTime>2018-6-4T11:11:51</FileEndTime>\n"
                 "</HisFile>\n"
                 "</Message>";

    sendXmlResponse(xml);
}

// 12. 历史文件回放接口
void HttpRequestHandler::handlePlayBackByFile(const QStringList& params)
{
    int camId = -1;
    QString fileName;

    for (const QString& param : params) {
        if (param.startsWith("iCamID=")) {
            camId = param.mid(7).toInt();
        }
        else if (param.startsWith("fileName=")) {
            fileName = param.mid(9);
        }
    }

    if (camId < 0 || fileName.isEmpty()) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // TODO: 调用主窗口的文件回放功能
    // m_mainWindow->playBackByFile(camId, fileName);

    sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>1</Result></Message>");
}

// 13. 获取历史文件已经播放的时间
void HttpRequestHandler::handleGetPlayBackTime()
{
    // TODO: 从主窗口获取播放时间
    int playTime = 46;  // 示例播放时间
    int allTime = 2071; // 示例总时间

    QString xml = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<Message>\n"
                        "<playTime>%1</playTime>\n"
                        "<AllTime>%2</AllTime>\n"
                        "</Message>").arg(playTime).arg(allTime);

    sendXmlResponse(xml);
}

// 15. 获取组内相机列表接口
void HttpRequestHandler::handleGroupInfo(const QStringList& params)
{
    int groupId = -1;
    for (const QString& param : params) {
        if (param.startsWith("iGroupID=")) {
            groupId = param.mid(9).toInt();
            break;
        }
    }

    if (groupId < 0) {
        sendXmlResponse("<?xml version=\"1.0\"?><Message><Result>0</Result></Message>");
        return;
    }

    // TODO: 从主窗口获取组内相机列表
    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                 "<Message>\n"
                 "<GroupInfo>\n"
                 "<CamName>仓库内东北角</CamName>\n"
                 "<CamID>108</CamID>\n"
                 "</GroupInfo>\n"
                 "<GroupInfo>\n"
                 "<CamName>仓库内西南角</CamName>\n"
                 "<CamID>111</CamID>\n"
                 "</GroupInfo>\n"
                 "</Message>";

    sendXmlResponse(xml);
}

// 辅助函数：解析时间字符串
QDateTime HttpRequestHandler::parseDateTime(const QString& dateTimeStr)
{
    // 格式：2018-4-30T22:44:52
    return QDateTime::fromString(dateTimeStr, "yyyy-M-dTHH:mm:ss");
}

// ... 实现其他接口处理函数 ... 
