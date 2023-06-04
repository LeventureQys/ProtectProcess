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
//���� ��֪�� ҵ��Ӧ�úͽ���ֿ��������� ���÷ֿ��ˣ�����

struct ProcessInfo {
	QFileInfo fileInfo;
	bool bln_resetable = false;
	qint32 id = 0;								//��������ID
	QString info = "";							//��������˵��
};
class Process :public QObject{
	Q_OBJECT
public:
	Process();
	~Process();


	void setResetable(bool resetable);						//�����Ƿ��ػ�
	bool init(QString filePath,qint32 id);					//��ȡ�ļ�·����ͬʱ��ȡһ����Ϣ
	bool init(QString filePath, qint32 id, QString info,bool resetable);
	bool setResetableTimer(bool timerOn);					//����һ����ϣ����������ʱ����Ҫ�ػ��ˣ���ô�Ͱ�ʱ��ֹͣ����

	ProcessInfo &getProcessInfo();
	void setProcessInfo(ProcessInfo &info);
	void setProcessInfo(QFileInfo& info);
	bool isEmpty();
	static Process& Empty();
signals:
	void Exception(qint32 id,const QString& string);
	void Info(qint32 id,const QString& program, const QString& string);
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
	bool bln_isPause = false;
};

class ProcessManager :public QObject {
	Q_OBJECT
public:
	ProcessManager();
	~ProcessManager();

	void init();	

	//ProtectProcess.ini ÿ����Ҫ����Ľ��̶�ͨ��

	//curd
	qint32 AddProcess(QString Path);
	qint32 AddProcess(QString Path, QString info,bool blnResetable);

	void DeleteProcess(qint32 id);
	bool DeleteProcess(QString filePath);
	void SetProcess(qint32 id,QFileInfo fileInfo);		//�޸�ָ��ID������ļ���Ϣ
	void SetProcess(ProcessInfo processInfo);

	ProcessInfo getProcess(qint32 id);					//����ָ��ID����ļ���Ϣ
	ProcessInfo getProcess(QString filePath);

	QHash<qint32, ProcessInfo> getProcessInfo();		//������н�����Ϣ

	void PauseAll(bool pauseAll);						//��ʼ��������
	bool get_bln_init();
	qint32 getSize();
	bool exist(QString filePath);						//�Ƿ��Ѿ������ظ�·����
private slots:

	void sendException(const QString& string);
	void sendInfo(const QString& string);

private:
	bool bln_init = false;
	//QList<Process*> list_process;						//���г�����б�
	QHash<qint32, Process*> list_process;				//���н��̵��б�
	QHash<qint32, ProcessInfo> hash_info;				//�����Ϣ��ID�Ĺ�ϣ��,չʾ�õģ���������ʵ����ͬ��
	void createConfigFolderIfNeeded();
	void SaveSettingsFile();							//����ǰ��״̬����
	QString configIniFilepath = "";
signals:
	void Exception(qint32 id,const QString& strMessage);			//�쳣
	void Info(qint32 id,const QString& program,const QString& strMessage);				//֪ͨ
};



//info:������Ϊ���滯�����ػ��������
class View : public QMainWindow
{
	Q_OBJECT

public:
	View(QWidget *parent = nullptr);
	~View();

	void initView();

	
private slots:
	void AddProcess_view(ProcessInfo pInfo);			//����һ������
	void DeleteProcess_view(qint32 id);				//ɾ��һ������
	void InitTableView();
	void on_btn_start_clicked();
	void on_btn_stop_clicked();

	void on_btn_save_clicked();
	void on_btn_add_clicked();
	void on_btn_del_clicked();
private:
	Ui::ViewClass ui;
	ProcessManager manager;					//������˵�ǲ�Ӧ�ð�ҵ���߼�Ƕ�����ģ�����������򲻴�������ô�������һ��
};
