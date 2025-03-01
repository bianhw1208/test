#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videowidget.h"
#include <QDebug>
#include "httpserver.h"
#include "videomanager.h"
#include "configmanager.h"
#include "databasemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentRows(2)
    , currentCols(2)
    , isFullScreen(false)
{
    ui->setupUi(this);
    initUI();
    setupMenus();
    setSplitScreen(2, 2); // 默认2x2分屏
    
    // 创建视频管理器
    m_videoManager = new VideoManager(this);
    
    // 创建并启动 HTTP 服务器
    m_httpServer = new HttpServer(this, m_videoManager, this);
    
    // 启动HTTP服务器
    int httpPort = ConfigManager::getInstance()->getHttpPort();
    if (!m_httpServer->start(httpPort)) {
        qDebug() << "Failed to start HTTP server on port" << httpPort;
    }
}

MainWindow::~MainWindow()
{
    clearVideoWidgets();
    delete ui;
}

void MainWindow::initUI()
{
    // 获取配置
    ConfigManager* config = ConfigManager::getInstance();
    
    // 初始化数据库连接
    DatabaseManager* db = DatabaseManager::getInstance();
    if (!db->connect(config->getDbIP(), config->getDbPort(), config->getDbName(), config->getDbUser(), config->getDbPassword())) {
        qDebug() << "Failed to connect to database:" << db->lastError();
        return;
    }
    
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setSpacing(1);
    gridLayout->setContentsMargins(1, 1, 1, 1);
    
    // 设置最小窗口大小
    setMinimumSize(800, 600);
}

void MainWindow::setupMenus()
{
    // 创建右键菜单
    contextMenu = new QMenu(this);
    splitScreenMenu = new QMenu("分屏模式", this);
    contextMenu->addMenu(splitScreenMenu);
    
    // 添加分屏选项
    QVector<QPair<QString, QPair<int, int>>> splitModes = {
        {"1分屏", {1, 1}},
        {"4分屏", {2, 2}},
        {"9分屏", {3, 3}},
        {"16分屏", {4, 4}},
        {"25分屏", {5, 5}}
    };
    
    for (const auto &mode : splitModes) {
        QAction *action = splitScreenMenu->addAction(mode.first);
        action->setData(QVariant::fromValue(mode.second));
        connect(action, &QAction::triggered, this, &MainWindow::handleSplitScreenAction);
    }
    
    // 添加全屏切换选项
    QAction *fullScreenAction = contextMenu->addAction("全屏");
    fullScreenAction->setCheckable(true);
    connect(fullScreenAction, &QAction::triggered, this, &MainWindow::handleFullScreenAction);
}

void MainWindow::setSplitScreen(int rows, int cols)
{
    if (rows <= 0 || cols <= 0 || rows > 5 || cols > 5) {
        return;
    }
    
    clearVideoWidgets();
    currentRows = rows;
    currentCols = cols;
    
    // 创建视频窗口
    int totalWindows = rows * cols;
    createVideoWidgets(totalWindows);
    
    // 布局视频窗口
    int index = 0;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (index < videoWidgets.size()) {
                gridLayout->addWidget(videoWidgets[index], row, col);
                videoWidgets[index]->setIndex(index + 1);
                index++;
            }
        }
    }
}

void MainWindow::createVideoWidgets(int count)
{
    for (int i = 0; i < count; ++i) {
        VideoWidget *widget = new VideoWidget(this);
        connect(widget, &VideoWidget::rightClicked, this, &MainWindow::createContextMenu);
        connect(widget, &VideoWidget::doubleClicked, [this](int) {
            setFullScreen(!isFullScreen);
        });
        videoWidgets.append(widget);
    }
}

void MainWindow::clearVideoWidgets()
{
    for (auto widget : videoWidgets) {
        gridLayout->removeWidget(widget);
        delete widget;
    }
    videoWidgets.clear();
    
    // 清理播放器资源
    for (auto player : rtspPlayers) {
        delete player;
    }
    rtspPlayers.clear();
    
    for (auto camera : hikCameras) {
        delete camera;
    }
    hikCameras.clear();
}

void MainWindow::createContextMenu(const QPoint &pos)
{
    contextMenu->exec(QCursor::pos());
}

void MainWindow::handleSplitScreenAction()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        QPair<int, int> mode = action->data().value<QPair<int, int>>();
        setSplitScreen(mode.first, mode.second);
    }
}

void MainWindow::handleFullScreenAction()
{
    setFullScreen(!isFullScreen);
}

void MainWindow::setFullScreen(bool fullscreen)
{
    isFullScreen = fullscreen;
    if (isFullScreen) {
        showFullScreen();
    } else {
        showNormal();
    }
}

void MainWindow::startRtspStream(const QString &rtspUrl, int windowIndex)
{
    if (windowIndex < 0 || windowIndex >= videoWidgets.size()) {
        return;
    }
    
    // 这里预留RTSP播放器的实现
    // RtspPlayer *player = new RtspPlayer(this);
    // player->startPlay(rtspUrl, videoWidgets[windowIndex]);
    // rtspPlayers.append(player);
}

void MainWindow::startHikCamera(const QString &cameraIP, const QString &username, 
                              const QString &password, int windowIndex)
{
    if (windowIndex < 0 || windowIndex >= videoWidgets.size()) {
        return;
    }
    
    // 这里预留海康SDK的实现
    // HikCamera *camera = new HikCamera(this);
    // camera->connect(cameraIP, username, password);
    // camera->startPreview(videoWidgets[windowIndex]);
    // hikCameras.append(camera);
}

void MainWindow::stopPlayback(int windowIndex)
{
    if (windowIndex < 0 || windowIndex >= videoWidgets.size()) {
        return;
    }
    
    // 停止播放并清理资源 TODO


    videoWidgets[windowIndex]->showNoSignal();
}

WindowStatus MainWindow::getWindowStatus(int index) const
{
    WindowStatus status;
    status.isPlaying = false;
    status.source = "";
    status.type = "";

    if (index >= 0 && index < videoWidgets.size()) {
        // TODO: 根据实际情况设置状态
        // 例如：检查对应的 hikCameras 和 rtspPlayers
    }

    return status;
}

QVector<WindowStatus> MainWindow::getAllWindowStatus() const
{
    QVector<WindowStatus> statuses;
    for (int i = 0; i < videoWidgets.size(); ++i) {
        statuses.append(getWindowStatus(i));
    }
    return statuses;
}

void MainWindow::getWindowLayout(int& rows, int& cols) const
{
    rows = currentRows;
    cols = currentCols;
}

bool MainWindow::startCamera(int indexNo, int camId, QString& result)
{
    if (indexNo < 0 || indexNo >= videoWidgets.size()) {
        result = "Invalid window index";
        return false;
    }

    // 先停止当前窗口的播放
    stopPlayback(indexNo);

    // 保存当前播放信息
    m_currentCamId = camId;
    
    try {
        // TODO: 实现实时视频播放
        // 1. 获取摄像机信息（IP、用户名、密码等）
        // 2. 创建播放器
        // 3. 连接摄像机
        // 4. 开始预览
        qDebug() << "Starting live camera:" << camId << "in window:" << indexNo;
        
        // 示例：使用海康SDK播放
        // HikCamera* camera = new HikCamera(this);
        // if (!camera->connect(cameraInfo.ip, cameraInfo.username, cameraInfo.password)) {
        //     result = "Failed to connect camera";
        //     return false;
        // }
        // if (!camera->startPreview(videoWidgets[indexNo])) {
        //     result = "Failed to start preview";
        //     return false;
        // }
        // hikCameras.append(camera);

        result = "Success";
        return true;
    }
    catch (const std::exception& e) {
        result = QString("Exception: %1").arg(e.what());
        return false;
    }
}

bool MainWindow::startPlayback(int indexNo, int camId, QString& result)
{
    if (indexNo < 0 || indexNo >= videoWidgets.size()) {
        result = "Invalid window index";
        return false;
    }

    // 先停止当前窗口的播放
    stopPlayback(indexNo);

    // 保存当前播放信息
    m_currentCamId = camId;
    
    // 更新配置文件中的播放信息
    ConfigManager::getInstance()->setPlayId(camId);
    ConfigManager::getInstance()->setPlayType(1);  // 1表示录像回放

    try {
        // TODO: 实现录像回放
        // 1. 获取摄像机信息
        // 2. 创建播放器
        // 3. 连接存储服务器
        // 4. 开始回放
        qDebug() << "Starting playback camera:" << camId << "in window:" << indexNo;

        result = "Success";
        return true;
    }
    catch (const std::exception& e) {
        result = QString("Exception: %1").arg(e.what());
        return false;
    }
}

void MainWindow::startGroupCameras(int groupId)
{
    m_currentGroupId = groupId;
    // TODO: 实现分组播放
    qDebug() << "Starting group:" << groupId;
}

void MainWindow::maximizeWindow(int windowIndex)
{
    // TODO: 实现窗口放大
    qDebug() << "Maximizing window:" << windowIndex;
}

void MainWindow::controlPTZ(PTZCtrlType ctrlType)
{
    // TODO: 实现云台控制
    qDebug() << "PTZ control:" << ctrlType;
}

void MainWindow::playBackByTime(int camId, const QDateTime& startTime, const QDateTime& endTime)
{
    m_currentCamId = camId;
    m_isPlayingBack = true;
    // TODO: 实现按时间回放
    qDebug() << "Playing back camera" << camId << "from" << startTime << "to" << endTime;
}

void MainWindow::playBackByFile(int camId, const QString& fileName)
{
    m_currentCamId = camId;
    m_isPlayingBack = true;
    // TODO: 实现按文件回放
    qDebug() << "Playing back file:" << fileName;
}

void MainWindow::controlPlayBack(PlaybackCtrlType ctrlType)
{
    if (!m_isPlayingBack) return;
    
    // TODO: 实现回放控制
    qDebug() << "Playback control:" << ctrlType;
}

void MainWindow::seekPlayBack(int pos)
{
    if (!m_isPlayingBack) return;
    
    m_playBackPos = pos;
    // TODO: 实现进度定位
    qDebug() << "Seeking to position:" << pos;
}

void MainWindow::seekPlayBackByTime(const QDateTime& time)
{
    if (!m_isPlayingBack) return;
    
    // TODO: 实现时间定位
    qDebug() << "Seeking to time:" << time;
}

int MainWindow::getPlayBackPos() const
{
    return m_playBackPos;
}

void MainWindow::getPlayBackTime(int& playTime, int& allTime) const
{
    playTime = m_playBackTime;
    allTime = m_playBackTotalTime;
}

QVector<QPair<QString, int>> MainWindow::getGroupCameras(int groupId) const
{
    // TODO: 实现获取分组内的摄像头列表
    QVector<QPair<QString, int>> cameras;
    cameras.append({"仓库内东北角", 108});
    cameras.append({"仓库内西南角", 111});
    return cameras;
}

QVector<QPair<QString, QPair<QDateTime, QDateTime>>> MainWindow::findHistoryFiles(
    int camId, const QDateTime& startTime, const QDateTime& endTime) const
{
    // TODO: 实现历史文件查询
    QVector<QPair<QString, QPair<QDateTime, QDateTime>>> files;
    files.append({"ch0007_00010000330000100", 
        {QDateTime::fromString("2018-6-4T9:26:51", "yyyy-M-dTHH:mm:ss"),
         QDateTime::fromString("2018-6-4T10:1:21", "yyyy-M-dTHH:mm:ss")}});
    return files;
}

