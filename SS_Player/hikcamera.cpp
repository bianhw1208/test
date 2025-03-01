#include "hikcamera.h"
#include "videowidget.h"
#include <QDebug>

HikCamera::HikCamera(QObject *parent)
    : QObject(parent)
    , m_userId(-1)
    , m_realHandle(-1)
    , m_displayWidget(nullptr)
    , m_isConnected(false)
    , m_isPreviewing(false)
{
}

HikCamera::~HikCamera()
{
    stopPreview();
    disconnect();
}

bool HikCamera::connect(const QString& ip, const QString& username, const QString& password)
{
    if (m_isConnected) {
        disconnect();
    }

    // TODO: 实现海康SDK的设备连接
    // NET_DVR_Login_V30() 等接口调用
    m_userId = -1;  // 实际应该是登录返回的用户ID
    
    qDebug() << "Connecting to camera:" << ip;
    m_isConnected = true;
    return true;
}

void HikCamera::disconnect()
{
    if (!m_isConnected) {
        return;
    }

    stopPreview();

    // TODO: 实现设备断开连接
    // NET_DVR_Logout() 等接口调用
    
    m_userId = -1;
    m_isConnected = false;
}

bool HikCamera::startPreview(VideoWidget* displayWidget)
{
    if (!m_isConnected || m_isPreviewing || !displayWidget) {
        return false;
    }

    m_displayWidget = displayWidget;

    // TODO: 实现预览功能
    // NET_DVR_RealPlay_V40() 等接口调用
    m_realHandle = -1;  // 实际应该是预览接口返回的句柄
    
    m_isPreviewing = true;
    return true;
}

void HikCamera::stopPreview()
{
    if (!m_isPreviewing) {
        return;
    }

    // TODO: 停止预览
    // NET_DVR_StopRealPlay() 等接口调用
    
    m_realHandle = -1;
    m_isPreviewing = false;
    if (m_displayWidget) {
        m_displayWidget->showNoSignal();
        m_displayWidget = nullptr;
    }
}

bool HikCamera::isConnected() const
{
    return m_isConnected;
}

bool HikCamera::isPreviewing() const
{
    return m_isPreviewing;
} 