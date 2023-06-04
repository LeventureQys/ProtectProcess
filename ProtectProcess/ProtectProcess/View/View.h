#pragma once
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include "ui_View.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qdebug.h"
#include "qprocess.h"
#include "qtimer.h"
#include"qdir.h"
#include "qsettings.h"
#include "qcheckbox.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qfiledialog.h"
//就是 我知道 业务应该和界面分开，但是我 懒得分开了，嘻嘻

struct ProcessInfo {
	QFileInfo fileInfo;
	bool bln_resetable = false;
	qint32 id = 0;								//这个程序的ID
	QString info = "";							//这个程序的说明
};
class Process :public QObject{
	Q_OBJECT
public:
	Process();
	~Process();


	void setResetable(bool resetable);						//设置是否守护
	bool init(QString filePath,qint32 id);					//获取文件路径，同时获取一切信息
	bool init(QString filePath, qint32 id, QString info,bool resetable);
	bool setResetableTimer(bool timerOn);					//设置一个阻断，如果我们暂时不需要守护了，那么就把时钟停止就行

	ProcessInfo &getProcessInfo();
	void setProcessInfo(ProcessInfo &info);
	void setProcessInfo(QFileInfo& info);
	bool isEmpty();
	static Process& Empty();
signals:
	void Exception(qint32 id,const QString& string);
	void Info(qint32 id,const QString& program, const QString& string);
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
	bool bln_isPause = false;
};

class ProcessManager :public QObject {
	Q_OBJECT
public:
	ProcessManager();
	~ProcessManager();

	void init();	

	//ProtectProcess.ini 每个需要管理的进程都通过

	//curd
	qint32 AddProcess(QString Path);
	qint32 AddProcess(QString Path, QString info,bool blnResetable);

	void DeleteProcess(qint32 id);
	bool DeleteProcess(QString filePath);
	void SetProcess(qint32 id,QFileInfo fileInfo);		//修改指定ID对象的文件信息
	void SetProcess(ProcessInfo processInfo);

	ProcessInfo getProcess(qint32 id);					//根据指定ID获得文件信息
	ProcessInfo getProcess(QString filePath);

	QHash<qint32, ProcessInfo> getProcessInfo();		//获得所有进程信息

	void PauseAll(bool pauseAll);						//开始所有任务
	bool get_bln_init();
	qint32 getSize();
	bool exist(QString filePath);						//是否已经存在重复路径了
private slots:

	void sendException(const QString& string);
	void sendInfo(const QString& string);

private:
	bool bln_init = false;
	//QList<Process*> list_process;						//所有程序的列表
	QHash<qint32, Process*> list_process;				//所有进程的列表
	QHash<qint32, ProcessInfo> hash_info;				//获得信息与ID的哈希表,展示用的，并不是与实际上同步
	void createConfigFolderIfNeeded();
	void SaveSettingsFile();							//将当前的状态保存
	QString configIniFilepath = "";
signals:
	void Exception(qint32 id,const QString& strMessage);			//异常
	void Info(qint32 id,const QString& program,const QString& strMessage);				//通知
};



//info:用于作为界面化操作守护程序的类
class View : public QMainWindow
{
	Q_OBJECT

public:
	View(QWidget *parent = nullptr);
	~View();

	void initView();

	
private slots:
	void AddProcess_view(ProcessInfo pInfo);			//加入一个进程
	void DeleteProcess_view(qint32 id);				//删除一个进程
	void InitTableView();
	void on_btn_start_clicked();
	void on_btn_stop_clicked();

	void on_btn_save_clicked();
	void on_btn_add_clicked();
	void on_btn_del_clicked();
private:
	Ui::ViewClass ui;
	ProcessManager manager;					//按道理说是不应该把业务逻辑嵌入界面的，但是这个程序不大，所以这么做，理解一下
};
