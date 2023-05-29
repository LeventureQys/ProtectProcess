#pragma once
//Author:Leventure
//Date:2023-4-23
//Info:�����̣��ý���ֻӦ�ñ�����һ��
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

		//��Ҫ�Ѻ�����д��ͷ�ļ��ڲ�����ʹ���ǿ��еģ�����Ӱ��IDE�Ժ���������ǩ��ʶ�𣬶�������д�ǳ���רҵ(������������)
public:

	MainApplication(int& argc, char** argv);
	~MainApplication();
	
	void Start();

private:

	QSettings *m_settings;
	void initApplication();

	void AnalysisArgs(QList<QString> argv);			//�������飬������ģʽ���
	void CheckDevloperMode();						//�����ڴ��飬����Ƿ��п�������Կ
	void ReadDevloperMode();
	
	//Ѱ����־
	inline void nLog(QString FuncName, QString strValue) {
		South_Tools::WriteNormalMessage(this->metaObject()->className(), FuncName, strValue);
	}
	//�쳣��־
	inline void eLog(QString FuncName, QString strValue) {
		South_Tools::WriteErrorMessage(this->metaObject()->className(), FuncName, strValue);
	}

	
	View viewer;

};
