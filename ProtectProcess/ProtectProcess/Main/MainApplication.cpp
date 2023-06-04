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

	// 注册表键名
	const QString keyName = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	// 获取应用程序路径
	const QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

	// 创建 QSettings 对象，并使用上面定义的键名进行初始化
	QSettings settings(keyName, QSettings::NativeFormat);

#ifdef RELEASE
	// 设置应用程序自启动
	settings.setValue(".", QVariant(appPath));
#endif // RELEASE

	//读取启动项
	this->CheckDevloperMode();	//检查权限
	this->AnalysisArgs(string_argv);
	
#ifdef QDEBUG_H
	SystemInfo::Singleton()->Dev_Mode = DevState::Developer_Mode;	//debug模式下默认进入开发者模式
#endif
}

MainApplication::~MainApplication()
{

}
void MainApplication::initApplication() {
	SystemInfo::Singleton(); //初始化单例对象
	//manager.init();			//初始化进程管理对象

}

void MainApplication::AnalysisArgs(QList<QString> argv)
{
	//这个函数仅用于输入启动项
	//出于安全考虑，已关闭启动项进入开发者模式和测试模式的入口
	// 现在如果需要进入开发者模式和测试模式，需要启动我的个人密钥程序，为了后来的开发者能够方便使用
	// 我将这个 个人密钥 开源到github上去，如果需要可以在在这个目录下获取
	// 未作严格加密，当然了你也可以自制密钥，反正源码都在你手上
	// https://github.com/LeventureQys/Leventure_DevelopKey
	// 

	if (argv.contains("Dev")) {
		SystemInfo::Singleton()->Dev_Mode = DevState::Developer_Mode;
	}



}

void MainApplication::CheckDevloperMode()
{
	// 创建共享内存，并指定唯一的键名
	QSharedMemory sharedMemory("Leventure_Developer");

	// 锁定共享内存，以便读取数据
	if (sharedMemory.lock()) {

		// 从共享内存中读取数据并转换为 QString 类型
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
		
		// 解锁共享内存
		sharedMemory.unlock();

		// 处理从共享内存读取到的数据
		
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
	
	this->initApplication();		//初始化进程
	this->viewer.initView();
	this->viewer.show();
	this->ReadDevloperMode();		//读取开发者模式


}

