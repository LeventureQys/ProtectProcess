#include "View.h"

View::View(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

View::~View()
{}

void View::initView()
{
	this->show();
	this->manager.init();
	this->ui.tableWidget->setColumnCount(4);
	this->ui.tableWidget->setColumnWidth(0, 50);		//id
	this->ui.tableWidget->setColumnWidth(1, 150);		//是否守护
	this->ui.tableWidget->setColumnWidth(2, 350);		//路径
	this->ui.tableWidget->setColumnWidth(3, 150);		//说明

	QStringList titleList;
	titleList << "ID" << "是否守护" << "路径" << "说明";

	this->ui.tableWidget->setHorizontalHeaderLabels(titleList);

}

void View::AddProcess_view(ProcessInfo pInfo)
{
	QTableWidgetItem* id = new QTableWidgetItem(pInfo.id);
	QTableWidgetItem* filePath = new QTableWidgetItem(pInfo.fileInfo.absolutePath());
	QTableWidgetItem* Info = new QTableWidgetItem(pInfo.info);

	

	QCheckBox* checkBox = new QCheckBox();
	checkBox->setChecked(pInfo.bln_resetable);
	this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount(), 0, id);
	//this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount(), 2, id);
	this->ui.tableWidget->setCellWidget(this->ui.tableWidget->rowCount() - 1, 1, checkBox);
	this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount(), 2, filePath);
	this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount(), 3, Info);


}

void View::DeleteProcess_view(qint32 id)
{
	for (int i = 0; i < this->ui.tableWidget->rowCount(); ++i) {
		QTableWidgetItem* item = this->ui.tableWidget->item(i, 0);	//第i行第1个元素
		qint32 id = item->text().toInt();

		this->manager.DeleteProcess(id);
		this->ui.tableWidget->removeRow(i);

		return;
	}
}

void View::on_btn_start_clicked()
{
	this->manager.PauseAll(false);
}

void View::on_btn_stop_clicked()
{
	this->manager.PauseAll(true);
}

void View::on_btn_save_clicked()
{
	for (int i = 0; i < this->ui.tableWidget->rowCount(); ++i) {
		QTableWidgetItem* id = this->ui.tableWidget->item(i, 0);	//第i行第1个元素
		QWidget* cBox = this->ui.tableWidget->cellWidget(i, 1);
		QTableWidgetItem* filePath = this->ui.tableWidget->item(i, 2);
		QTableWidgetItem* Info = this->ui.tableWidget->item(i, 3);
		if (id == nullptr || cBox == nullptr || filePath == nullptr || Info == nullptr) {
			continue;
		}
		if (filePath->text().isEmpty()) {			//没有存文件路径的直接删除这一行 
			this->ui.tableWidget->removeRow(i);
			return;
		}
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(cBox);

		ProcessInfo processInfo;
		processInfo.id = id->text().toInt();
		processInfo.bln_resetable = checkBox->isChecked();
		processInfo.fileInfo = QFileInfo(filePath->text());
		processInfo.info = Info->text();

		this->manager.SetProcess(processInfo.id, processInfo);

	}
}

void View::on_btn_add_clicked()
{
	this->ui.tableWidget->insertRow(this->ui.tableWidget->rowCount());
	QCheckBox* checkBox = new QCheckBox();
	this->ui.tableWidget->setCellWidget(this->ui.tableWidget->rowCount() - 1, 1, checkBox);
}

void View::on_btn_del_clicked()
{
	// 获取当前选中的行号（可能有多行选中）
	QList<QTableWidgetItem*> items = ui.tableWidget->selectedItems();
	QSet<int> rows;
	for(QTableWidgetItem * item: items) {
		rows.insert(item->row());
	}

	// 逆序删除所有选中的行
	for(int row: rows.toList()) {
		ui.tableWidget->removeRow(row);
	}
}


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

bool Process::setResetableTimer(bool timerOn)
{
	if (timerOn) {
		//启动时钟
		if (!this->bln_isPause) {
			this->timer.start(5000);
			return true;
		}
		else {
			return false;	//如果已经启动了时钟则不再启动时钟
		}
	}
	else {
		this->timer.stop();
		return true;
	}
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

void Process::setProcessInfo(QFileInfo& info)
{
	ProcessInfo pInfo;
	pInfo.bln_resetable = this->pInfo.bln_resetable;
	pInfo.id = this->pInfo.id;
	pInfo.fileInfo = info;
	
}

void Process::sendInfo(const QString& string)
{
	emit this->Info(this->pInfo.id,this->pInfo.fileInfo.fileName(), string);
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
	emit Exception(this->pInfo.id,this->pInfo.fileInfo.absolutePath() + string);
	return;
}

ProcessManager::~ProcessManager()
{
}
ProcessManager::ProcessManager()
{
}


void ProcessManager::init()
{
	this->createConfigFolderIfNeeded();

}

qint32 ProcessManager::AddProcess(QString Path)
{
	Process* process = new Process();
	process->init(Path, this->list_process.size());
	qint32 id = this->list_process.size();
	this->list_process.insert(id, process);

	//需要绑定事件
	connect(process, &Process::Exception, this, &ProcessManager::Exception);			//异常
	connect(process, &Process::Info, this,& ProcessManager::Info);					//通知

	this->SaveSettingsFile();

	return id;
}

void ProcessManager::DeleteProcess(qint32 id)
{
	this->hash_info.remove(id);
	this->list_process.remove(id);

	this->SaveSettingsFile();
}

void ProcessManager::SetProcess(qint32 id, QFileInfo fileInfo)
{
	if (this->list_process.contains(id)) {
		Process* p = this->list_process.value(id);
		p->setProcessInfo(fileInfo);
	}
	else {
		Process* process = new Process();
		process->init(fileInfo.absolutePath(), this->list_process.size());

		this->list_process.insert(this->list_process.size(),process);
	}

	this->SaveSettingsFile();
}

void ProcessManager::SetProcess(qint32 id, ProcessInfo processInfo)
{
	if (this->list_process.contains(id)) {
		Process* p = this->list_process.value(id);
		p->setProcessInfo(processInfo);
	}
	else {
		Process* process = new Process();
		process->init(processInfo.fileInfo.absolutePath(), this->list_process.size());

		this->list_process.insert(this->list_process.size(), process);
	}

	this->SaveSettingsFile();
}

ProcessInfo ProcessManager::getProcess(qint32 id)
{
	if (this->list_process.contains(id)) {
		return this->list_process.value(id)->getProcessInfo();
	}
	return ProcessInfo();
}




QHash<qint32, ProcessInfo> ProcessManager::getProcessInfo()
{
	this->hash_info.clear();
	QList<qint32> list_id = list_process.keys();
	for (auto key : list_id) {
		this->hash_info.insert(key, list_process.value(key)->getProcessInfo());
	}
	return this->hash_info;
}

void ProcessManager::PauseAll(bool pauseAll)
{
	for (auto process : this->list_process) {
		process->setResetableTimer(pauseAll);
	}
}

void ProcessManager::sendInfo(const QString& string)
{
	emit this->Info(0, "", string);
}

void ProcessManager::createConfigFolderIfNeeded()
{
	//QDir dir("."); // "." 表示程序当前文件夹
	//QString configFolderName = "config"; // 配置文件夹名称
	 // 获取当前应用程序所在的目录
	QString appPath = QDir::currentPath();

	// 检查config文件夹是否存在，如果不存在则创建该文件夹
	QString configPath = appPath + "/config";
	if (!QDir(configPath).exists()) {
		qDebug() << "Creating config directory...";
		if (!QDir().mkdir(configPath)) {
			qDebug() << "Failed to create config directory!";
			return;
		}
	}
	//if (!dir.exists(configFolderName)) { // 检查文件夹是否存在
	//	QDir().mkdir(configFolderName); // 创建文件夹
	//}
	// 在config文件夹中创建ProtectProcess.ini文件，如果不存在
	QString configFile = configPath + "/ProtectProcess.ini";
	QString configIni = configPath + "/ProtectProcess";
	QFile file(configFile);
	if (!QFile(configFile).exists()) {
		qDebug() << "Creating ProtectProcess.ini file...";
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << "Failed to create ProtectProcess.ini file!";
			return;
		}
	}

	//QString configFile = QString("%1/ProtectProcess.ini").arg(configPath)/*.arg(configFolderName)*/;
	//QFile file(configFile);

	if (!file.exists()) { // 检查文件是否存在
		qDebug() << "1.ProtectProcess.ini dont exist yet";
		return;
	}
	file.close();
	this->configIniFilepath = configFile;
	
	QSettings temp(configIni);
	bool result = temp.isWritable();
	QList<QString> list_string = temp.allKeys();
	for (auto key : list_string) {
		//添加到目录下
		this->AddProcess(temp.value(key).toString());
	}

}

void ProcessManager::SaveSettingsFile()
{
	QSettings setting(this->configIniFilepath);

	if (!setting.isWritable()) {
		sendException("Cant write QSettings");
		return;
	}

	for (auto value : this->list_process) {
		setting.setValue(QString::number(value->getProcessInfo().id), value->getProcessInfo().fileInfo.absolutePath());
	}
	//保存写入磁盘
	setting.sync();
}

void ProcessManager::sendException(const QString& string) {
	emit this->Exception(0, string);
}
