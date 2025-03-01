#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QVector>
#include <QMenu>
#include <QAction>
#include "httpserver.h"
#include "videomanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 前向声明
class VideoWidget;
class HikCamera;  // 海康相机类
class RtspPlayer; // RTSP播放器类

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // 分屏相关
    void setSplitScreen(int rows, int cols);
    void setFullScreen(bool fullscreen);
    
    // 播放控制
    void startRtspStream(const QString& rtspUrl, int windowIndex);
    void startHikCamera(const QString& cameraIP, const QString& username, 
                       const QString& password, int windowIndex);
    void stopPlayback(int windowIndex);

    // 获取窗口状态
    WindowStatus getWindowStatus(int index) const;
    QVector<WindowStatus> getAllWindowStatus() const;
    // 获取窗口布局
    void getWindowLayout(int& rows, int& cols) const;

    // 实时播放相关
    bool startCamera(int indexNo, int camId, QString& result);
    void startGroupCameras(int groupId);
    void maximizeWindow(int windowIndex);
    
    // 云台控制相关
    void controlPTZ(PTZCtrlType ctrlType);
    
    // 历史回放相关
    void playBackByTime(int camId, const QDateTime& startTime, const QDateTime& endTime);
    void playBackByFile(int camId, const QString& fileName);
    void controlPlayBack(PlaybackCtrlType ctrlType);
    void seekPlayBack(int pos);
    void seekPlayBackByTime(const QDateTime& time);
    
    // 状态查询相关
    int getPlayBackPos() const;
    void getPlayBackTime(int& playTime, int& allTime) const;
    QVector<QPair<QString, int>> getGroupCameras(int groupId) const;
    QVector<QPair<QString, QPair<QDateTime, QDateTime>>> findHistoryFiles(
        int camId, const QDateTime& startTime, const QDateTime& endTime) const;

    // 新增录像回放方法
    bool startPlayback(int indexNo, int camId, QString& result);

private slots:
    void createContextMenu(const QPoint& pos);
    void handleSplitScreenAction();
    void handleFullScreenAction();

private:
    void initUI();
    void setupMenus();
    void createVideoWidgets(int count);
    void clearVideoWidgets();
    
private:
    Ui::MainWindow *ui;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QVector<VideoWidget*> videoWidgets;
    QVector<HikCamera*> hikCameras;
    QVector<RtspPlayer*> rtspPlayers;
    
    int currentRows;
    int currentCols;
    bool isFullScreen;
    
    // 右键菜单
    QMenu *contextMenu;
    QMenu *splitScreenMenu;
    
    HttpServer* m_httpServer;

    // 添加成员变量
    int m_currentCamId;
    int m_currentGroupId;
    bool m_isPlayingBack;
    int m_playBackPos;
    int m_playBackTime;
    int m_playBackTotalTime;
    VideoManager* m_videoManager;
};

#endif // MAINWINDOW_H
