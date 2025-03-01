#include "videomanager.h"
#include <QDebug>

VideoManager::VideoManager(QObject *parent)
    : QObject(parent)
    , m_currentCamId(-1)
    , m_currentGroupId(-1)
    , m_isPlayingBack(false)
    , m_playBackPos(0)
    , m_playBackTime(0)
    , m_playBackTotalTime(0)
{
}

bool VideoManager::startCamera(int camId)
{
    m_currentCamId = camId;
    // TODO: 实现摄像头播放
    qDebug() << "Starting camera:" << camId;
    return true;
}

bool VideoManager::startGroupCameras(int groupId)
{
    m_currentGroupId = groupId;
    // TODO: 实现分组播放
    qDebug() << "Starting group:" << groupId;
    return true;
}

bool VideoManager::controlPTZ(PTZCtrlType ctrlType)
{
    if (m_currentCamId < 0) return false;
    
    // TODO: 实现云台控制
    qDebug() << "PTZ control:" << ctrlType;
    return true;
}

bool VideoManager::playBackByTime(int camId, const QDateTime& startTime, const QDateTime& endTime)
{
    m_currentCamId = camId;
    m_isPlayingBack = true;
    // TODO: 实现按时间回放
    qDebug() << "Playing back camera" << camId << "from" << startTime << "to" << endTime;
    emit playbackStatusChanged(true);
    return true;
}

bool VideoManager::playBackByFile(int camId, const QString& fileName)
{
    m_currentCamId = camId;
    m_isPlayingBack = true;
    // TODO: 实现按文件回放
    qDebug() << "Playing back file:" << fileName;
    emit playbackStatusChanged(true);
    return true;
}

bool VideoManager::controlPlayBack(PlaybackCtrlType ctrlType)
{
    if (!m_isPlayingBack) return false;
    
    // TODO: 实现回放控制
    qDebug() << "Playback control:" << ctrlType;
    return true;
}

bool VideoManager::seekPlayBack(int pos)
{
    if (!m_isPlayingBack) return false;
    
    m_playBackPos = pos;
    // TODO: 实现进度定位
    qDebug() << "Seeking to position:" << pos;
    emit playbackPositionChanged(pos);
    return true;
}

bool VideoManager::seekPlayBackByTime(const QDateTime& time)
{
    if (!m_isPlayingBack) return false;
    
    // TODO: 实现时间定位
    qDebug() << "Seeking to time:" << time;
    return true;
}

int VideoManager::getPlayBackPos() const
{
    return m_playBackPos;
}

void VideoManager::getPlayBackTime(int& playTime, int& allTime) const
{
    playTime = m_playBackTime;
    allTime = m_playBackTotalTime;
}

QVector<QPair<QString, int>> VideoManager::getGroupCameras(int groupId) const
{
    // TODO: 实现获取分组内的摄像头列表
    QVector<QPair<QString, int>> cameras;
    cameras.append({"仓库内东北角", 108});
    cameras.append({"仓库内西南角", 111});
    return cameras;
}

QVector<QPair<QString, QPair<QDateTime, QDateTime>>> VideoManager::findHistoryFiles(
    int camId, const QDateTime& startTime, const QDateTime& endTime) const
{
    // TODO: 实现历史文件查询
    QVector<QPair<QString, QPair<QDateTime, QDateTime>>> files;
    files.append({"ch0007_00010000330000100", 
        {QDateTime::fromString("2018-6-4T9:26:51", "yyyy-M-dTHH:mm:ss"),
         QDateTime::fromString("2018-6-4T10:1:21", "yyyy-M-dTHH:mm:ss")}});
    return files;
} 