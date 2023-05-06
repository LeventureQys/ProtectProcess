#include "Tools_South.h"
void South_Tools::WriteErrorMessage(QString ModuleName, QString FunctionName, QString ErrorMessage) {
	QString ErrorTitle = "BaseTools::";
	if (ModuleName.isEmpty()) {
		qDebug() << ErrorTitle + "WriteErrorMessage-->ModuleName is Empty!";
		return;
	}
	if (ErrorMessage.isEmpty()) {
		qDebug() << ErrorTitle + "WriteErrorMessage-->ErrorMessage is Empty!";
		return;
	}
	//创建日志文件夹
	QString str_filename;
	QString str_filepath;
	QString current_datetime = QDate::currentDate().toString("MM-dd");
	QString current_time = current_datetime + " " + QTime::currentTime().toString("hh::mm::ss ");
	//报错日志文件只需要到具体哪一天的就行
	str_filepath = QApplication::applicationDirPath() + "/ErrorLog";
	str_filename = QApplication::applicationDirPath() + "/ErrorLog/" + current_datetime + ModuleName + ".txt";
	QDir logDir(str_filename);
	QString fileName = str_filename;
	if (static_cast<int>(SystemInfo::Singleton()->Dev_Mode) != 0) {
		qDebug() << "Errlog--->Makefile:" + str_filename + "result:" + logDir.mkdir(str_filepath) + "\ninfo:" + ErrorMessage;
	}

	QFile file(fileName);
	//    file.setFileName("log.txt");
		//只写 追加写入
	if (file.open(QIODevice::Append | QIODevice::Text | QIODevice::WriteOnly))
	{
		QTextStream in(&file);
		in << current_time + " " + FunctionName + " : " + ErrorMessage << endl;
	}
}

void South_Tools::WriteNormalMessage(QString ModuleName, QString FunctionName, QString NormalMessage) {
	QString ErrorTitle = "BaseTools::";
	if (ModuleName.isEmpty()) {
		qDebug() << ErrorTitle + "WriteErrorMessage-->ModuleName is Empty!";
		return;
	}
	if (NormalMessage.isEmpty()) {
		qDebug() << ErrorTitle + "WriteErrorMessage-->ErrorMessage is Empty!";
		return;
	}
	//创建日志文件夹
	QString str_filename;
	QString str_filepath;
	QString current_datetime = QDate::currentDate().toString("MM-dd");
	QString current_time = current_datetime + " " + QTime::currentTime().toString("hh::mm::ss ");
	//报错日志文件只需要到具体哪一天的就行
	str_filepath = QApplication::applicationDirPath() + "/NormalLog";
	str_filename = QApplication::applicationDirPath() + "/NormalLog/" + current_datetime + FunctionName + ".txt";
	QDir logDir(str_filename);
	QString fileName = str_filename;
	if (static_cast<int>(SystemInfo::Singleton()->Dev_Mode) != 0) {
		qDebug() << "NorMal--->Makefile:" + str_filename + "result:" + logDir.mkdir(str_filepath) + "\ninfo:" + NormalMessage;
	}
	QFile file(fileName);
	//    file.setFileName("log.txt");
	//只写 追加写入
	if (file.open(QIODevice::Append | QIODevice::Text | QIODevice::WriteOnly))
	{
		QTextStream in(&file);
		in << current_time + " " + FunctionName + " : " + NormalMessage << endl;
	}
}
QString South_Tools::ParseColon(QString& insert)
{
	if (!insert.contains(":")) {
		return "Didn't Have : to Parse.Error in BaseTools::ParseColon";
	}
	QList<QString> list = insert.split(":");
	return list.at(1);
}

void South_Tools::set_Rigster(QString& location, QString& key, QVariant& value)
{
	QSettings setting(location, QSettings::NativeFormat);

	setting.setValue(key, value);
}

QVariant South_Tools::get_Rigster(QString& location, QString& key)
{
	QSettings setting(location, QSettings::NativeFormat);

	return setting.value(key);
}

bool South_Tools::isProcessExist(const QString& processName)
{
	QProcess process;
	process.start("tasklist");
	process.waitForFinished();

	QByteArray result = process.readAllStandardOutput();
	QString str = QString::fromUtf8(result);

	int times = str.count(processName, Qt::CaseInsensitive);
	/*int times = str.contains(processName);*/
	if (times > 1)
		return true;
	else
		return false;
}