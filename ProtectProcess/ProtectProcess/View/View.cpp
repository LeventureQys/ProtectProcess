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
	//文件不一定要能打开
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

	if (this->process.waitForStarted(30000)) {//等待启动并进入
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
		return;				//如果没有初始化，则直接退出
	}
	
	this->pInfo.bln_resetable = resetable;
	if (this->pInfo.bln_resetable) {
		//需要做一个检查，如果已经进入了启动状态则不需要再重新开启检查
		if (this->timer.isActive()) {
			this->sendInfo("Timer Already Start,Dont Start Again");
			return;
		}

		this->timer.start(this->CheckInterval);
		

		connect(&this->timer, &QTimer::timeout, [=]() {		//事件绑定
			QProcess::ProcessState state = process.state();
			emit ProcessState(state);
			if (state != QProcess::ProcessState::Running && state != QProcess::ProcessState::Starting) {	//既没有在运行也没有在启动
				if (!openProcess()) {				//检查，如果程序没有运行，则让它自己启动
					sendInfo("OpenProgram Failed");
				}
			}
		});		
	}
	else {
		this->timer.stop();					//停止计时器
		this->timer.disconnect();			//断开事件
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
	QDir dir("."); // "." 表示程序当前文件夹
	QString configFolderName = "config"; // 配置文件夹名称

	if (!dir.exists(configFolderName)) { // 检查文件夹是否存在
		QDir().mkdir(configFolderName); // 创建文件夹
	}

	QString configFile = QString("%1/%2/ProtectProcess.ini").arg(QCoreApplication::applicationDirPath()).arg(configFolderName);
	QFile file(configFile);

	if (!file.exists()) { // 检查文件是否存在
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
