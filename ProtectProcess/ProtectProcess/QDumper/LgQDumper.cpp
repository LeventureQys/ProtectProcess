
#include "LgQDumper.h"

#include <QObject>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#pragma comment(lib, "dbghelp.lib")

char* g_qsDumpName;

QtDumpGenerate::QtDumpGenerate()
{

}

void QtDumpGenerate::ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyyMMdd_hhmmss");
	QString dmpName = QString::fromLocal8Bit(g_qsDumpName);
	QString dmpFileName = QCoreApplication::applicationDirPath() + "/Log(Intercom)/" + dmpName + current_date + ".dmp";
	EXCEPTION_RECORD* record = pException->ExceptionRecord;
	QString errCode(QString::number(record->ExceptionCode, 16));
	QString errAddr(QString::number((uint)record->ExceptionAddress, 16));
	QString errFlag(QString::number(record->ExceptionFlags, 16));
	QString errPara(QString::number(record->NumberParameters, 16));

	qDebug() << " ##errCode: " << errCode;
	qDebug() << " ##errAddr: " << errAddr;
	qDebug() << " ##errFlag: " << errFlag;
	qDebug() << " ##errPara: " << errPara;

	HANDLE hDumpFile = CreateFile((LPCWSTR)QString(dmpFileName).utf16(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hDumpFile != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
		CloseHandle(hDumpFile);
	}
	else {
		qDebug() << " ##hDumpFile == null";
	}
}

long ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
	QtDumpGenerate dmpGenerate;
	dmpGenerate.ApplicationCrashHandler(pException);
	return EXCEPTION_EXECUTE_HANDLER;
}
