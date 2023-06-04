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
	this->ui.tableWidget->setColumnWidth(0, 150);		//�Ƿ��ػ�
	this->ui.tableWidget->setColumnWidth(1, 350);		//·��
	this->ui.tableWidget->setColumnWidth(2, 150);		//˵��

	QStringList titleList;
	titleList << "�Ƿ��ػ�" << "·��" << "˵��";

	this->ui.tableWidget->setHorizontalHeaderLabels(titleList);

	if (!this->manager.get_bln_init() || this->manager.getProcessInfo().size() == 0) {
		qDebug() << "Cant Read Manager";
		return;
	}

	for each (auto key in this->manager.getProcessInfo().keys())
	{
		this->AddProcess_view(this->manager.getProcess(key));	//��ӽ���
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
		QTableWidgetItem* item = this->ui.tableWidget->item(i, 0);	//��i�е�1��Ԫ��
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
		//QTableWidgetItem* id = this->ui.tableWidget->item(i, 0);	//��i�е�1��Ԫ��
		QWidget* cBox = this->ui.tableWidget->cellWidget(i, 0);
		QTableWidgetItem* filePath = this->ui.tableWidget->item(i, 1);
		QTableWidgetItem* Info = this->ui.tableWidget->item(i, 2);
		if ( cBox == nullptr || filePath == nullptr) {
			continue;
		}
		if (filePath->text().isEmpty()) {			//û�д��ļ�·����ֱ��ɾ����һ�� 
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
		// �û��ɹ�ѡ����һ���ļ������Դ�����
		qDebug() << "Selected file: " << fileName;
	}
	else
	{
		// �û�ȡ�����ļ�ѡ��Ի���
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

	// ��ȡ��ǰѡ�е��кţ������ж���ѡ�У�
	QList<QTableWidgetItem*> items = ui.tableWidget->selectedItems();
	QSet<int> rows;
	for(QTableWidgetItem * item: items) {
		rows.insert(item->row());
		QTableWidgetItem* line = this->ui.tableWidget->item(item->row(), 1);
		this->manager.DeleteProcess(line->text());
	}

	// ����ɾ������ѡ�е���
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

bool Process::init(QString filePath, qint32 id, QString info,bool resetable)
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
	this->pInfo.info = info;
	this->setResetable(resetable);
	return true;
}

bool Process::setResetableTimer(bool timerOn)
{
	if (timerOn) {
		//����ʱ��
		if (!this->bln_isPause) {
			this->timer.start(5000);
			return true;
		}
		else {
			return false;	//����Ѿ�������ʱ����������ʱ��
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

	//��Ҫ���¼�
	connect(process, &Process::Exception, this, &ProcessManager::Exception);			//�쳣
	connect(process, &Process::Info, this,& ProcessManager::Info);					//֪ͨ

	this->SaveSettingsFile();

	return id;
}

qint32 ProcessManager::AddProcess(QString Path, QString info, bool blnResetable)
{
	Process* process = new Process();

	process->init(Path, this->list_process.size(), info,blnResetable);

	qint32 id = this->list_process.size();
	this->list_process.insert(id, process);

	//��Ҫ���¼�
	connect(process, &Process::Exception, this, &ProcessManager::Exception);			//�쳣
	connect(process, &Process::Info, this, &ProcessManager::Info);					//֪ͨ

	process->setResetable(process->getProcessInfo().bln_resetable);				//������������Ƿ��ػ�

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
			//����һ��ָ��֮ǰ����Ҫֹͣ���м�ʱ��
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
	//�ж��Ƿ����ͬ·��
	bool bln_exist = this->exist(processInfo.fileInfo.absoluteFilePath());
	
	if (bln_exist) {
		//�ҵ�ָ������
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
	//QDir dir("."); // "." ��ʾ����ǰ�ļ���
	//QString configFolderName = "config"; // �����ļ�������
	 // ��ȡ��ǰӦ�ó������ڵ�Ŀ¼
	QString appPath = QDir::currentPath();

	// ���config�ļ����Ƿ���ڣ�����������򴴽����ļ���
	QString configPath = appPath + "/config";
	if (!QDir(configPath).exists()) {
		this->sendInfo("Creating config directory...");
		if (!QDir().mkdir(configPath)) {
			this->sendInfo("Failed to create config directory!");
			return;
		}
	}
	// ��config�ļ����д���ProtectProcess.ini�ļ������������
	QString configFile = configPath + "/ProtectProcess.txt";
	QFile file(configFile);
	if (!QFile(configFile).exists()) {
		this->sendInfo("Creating ProtectProcess.ini file...");
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			this->sendInfo("Failed to create ProtectProcess.ini file!");
			return;
		}
	}

	//��JSON����ʽ��ţ�������Settings����ʽ��ţ���֪��Ϊʲôû������ʹ��
	if (!file.exists()) { // ����ļ��Ƿ����
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

		//json ������������:ID,Path,Info
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
		
		json_arr.append(obj);	//����һ��
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
