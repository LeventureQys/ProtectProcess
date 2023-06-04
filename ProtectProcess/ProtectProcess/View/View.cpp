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

	this->manager.init();
	this->ui.tableWidget->setColumnCount(3);
	//this->ui.tableWidget->setColumnWidth(0, 50);		//id
	this->ui.tableWidget->setColumnWidth(0, 150);		//是否守护
	this->ui.tableWidget->setColumnWidth(1, 350);		//路径
	this->ui.tableWidget->setColumnWidth(2, 150);		//说明

	QStringList titleList;
	titleList << "是否守护" << "路径" << "说明";

	this->ui.tableWidget->setHorizontalHeaderLabels(titleList);

	if (!this->manager.get_bln_init() || this->manager.getProcessInfo().size() == 0) {
		qDebug() << "Cant Read Manager";
		return;
	}

	for each (auto key in this->manager.getProcessInfo().keys())
	{
		this->AddProcess_view(this->manager.getProcess(key));	//添加进程
	}
	
}

void View::AddProcess_view(ProcessInfo pInfo)
{

	this->ui.tableWidget->insertRow(this->ui.tableWidget->rowCount());
	QTableWidgetItem* id = new QTableWidgetItem(QString::number(pInfo.id));
	QTableWidgetItem* filePath = new QTableWidgetItem(pInfo.fileInfo.absoluteFilePath());
	QTableWidgetItem* Info = new QTableWidgetItem(pInfo.info);

	QCheckBox* checkBox = new QCheckBox();
	checkBox->setChecked(pInfo.bln_resetable);
	//this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount() - 1, 0, id);
	//this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount(), 2, id);
	this->ui.tableWidget->setCellWidget(this->ui.tableWidget->rowCount() - 1, 0, checkBox);
	this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount() - 1, 1, filePath);
	this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount() - 1, 2, Info);

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

void View::InitTableView()
{
	if (this->manager.getProcessInfo().size() == 0) {
		qDebug() << "manager's processinfo is empty";
		return;
	}

	for each (auto key in this->manager.getProcessInfo().keys())
	{

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
		//QTableWidgetItem* id = this->ui.tableWidget->item(i, 0);	//第i行第1个元素
		QWidget* cBox = this->ui.tableWidget->cellWidget(i, 0);
		QTableWidgetItem* filePath = this->ui.tableWidget->item(i, 1);
		QTableWidgetItem* Info = this->ui.tableWidget->item(i, 2);
		if ( cBox == nullptr || filePath == nullptr) {
			continue;
		}
		if (filePath->text().isEmpty()) {			//没有存文件路径的直接删除这一行 
			this->ui.tableWidget->removeRow(i);
			return;
		}
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(cBox);

		ProcessInfo processInfo;
		processInfo.id = this->manager.getSize();
		processInfo.bln_resetable = checkBox->isChecked();
		processInfo.fileInfo = QFileInfo(filePath->text());
		processInfo.info = Info->text();

		this->manager.SetProcess(processInfo);

	}
}

void View::on_btn_add_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select File", QDir::homePath(), "All files (*.*)");

	if (!fileName.isEmpty())
	{
		// 用户成功选择了一个文件，可以处理它
		qDebug() << "Selected file: " << fileName;
	}
	else
	{
		// 用户取消了文件选择对话框
		qDebug() << "User canceled file selection.";
		return;
	}



	this->ui.tableWidget->insertRow(this->ui.tableWidget->rowCount());
	QCheckBox* checkBox = new QCheckBox();
	this->ui.tableWidget->setCellWidget(this->ui.tableWidget->rowCount() - 1, 0, checkBox);
	QTableWidgetItem* item = new QTableWidgetItem(fileName);

	this->ui.tableWidget->setItem(this->ui.tableWidget->rowCount() - 1, 1, item);
}

void View::on_btn_del_clicked()
{

	// 获取当前选中的行号（可能有多行选中）
	QList<QTableWidgetItem*> items = ui.tableWidget->selectedItems();
	QSet<int> rows;
	for(QTableWidgetItem * item: items) {
		rows.insert(item->row());
		QTableWidgetItem* line = this->ui.tableWidget->item(item->row(), 1);
		this->manager.DeleteProcess(line->text());
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

bool Process::init(QString filePath, qint32 id, QString info,bool resetable)
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
	this->pInfo.info = info;
	this->setResetable(resetable);
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
	this->setResetable(pInfo.bln_resetable);
	this->pInfo = info;
}

void Process::setProcessInfo(QFileInfo& info)
{
	ProcessInfo pInfo;
	pInfo.bln_resetable = this->pInfo.bln_resetable;
	pInfo.id = this->pInfo.id;
	pInfo.fileInfo = info;
	
}

bool Process::isEmpty()
{
	return !this->bln_init;
}

Process& Process::Empty()
{
	Process process;
	return process;
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

	this->process.setProgram(this->pInfo.fileInfo.absoluteFilePath());
	qDebug() << this->pInfo.fileInfo.absoluteFilePath();
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
	emit Exception(this->pInfo.id,this->pInfo.fileInfo.absoluteFilePath() + string);
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

	this->bln_init = true;
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

qint32 ProcessManager::AddProcess(QString Path, QString info, bool blnResetable)
{
	Process* process = new Process();

	process->init(Path, this->list_process.size(), info,blnResetable);

	qint32 id = this->list_process.size();
	this->list_process.insert(id, process);

	//需要绑定事件
	connect(process, &Process::Exception, this, &ProcessManager::Exception);			//异常
	connect(process, &Process::Info, this, &ProcessManager::Info);					//通知

	process->setResetable(process->getProcessInfo().bln_resetable);				//根据情况决定是否守护

	this->SaveSettingsFile();

	return id;
}



void ProcessManager::DeleteProcess(qint32 id)
{
	this->hash_info.remove(id);
	this->list_process.remove(id);

	this->SaveSettingsFile();
}

bool ProcessManager::DeleteProcess(QString filePath)
{
	QList<int> list_key = this->list_process.keys();
	for (auto key : list_key) {
		if (list_process[key]->getProcessInfo().fileInfo.absoluteFilePath().contains(filePath)) {
			//销毁一个指针之前，需要停止所有计时器
			for (auto child : this->list_process[key]->children())
			{
				QTimer* childTimer = qobject_cast<QTimer*>(child);
				if (childTimer != nullptr)
				{
					childTimer->stop();
					childTimer->deleteLater();
				}
			}
			this->list_process[key]->deleteLater();
			this->list_process.remove(key);
			this->SaveSettingsFile();
			return true;
		}
		this->list_process[key]->deleteLater();
	}
	this->SaveSettingsFile();
	return false;
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


void ProcessManager::SetProcess(ProcessInfo processInfo)
{
	//判断是否存在同路径
	bool bln_exist = this->exist(processInfo.fileInfo.absoluteFilePath());
	
	if (bln_exist) {
		//找到指定进程
		QList<int> list_key = this->list_process.keys();
		for (auto key : list_key) {
			if (list_process[key]->getProcessInfo().fileInfo.absoluteFilePath().contains(processInfo.fileInfo.absoluteFilePath())) {
				Process* p = list_process[key];
				p->setProcessInfo(processInfo);
			}
		}
	}
	else {
		Process* process = new Process();

		if (!process->init(processInfo.fileInfo.absoluteFilePath(), this->list_process.size(), processInfo.info,processInfo.bln_resetable)) {
			return;
		}

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

ProcessInfo ProcessManager::getProcess(QString filePath)
{
	//return ProcessInfo();
	for (auto process : this->list_process) {
		if (process->getProcessInfo().fileInfo.absoluteFilePath().contains(filePath)) {
			return process->getProcessInfo();
		}
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

bool ProcessManager::get_bln_init()
{
	return this->bln_init;
}

qint32 ProcessManager::getSize()
{
	return this->list_process.size();
}

bool ProcessManager::exist(QString filePath)
{
	
	QList<int> list_key = this->list_process.keys();
	for (auto key : list_key) {
		if (list_process[key]->getProcessInfo().fileInfo.absoluteFilePath().contains(filePath)) {
			return true;
		}
	}
	return false;
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
		this->sendInfo("Creating config directory...");
		if (!QDir().mkdir(configPath)) {
			this->sendInfo("Failed to create config directory!");
			return;
		}
	}
	// 在config文件夹中创建ProtectProcess.ini文件，如果不存在
	QString configFile = configPath + "/ProtectProcess.txt";
	QFile file(configFile);
	if (!QFile(configFile).exists()) {
		this->sendInfo("Creating ProtectProcess.ini file...");
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			this->sendInfo("Failed to create ProtectProcess.ini file!");
			return;
		}
	}

	//以JSON的形式存放，不再以Settings的形式存放，不知道为什么没法正常使用
	if (!file.exists()) { // 检查文件是否存在
		this->sendInfo("1.ProtectProcess.ini dont exist yet");
		return;
	}
	
	if (!file.open(QIODevice::ReadOnly)) {
		this->sendInfo("Cant Open File");
		return;
	}
	this->configIniFilepath = configFile;
	
	const auto strMessage = file.readAll();

	const auto json_doc = QJsonDocument::fromJson(strMessage);

	if (json_doc.isNull()) {
		this->sendInfo("QJson null");
		return;
	}

	const auto json_obj = json_doc.object();

	if (!json_obj.contains("data")) {
		this->sendInfo("jsonObject dont contains values that we need");
		return;
	}

	const auto data_val = json_obj.value("data");

	if (!data_val.isArray()) {
		this->sendInfo("data is not array");
		return;
	}

	const QJsonArray json_list = data_val.toArray();

	for (const auto& obj_val : json_list) {
		if (!obj_val.isObject()) {
			continue;
		}

		//json 包含三个部分:ID,Path,Info
		this->AddProcess(obj_val.toObject().value("Path").toString(), obj_val.toObject().value("Info").toString(),obj_val.toObject().value("Resetable").toBool());
	}
	

}

void ProcessManager::SaveSettingsFile()
{
	QFile file(this->configIniFilepath);
	if (!file.open(QIODevice::ReadWrite)) {
		this->sendInfo("Can't open file in ReadWrite");
		return;
	}

	QList<int> list_keys = this->list_process.keys();
	QJsonObject obj_data;
	QJsonArray json_arr;

	for (int key : list_keys) {
		ProcessInfo pi = this->list_process.value(key)->getProcessInfo();

		QJsonObject obj;
		obj["ID"] = pi.id;
		obj["Path"] = pi.fileInfo.absoluteFilePath();
		obj["info"] = pi.info;
		obj["Resetable"] = pi.bln_resetable;
		
		json_arr.append(obj);	//插入一个
	}

	obj_data["data"] = json_arr;

	QJsonDocument json_doc(obj_data);

	file.seek(0);
	file.write(json_doc.toJson());
	file.resize(file.pos());

	file.close();
}

void ProcessManager::sendException(const QString& string) {
	emit this->Exception(0, string);
}
