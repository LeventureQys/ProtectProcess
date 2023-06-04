#include "MainApplication.h"

MainApplication::MainApplication(int& argc, char** argv) :
	QApplication(argc, argv)
{
	//this->initApplication();
	QList<QString> string_argv;
	for (int i = 0; i < argc; ++i) {
		string_argv.append(QString(argv[i]));
	}

	this->AnalysisArgs(string_argv);
	QString configFilePath = QCoreApplication::applicationDirPath() + "/config/ProtectProcess.ini";
	this->m_settings = new QSettings(configFilePath, QSettings::IniFormat);

	// ע������
	const QString keyName = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	// ��ȡӦ�ó���·��
	const QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

	// ���� QSettings ���󣬲�ʹ�����涨��ļ������г�ʼ��
	QSettings settings(keyName, QSettings::NativeFormat);

#ifdef RELEASE
	// ����Ӧ�ó���������
	settings.setValue(".", QVariant(appPath));
#endif // RELEASE

	//��ȡ������
	this->CheckDevloperMode();	//���Ȩ��
	this->AnalysisArgs(string_argv);
	
#ifdef QDEBUG_H
	SystemInfo::Singleton()->Dev_Mode = DevState::Developer_Mode;	//debugģʽ��Ĭ�Ͻ��뿪����ģʽ
#endif
}

MainApplication::~MainApplication()
{

}
void MainApplication::initApplication() {
	SystemInfo::Singleton(); //��ʼ����������
	//manager.init();			//��ʼ�����̹������

}

void MainApplication::AnalysisArgs(QList<QString> argv)
{
	//�����������������������
	//���ڰ�ȫ���ǣ��ѹر���������뿪����ģʽ�Ͳ���ģʽ�����
	// ���������Ҫ���뿪����ģʽ�Ͳ���ģʽ����Ҫ�����ҵĸ�����Կ����Ϊ�˺����Ŀ������ܹ�����ʹ��
	// �ҽ���� ������Կ ��Դ��github��ȥ�������Ҫ�����������Ŀ¼�»�ȡ
	// δ���ϸ���ܣ���Ȼ����Ҳ����������Կ������Դ�붼��������
	// https://github.com/LeventureQys/Leventure_DevelopKey
	// 

	if (argv.contains("Dev")) {
		SystemInfo::Singleton()->Dev_Mode = DevState::Developer_Mode;
	}



}

void MainApplication::CheckDevloperMode()
{
	// ���������ڴ棬��ָ��Ψһ�ļ���
	QSharedMemory sharedMemory("Leventure_Developer");

	// ���������ڴ棬�Ա��ȡ����
	if (sharedMemory.lock()) {

		// �ӹ����ڴ��ж�ȡ���ݲ�ת��Ϊ QString ����
		QString* data = (QString*)sharedMemory.data();
		QString developer = *data;

		if (developer.contains("DeveloperMode")) {
			SystemInfo::Singleton()->Dev_Mode = DevState::Developer_Mode;
		}
		else if (developer.contains("InfoMode")) {
			SystemInfo::Singleton()->Dev_Mode = DevState::Infomation_Mode;
		}
		else {
			SystemInfo::Singleton()->Dev_Mode = DevState::None;
			qDebug() << tr(QString("DeveloperTool's Shared Memory Dectived.Nothing Marked : %1").arg(developer).toLocal8Bit());
		}
		
		// ���������ڴ�
		sharedMemory.unlock();

		// ����ӹ����ڴ��ȡ��������
		
	}
}

void MainApplication::ReadDevloperMode()
{
	switch (SystemInfo::Singleton()->Dev_Mode) {
	case DevState::Developer_Mode: {
		this->viewer.show();
		break;}
	case DevState::Infomation_Mode: {
		break;}
	case DevState::None: {
		break;}
	}
}

void MainApplication::Start() {
	
	this->initApplication();		//��ʼ������
	this->viewer.initView();
	this->viewer.show();
	this->ReadDevloperMode();		//��ȡ������ģʽ


}

