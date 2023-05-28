#include "View.h"

View::View(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

View::~View()
{}

Process::Process()
{
}

Process::~Process()
{
}

bool Process::init(QString filePath, qint32 id)
{
	this->file.setFileName(filePath);
	//�ļ���һ��Ҫ�ܴ�
	if (!file.exists()) {
		qDebug() << "file not exist!";
		return false;
	}
		this->pInfo.fileInfo.setFile(this->file);

	this->bln_init = true;
	this->pInfo.id = id;
	return true;
}

ProcessInfo &Process::getProcessInfo()
{
	return this->pInfo;
}

void Process::setProcessInfo(ProcessInfo& info)
{
	this->pInfo = info;
}

void Process::sendInfo(const QString& string)
{
	emit this->Info(this->pInfo.fileInfo.fileName(), string);
}

bool Process::openProcess()
{
	if (!this->bln_init) {
		this->sendException("process not init yet");
		return false;
	}

	this->process.setProgram(this->pInfo.fileInfo.absolutePath());
	this->process.start();

	if (this->process.waitForStarted(30000)) {//�ȴ�����������
		return true;
	}
	else {
		return false;
	}
	
	
}
void Process::setResetable(bool resetable)
{
	if (!this->bln_init) {
		this->sendInfo("Init befor set resetable");
		return;				//���û�г�ʼ������ֱ���˳�
	}
	
	this->pInfo.bln_resetable = resetable;
	if (this->pInfo.bln_resetable) {
		//��Ҫ��һ����飬����Ѿ�����������״̬����Ҫ�����¿������
		if (this->timer.isActive()) {
			this->sendInfo("Timer Already Start,Dont Start Again");
			return;
		}

		this->timer.start(this->CheckInterval);
		

		connect(&this->timer, &QTimer::timeout, [=]() {		//�¼���
			QProcess::ProcessState state = process.state();
			emit ProcessState(state);
			if (state != QProcess::ProcessState::Running && state != QProcess::ProcessState::Starting) {	//��û��������Ҳû��������
				if (!openProcess()) {				//��飬�������û�����У��������Լ�����
					sendInfo("OpenProgram Failed");
				}
			}
		});		
	}
	else {
		this->timer.stop();					//ֹͣ��ʱ��
		this->timer.disconnect();			//�Ͽ��¼�
	}
}
void Process::sendException(const QString& string) {
	emit Exception(this->pInfo.fileInfo.absolutePath() + string);
	return;
}

ProcessManager::~ProcessManager()
{
}

void ProcessManager::init()
{
	this->createConfigFolderIfNeeded();

}

void ProcessManager::AddProcess(QString Path)
{
	Process* process = new Process();
	process->init(Path, this->list_process.size());
}

void ProcessManager::DeleteProcess(qint32 id)
{
	this->hash_info.remove(id);
}

void ProcessManager::SetProcess(qint32 id, QFileInfo fileInfo)
{

}




QHash<qint32, ProcessInfo> ProcessManager::getProcessInfo()
{
	return this->hash_info;
}

void ProcessManager::createConfigFolderIfNeeded()
{
	QDir dir("."); // "." ��ʾ����ǰ�ļ���
	QString configFolderName = "config"; // �����ļ�������

	if (!dir.exists(configFolderName)) { // ����ļ����Ƿ����
		QDir().mkdir(configFolderName); // �����ļ���
	}

	QString configFile = QString("%1/%2/ProtectProcess.ini").arg(QCoreApplication::applicationDirPath()).arg(configFolderName);
	QFile file(configFile);

	if (!file.exists()) { // ����ļ��Ƿ����
		qDebug() << "1.";
		return;
	}

	this->settings.setPath(QSettings::IniFormat, QSettings::UserScope, configFile);

	QList<QString> list_string = this->settings.allKeys();
	for (auto key : list_string) {
		Process* process = new Process();
		process->init(this->settings.value(key).toString(), this->list_process.size());
		this->list_process.append(process);
	}

}
