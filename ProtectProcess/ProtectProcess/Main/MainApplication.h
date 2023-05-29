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
#include "qsharedmemory.h"
#include "qdir.h"
#include "../View/View.h"

class MainApplication  : public QApplication
{
	Q_OBJECT

		//不要把函数体写在头文件内部，即使这是可行的，都会影响IDE对后续函数的签名识别，而且这样写非常不专业(除了内联函数)
public:

	MainApplication(int& argc, char** argv);
	~MainApplication();
	
	void Start();

private:

	QSettings *m_settings;
	void initApplication();

	void AnalysisArgs(QList<QString> argv);			//启动项检查，开发者模式检查
	void CheckDevloperMode();						//共享内存检查，检查是否有开发者密钥
	void ReadDevloperMode();
	
	//寻常日志
	inline void nLog(QString FuncName, QString strValue) {
		South_Tools::WriteNormalMessage(this->metaObject()->className(), FuncName, strValue);
	}
	//异常日志
	inline void eLog(QString FuncName, QString strValue) {
		South_Tools::WriteErrorMessage(this->metaObject()->className(), FuncName, strValue);
	}

	
	View viewer;

};
