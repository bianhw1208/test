#ifndef VIDEOMANAGER_H
#define VIDEOMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QPair>

// 云台控制类型
enum PTZCtrlType {
    TILT_UP = 21,      // 云台上仰
    TILT_DOWN = 22,    // 云台下俯
    PAN_LEFT = 23,     // 云台左转
    PAN_RIGHT = 24,    // 云台右转
    UP_LEFT = 25,      // 云台上仰和左转
    UP_RIGHT = 26,     // 云台上仰和右转
    DOWN_LEFT = 27,    // 云台下俯和左转
    DOWN_RIGHT = 28,   // 云台下俯和右转
    PTZ_STOP = 100     // 云台控制停止
};

// 回放控制类型
enum PlaybackCtrlType {
    NET_DVR_PLAYPAUSE = 3,    // 暂停播放
    NET_DVR_PLAYRESTART = 4,  // 恢复播放
    NET_DVR_PLAYFAST = 5,     // 快放
    NET_DVR_PLAYSLOW = 6,     // 慢放
    NET_DVR_PLAYNORMAL = 7    // 正常速度播放
};

class VideoManager : public QObject
{
    Q_OBJECT
public:
    explicit VideoManager(QObject *parent = nullptr);

    // 实时播放相关
    bool startCamera(int camId);
    bool startGroupCameras(int groupId);
    
    // 云台控制相关
    bool controlPTZ(PTZCtrlType ctrlType);
    
    // 历史回放相关
    bool playBackByTime(int camId, const QDateTime& startTime, const QDateTime& endTime);
    bool playBackByFile(int camId, const QString& fileName);
    bool controlPlayBack(PlaybackCtrlType ctrlType);
    bool seekPlayBack(int pos);
    bool seekPlayBackByTime(const QDateTime& time);
    
    // 状态查询相关
    int getPlayBackPos() const;
    void getPlayBackTime(int& playTime, int& allTime) const;
    QVector<QPair<QString, int>> getGroupCameras(int groupId) const;
    QVector<QPair<QString, QPair<QDateTime, QDateTime>>> findHistoryFiles(
        int camId, const QDateTime& startTime, const QDateTime& endTime) const;

signals:
    void playbackStatusChanged(bool isPlaying);
    void playbackPositionChanged(int pos);
    void playbackTimeChanged(int currentTime, int totalTime);

private:
    int m_currentCamId;
    int m_currentGroupId;
    bool m_isPlayingBack;
    int m_playBackPos;
    int m_playBackTime;
    int m_playBackTotalTime;
};

#endif // VIDEOMANAGER_H 