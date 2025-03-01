#include "videowidget.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
    , videoLabel(nullptr)
    , indexLabel(nullptr)
    , windowIndex(-1)
{
    initUI();
    showNoSignal();
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::initUI()
{
    setStyleSheet("background-color: black;");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // 创建视频显示标签
    videoLabel = new QLabel(this);
    videoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(videoLabel);
    
    // 创建索引标签
    indexLabel = new QLabel(this);
    indexLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    indexLabel->setStyleSheet("QLabel { color: white; background-color: transparent; padding: 5px; }");
    indexLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    
    // 将索引标签覆盖在视频标签上
    indexLabel->setParent(this);
    indexLabel->raise();
}

void VideoWidget::setIndex(int index)
{
    windowIndex = index;
    indexLabel->setText(QString::number(index));
}

int VideoWidget::getIndex() const
{
    return windowIndex;
}

void VideoWidget::showNoSignal()
{
    videoLabel->setText("无信号");
    videoLabel->setStyleSheet("QLabel { color: white; background-color: black; }");
}

void VideoWidget::clear()
{
    videoLabel->clear();
}

void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(windowIndex);
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked(event->pos());
    }
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(windowIndex);
    }
} 