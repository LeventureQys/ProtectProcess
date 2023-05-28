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
	qint32 id = 0;								//��������ID
};
class Process :public QObject{
	Q_OBJECT
public:
	Process();
	~Process();


	void setResetable(bool resetable);						//�����Ƿ��ػ�
	bool init(QString filePath,qint32 id);					//��ȡ�ļ�·����ͬʱ��ȡһ����Ϣ
	
	ProcessInfo &getProcessInfo();
	void setProcessInfo(ProcessInfo &info);
signals:
	void Exception(const QString& string);
	void Info(const QString& program, const QString& string);
	void ProcessState(QProcess::ProcessState state);	//���͵�ǰ����״̬���ⲿ��
private slots:

	void sendException(const QString& string);
	void sendInfo(const QString& string);
	bool openProcess();									//��������



private:
	ProcessInfo pInfo;
	bool bln_init = false;
	QFile file;
	QProcess process;
	QTimer timer;										//��ѯ�����̼�ʱ��
	qint32 CheckInterval = 5000;						//��ü��һ�ν����Ƿ����,ms
	QString Process_info;
	qint32 id = 0;										//��������ID
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
	void SetProcess(qint32 id,QFileInfo fileInfo);		//�޸�ָ��ID������ļ���Ϣ
	ProcessInfo getProcess(qint32 id);					//����ָ��ID����ļ���Ϣ

	QHash<qint32, ProcessInfo> getProcessInfo();		//������н�����Ϣ

private:
	bool bln_init = false;
	QList<Process*> list_process;						//���г�����б�
	QHash<qint32, ProcessInfo> hash_info;				//�����Ϣ��ID�Ĺ�ϣ��
	QSettings settings;
	void createConfigFolderIfNeeded();

};
//info:������Ϊ���滯�����ػ��������
class View : public QMainWindow
{
	Q_OBJECT

public:
	View(QWidget *parent = nullptr);
	~View();

private:
	Ui::ViewClass ui;
};
