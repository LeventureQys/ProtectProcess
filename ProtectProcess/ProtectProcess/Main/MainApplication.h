#pragma once
//Author:Leventure
//Date:2023-4-23
//Info:主进程，该进程只应该被启动一次
//
#include <QApplication>
#include "qapplication.h"
#include "../Tools/Tools_South.h"
#include "../LocalInfo/SystemInfo.h"
#include "qlist.h"
#include "qsettings.h"
#include "qtimer.h"

#include "qdir.h"
class MainApplication  : public QApplication
{
	Q_OBJECT

public:

	MainApplication(int& argc, char** argv);
	~MainApplication();
	
	void Start();
	void StartWatching() {
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, [=]() {
			QList<QString> processNames = m_settings->childKeys();
			foreach(const QString & name, processNames) {
				QString path = m_settings->value(name).toString();
				if (!isProcessRunning(name) && path != "") {
					QProcess::startDetached(path);
				}
			}
			});
	}


private:
	
	QSettings *m_settings;
	void createConfigFolderIfNeeded() {
		QString folderPath = QDir::currentPath() + "/config";
		QDir folder(folderPath);

		if (!folder.exists()) {
			folder.mkpath(".");
		}
	}
	void initApplication();
	bool isProcessRunning(const QString& processName) {
		QProcess process;
		process.start("pidof " + processName);
		process.waitForFinished(-1);

		QByteArray output = process.readAllStandardOutput();
		QList<QByteArray> pids = output.split(' ');

		return !pids.isEmpty();
	}
	void AnalysisArgs(QList<QString> argv); //启动项检查，开发者模式检查

	//寻常日志
	inline void nLog(QString FuncName, QString strValue) {
		South_Tools::WriteNormalMessage(this->metaObject()->className(), FuncName, strValue);
	}
	//异常日志
	inline void eLog(QString FuncName, QString strValue) {
		South_Tools::WriteErrorMessage(this->metaObject()->className(), FuncName, strValue);
	}

	QSharedPointer<QThread> thread_tcp;
	QSharedPointer<QThread> thread_file;
};
