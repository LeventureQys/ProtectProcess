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
	this->ui.tableWidget->setColumnWidth(1, 150);		//�Ƿ��ػ�
	this->ui.tableWidget->setColumnWidth(2, 350);		//·��
	this->ui.tableWidget->setColumnWidth(3, 150);		//˵��

	QStringList titleList;
	titleList << "ID" << "�Ƿ��ػ�" << "·��" << "˵��";

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
		QTableWidgetItem* item = this->ui.tableWidget->item(i, 0);	//��i�е�1��Ԫ��
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
		QTableWidgetItem* id = this->ui.tableWidget->item(i, 0);	//��i�е�1��Ԫ��
		QWidget* cBox = this->ui.tableWidget->cellWidget(i, 1);
		QTableWidgetItem* filePath = this->ui.tableWidget->item(i, 2);
		QTableWidgetItem* Info = this->ui.tableWidget->item(i, 3);
		if (id == nullptr || cBox == nullptr || filePath == nullptr || Info == nullptr) {
			continue;
		}
		if (filePath->text().isEmpty()) {			//û�д��ļ�·����ֱ��ɾ����һ�� 
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
	// ��ȡ��ǰѡ�е��кţ������ж���ѡ�У�
	QList<QTableWidgetItem*> items = ui.tableWidget->selectedItems();
	QSet<int> rows;
	for(QTableWidgetItem * item: items) {
		rows.insert(item->row());
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

	//��Ҫ���¼�
	connect(process, &Process::Exception, this, &ProcessManager::Exception);			//�쳣
	connect(process, &Process::Info, this,& ProcessManager::Info);					//֪ͨ

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
	//QDir dir("."); // "." ��ʾ����ǰ�ļ���
	//QString configFolderName = "config"; // �����ļ�������
	 // ��ȡ��ǰӦ�ó������ڵ�Ŀ¼
	QString appPath = QDir::currentPath();

	// ���config�ļ����Ƿ���ڣ�����������򴴽����ļ���
	QString configPath = appPath + "/config";
	if (!QDir(configPath).exists()) {
		qDebug() << "Creating config directory...";
		if (!QDir().mkdir(configPath)) {
			qDebug() << "Failed to create config directory!";
			return;
		}
	}
	//if (!dir.exists(configFolderName)) { // ����ļ����Ƿ����
	//	QDir().mkdir(configFolderName); // �����ļ���
	//}
	// ��config�ļ����д���ProtectProcess.ini�ļ������������
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

	if (!file.exists()) { // ����ļ��Ƿ����
		qDebug() << "1.ProtectProcess.ini dont exist yet";
		return;
	}
	file.close();
	this->configIniFilepath = configFile;
	
	QSettings temp(configIni);
	bool result = temp.isWritable();
	QList<QString> list_string = temp.allKeys();
	for (auto key : list_string) {
		//��ӵ�Ŀ¼��
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
	//����д�����
	setting.sync();
}

void ProcessManager::sendException(const QString& string) {
	emit this->Exception(0, string);
}
