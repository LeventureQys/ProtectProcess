#pragma once

#include <QMainWindow>
#include "ui_View.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qdebug.h"
#include "qprocess.h"
#include "qtimer.h"
#include"qdir.h"
#include "qsettings.h"
struct ProcessInfo {
	QFileInfo fileInfo;
	bool bln_resetable = false;
	qint32 id = 0;								//这个程序的ID
};
class Process :public QObject{
	Q_OBJECT
public:
	Process();
	~Process();


	void setResetable(bool resetable);						//设置是否守护
	bool init(QString filePath,qint32 id);					//获取文件路径，同时获取一切信息
	
	ProcessInfo &getProcessInfo();
	void setProcessInfo(ProcessInfo &info);
signals:
	void Exception(const QString& string);
	void Info(const QString& program, const QString& string);
	void ProcessState(QProcess::ProcessState state);	//发送当前进程状态给外部看
private slots:

	void sendException(const QString& string);
	void sendInfo(const QString& string);
	bool openProcess();									//启动进程



private:
	ProcessInfo pInfo;
	bool bln_init = false;
	QFile file;
	QProcess process;
	QTimer timer;										//轮询检查进程计时器
	qint32 CheckInterval = 5000;						//多久检查一次进程是否存在,ms
	QString Process_info;
	qint32 id = 0;										//这个程序的ID
};

class ProcessManager :public QObject {
	Q_OBJECT
public:
	ProcessManager();
	~ProcessManager();

	void init();	

	

	//curd
	void AddProcess(QString Path);
	void DeleteProcess(qint32 id);
	void SetProcess(qint32 id,QFileInfo fileInfo);		//修改指定ID对象的文件信息
	ProcessInfo getProcess(qint32 id);					//根据指定ID获得文件信息

	QHash<qint32, ProcessInfo> getProcessInfo();		//获得所有进程信息

private:
	bool bln_init = false;
	QList<Process*> list_process;						//所有程序的列表
	QHash<qint32, ProcessInfo> hash_info;				//获得信息与ID的哈希表
	QSettings settings;
	void createConfigFolderIfNeeded();

};
//info:用于作为界面化操作守护程序的类
class View : public QMainWindow
{
	Q_OBJECT

public:
	View(QWidget *parent = nullptr);
	~View();

private:
	Ui::ViewClass ui;
};
