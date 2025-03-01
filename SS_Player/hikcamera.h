#ifndef HIKCAMERA_H
#define HIKCAMERA_H

#include <QObject>
#include <QString>

class VideoWidget;

class HikCamera : public QObject
{
    Q_OBJECT
public:
    explicit HikCamera(QObject *parent = nullptr);
    ~HikCamera();

    // 连接相机
    bool connect(const QString& ip, const QString& username, const QString& password);
    // 断开连接
    void disconnect();
    // 开始预览
    bool startPreview(VideoWidget* displayWidget);
    // 停止预览
    void stopPreview();
    // 是否已连接
    bool isConnected() const;
    // 是否正在预览
    bool isPreviewing() const;

private:
    // 海康SDK相关变量
    long m_userId;     // 用户ID
    long m_realHandle; // 预览句柄
    
    VideoWidget* m_displayWidget;
    bool m_isConnected;
    bool m_isPreviewing;
};

#endif // HIKCAMERA_H 