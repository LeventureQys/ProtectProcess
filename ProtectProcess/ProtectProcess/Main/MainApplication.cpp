#include "MainApplication.h"

MainApplication::MainApplication(int& argc, char** argv) :
	QApplication(argc, argv)
{
	this->createConfigFolderIfNeeded();
	this->initApplication();
	QList<QString> string_argv;
	for (int i = 0; i < argc; ++i) {
		string_argv.append(QString(argv[i]));
	}

	this->AnalysisArgs(string_argv);
	QString configFilePath = QCoreApplication::applicationDirPath() + "/config/ProtectProcess.ini";
	this->m_settings = new QSettings(configFilePath, QSettings::IniFormat);
	
}

MainApplication::~MainApplication()
{

}
void MainApplication::initApplication() {
	SystemInfo::Singleton(); //初始化单例对象
}

void MainApplication::AnalysisArgs(QList<QString> argv)
{
	//出于安全考虑，已关闭启动项进入开发者模式和测试模式的入口
	// 现在如果需要进入开发者模式和测试模式，需要启动我的个人密钥程序，为了后来的开发者能够方便使用
	// 我将这个 个人密钥 开源到github上去，如果需要可以在在这个目录下获取
	// 未作严格加密，当然了你也可以自制密钥，反正源码都在你手上
	// https://github.com/LeventureQys/Leventure_DevelopKey
	// 
	// Leventure TODO:目前这个进程识别有问题，暂时不适用这个密钥了，直接通过启动项算了
	if (argv.contains("Dev")) {
		SystemInfo::Singleton()->Dev_Mode = DevState::Developer_Mode;
	}



}

void MainApplication::Start() {
	//提供初始化
	//解析启动状态
	switch (SystemInfo::Singleton()->Dev_Mode)
	{
	case DevState::None:
		break;
	case DevState::Developer_Mode:
		break;
	case DevState::Infomation_Mode:
		break;
	default:
		break;
	}

}

