#include <QApplication>
#include <QDir>
#include "mainwindow.h"

/*
MainWindow 负责 UI
VideoManager 负责视频功能
HttpServer/HttpRequestHandler 负责网络通信
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置插件路径
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    
    MainWindow w;
    w.show();
    return a.exec();
}
