#pragma once
//Author:Leventure
//Date:2023-4-23
//Info:系统属性，用于存放一些当前系统下的信息
#include <QObject>
#include "qmutex.h"
#include "qsharedpointer.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "qexception.h"
#include "qjsonarray.h"
//大于0提权
enum class DevState {
	None = 0, //用户模式
	Developer_Mode = 1 , //开发者模式
	Infomation_Mode = 2 //调试模式
};

class SystemInfo  : public QObject
{
	Q_OBJECT

public:
	SystemInfo();
	~SystemInfo();

	static SystemInfo* SystemInfo::Singleton() {
		static QMutex mutext;
		static QSharedPointer<SystemInfo> inst;
		if (Q_UNLIKELY(!inst)) {
			if (!inst) {
				inst.reset(new SystemInfo());
			}
		}
		return inst.data();
	}

	DevState Dev_Mode = DevState::None;   //开发者模式

};
