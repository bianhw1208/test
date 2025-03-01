#ifndef RTSPPLAYER_H
#define RTSPPLAYER_H

#include <QObject>
#include <QString>
#include <memory>

class VideoWidget;

class RtspPlayer : public QObject
{
    Q_OBJECT
public:
    explicit RtspPlayer(QObject *parent = nullptr);
    ~RtspPlayer();

    // 开始播放RTSP流
    bool startPlay(const QString& rtspUrl, VideoWidget* displayWidget);
    // 停止播放
    void stopPlay();
    // 是否正在播放
    bool isPlaying() const;

private:
    class Private;
    std::unique_ptr<Private> d;
    
    VideoWidget* m_displayWidget;
    bool m_isPlaying;
};

#endif // RTSPPLAYER_H 