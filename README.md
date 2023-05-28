7# QApplicationFrame
Qt开发应用的框架，包括QDump、进程等

主要内容包含一个正式注册了的QDumper，重写了的QApplication，包括单例实例，还包括日志系统

提供了后台检查进程的功能，当这个进程不存在的时候会尝试启动

需要守护的进程都写在配置文件中9