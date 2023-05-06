#include <QtCore/QCoreApplication>
#include "../Main/MainApplication.h"
#include "../Tools/Tools_South.h"
#include "../QDumper/LgQDumper.h"
#include "qdebug.h"
#include <QApplication>
#include <QSharedMemory>
//Author:Leventure
//Date:2023-4-23
//Info:Qt程序的基础框架，用来Qt开发应用的框架，包括QDump、进程等
//主要内容包含一个正式注册了的QDumper，重写了的QApplication，包括单例实例，还包括日志系统
//需要使用Application作为最基础的底层进程框架，因为不知道将来会不会使用到窗体消息，当然了也可以直接换成QCoreApplication，按需要来进行即可
// 还需要提供一个调试模式处理器，通过https://github.com/LeventureQys/Leventure_DevelopKey开发者调试工具进入
//编译器:MSVC2017_32
//包含模块:core;gui;widgets
int main(int argc, char *argv[])
{
    MainApplication app(argc, argv);

        //创建一个共享内存，如果共享内存被占用，则该程序不在启动，当今当程序消失之后归还该共享内存
    // Use shared memory to check if app is already running

    QSharedMemory sharedMem("myUniqueSharedMemoryName");
    if (sharedMem.attach()) {
        // The other instance of the app is already running
        return 0;
    }
    sharedMem.create(1); // create a small segment of shared memory

    qDebug() << "当前使用的是Windows系统";
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);



    return app.exec();
}
