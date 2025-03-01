#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    void setIndex(int index);
    int getIndex() const;
    void showNoSignal();
    void clear();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void clicked(int index);
    void doubleClicked(int index);
    void rightClicked(const QPoint &pos);

private:
    void initUI();

private:
    QLabel *videoLabel;
    QLabel *indexLabel;
    int windowIndex;
};

#endif // VIDEOWIDGET_H 