#pragma once
//Author:Leventure
//Date:2023-4-23
//Info:提供一些基本的功能
#pragma execution_character_set("utf-8")
#include <QObject>
#include "qdebug.h"
#include "qapplication.h"
#include "qdatetime.h"
#include "qdir.h"
#include "../LocalInfo/SystemInfo.h"
#include "qsettings.h"
#include "qprocess.h"
#include "qtextcodec.h"

#pragma region Logs



#pragma endregion

	namespace South_Tools {
		void WriteErrorMessage(QString ModuleName, QString FunctionName, QString ErrorMessage); //本地写入错误信息
		void WriteNormalMessage(QString ModuleName, QString FunctionName, QString NormalMessage); //本地写入寻常信息
		//获得冒号右边的内容
		QString ParseColon(QString& insert);
		//写入注册表
		void set_Rigster(QString& location, QString& key, QVariant& value);
		//读取注册表
		QVariant get_Rigster(QString& location, QString& key);
		bool isProcessExist(const QString& processName);

	}



	//Json相关
	//给定任意模板类，将其公开属性打包成一个Json字符串，使用此方法需要所有的公开属性均为Q_PROPERTY宏声明，该类提供单例。
	//序列化类Q_PROPERTY宏声明的属性 set/get函数命名规则：get/set+属性名 如getBirthday setList，大小写不限，如果是set方法需要在set方法前面加上Q_INVOKABLE 宏
	//如果需要反序列化数组，请保证数组中的所有数据结构是同一个类型，否则可能会出错
	//注:请尽量使用int不要使用qint32，使用double不要使用float
	class JsonMaker :public QObject {
		JsonMaker(QObject* parent = Q_NULLPTR) {
		}
		//提供单例
	public:
		static JsonMaker& JsonMaker::Singleton() {
			static JsonMaker Instance;
			return Instance;
			// TODO: 在此处插入 return 语句
		}

		//序列化类Q_PROPERTY宏声明的属性，如果有数组类型，请使用QList<QSharedPointer>来执行指定操作
		template<class T1>
		QString JsonSerialization(QSharedPointer<T1>T_Class_1) {
			auto T_Class = dynamic_cast<QObject*>(T_Class_1.data());
			QJsonObject jsonObject;
			//通过元对象定义成员
			const QMetaObject* metaObject = T_Class->metaObject();
			for (int i = 0; i < metaObject->propertyCount(); ++i) {
				QMetaProperty property = metaObject->property(i);

				if (!property.isReadable()) {
					continue;
				}
				//这个不知道是什么，暂时需要先屏蔽掉

				if (QString(property.name()) == "objectName") {
					continue;
				}

				//如果是QList

				if (QString(property.typeName()).contains("QList")) {
					//这里可能要根据常见类型进行一下分类
					QJsonArray jsonListArray;
					//输入一个模板类类型，输出一个jsonObject
					if (QString(property.typeName()) == "QList<QString>") {
						QList<QString> str_message = property.read(T_Class).value<QList<QString>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<qint32>" || QString(property.typeName()) == "QList<int>") {
						QList<qint32> str_message = property.read(T_Class).value<QList<qint32>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<qint64>") {
						QList<qint64> str_message = property.read(T_Class).value<QList<qint64>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<int>") {
						QList<int> str_message = property.read(T_Class).value<QList<int>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<bool>") {
						QList<bool> str_message = property.read(T_Class).value<QList<bool>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<double>") {
						QList<double> str_message = property.read(T_Class).value<QList<double>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<Float>") {
						QList<float> str_message = property.read(T_Class).value<QList<float>>();
						jsonListArray = QListToJsonArray(str_message);
					}
					else if (QString(property.typeName()) == "QList<QByteArray>") {
						QList<QByteArray> str_message = property.read(T_Class).value<QList<QByteArray>>();
						jsonListArray = QListToJsonArray(str_message);
					}

					jsonObject.insert(property.name(), QJsonValue(jsonListArray));
				}
				//如果不是QList
				else {
					QVariant result = property.read(T_Class);
					jsonObject[property.name()] = QJsonValue::fromVariant(property.read(T_Class));
				}
				qDebug() << property.name();
			}
			QJsonDocument doc(jsonObject);
			return doc.toJson(QJsonDocument::Compact);
		}
		//反序列化类Q_PROPERTY宏声明的属性，如果有数组类型，请使用QList
		template<class T>
		void JsonDeserialization(QSharedPointer<T> T_Class, const QString& jsonString)
		{
			auto qobject = dynamic_cast<QObject*>(T_Class.data());

			QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
			QJsonObject jsonObject = doc.object();
			// 使用QMetaObject的invokeMethod()函数来调用模板类T的setter函数
			const QMetaObject* metaObject = qobject->metaObject();

			for (int i = 0; i < metaObject->propertyCount(); ++i) {
				QMetaProperty property = metaObject->property(i);
				if (property.isReadable() && property.isWritable()) {
					QString propertyName = property.name();
					QString str_functinoname = QString("set" + propertyName);
					//为了转换成const char*类型必须的一个中间步骤
					QByteArray temp_qba_functinoname = str_functinoname.toLocal8Bit();
					const char* func_name = temp_qba_functinoname.data();

					if (jsonObject.contains(propertyName)) {
						QJsonValue value = jsonObject[propertyName];
						JsonMaker temp;
						qDebug() << value;
						switch (value.type()) {
						case QJsonValue::Type::Bool:
							QMetaObject::invokeMethod(qobject, func_name, Q_ARG(bool, value.toBool()));
							break;
						case QJsonValue::Type::Double:
							if (!QMetaObject::invokeMethod(qobject, func_name, Q_ARG(int, value.toDouble())))
								QMetaObject::invokeMethod(qobject, func_name, Q_ARG(double, value.toDouble()));
							break;
						case QJsonValue::Type::String:
							qDebug() << QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QString, value.toString()));
							break;
						case QJsonValue::Type::Array: {
							//如果是数组则需要根据情况进行解析
							if (!value.isArray()) {
								break;
							}
							QJsonArray arr = value.toArray();
							//下面确定数组类型
							this->JsonArrayDeserialization(qobject, func_name, arr);
						}
													break;
						case QJsonValue::Type::Object:
							QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QJsonValue, value));
							break;
						default:
							break;
						}
					}
				}
			}
		}

	private:
		//将模板类QList转换成JsonObject
		template<class T>
		QJsonArray QListToJsonArray(QList<T> list) {
			QJsonArray jsonArray;
			for each (T temp_T in list)
			{
				jsonArray.append(QJsonValue::fromVariant(temp_T));
			}

			return jsonArray;
		}

		//解析数组并注入QObject对象
		void JsonArrayDeserialization(QObject* qobject, const char* func_name, QJsonArray arr) {
			try {
				//判断类型
			   //QString
				if (arr[0].type() == QJsonValue::String) {
					QList<QString> list_result;
					QJsonValue value;

					for each (QJsonValue temp_value in arr)
					{
						list_result.append(temp_value.toString());
					}
					QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QList<QString>, list_result));
				}
				else if (arr[0].isDouble()) {
					//若为为整形
					if (arr[0].toDouble() == arr[0].toInt()) {
						qDebug() << arr[0].toDouble() << arr[0].toInt();
						QList<qint32> list_result;
						QList<int> list_result_2;
						QJsonValue value;

						for each (QJsonValue temp_value in arr)
						{
							//int 和 qint32都需要尝试，但请尽量尝试使用qint32，这段代码占用了两倍的内存，将来可能考虑删除
							list_result.append(temp_value.toInt());
							list_result_2.append(temp_value.toInt());
						}
						if (!QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QList<qint32>, list_result))) {
							QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QList<int>, list_result_2));
						}
					}
					//若为双精度
					else {
						QList<double> list_result;
						QList<float> list_result_2;
						QJsonValue value;

						for each (QJsonValue temp_value in arr)
						{
							list_result.append(temp_value.toDouble());
						}
						//double和float都会尝试，请尽量使用double
						if (!QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QList<double>, list_result))) {
							QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QList<float>, list_result_2));
						}
					}
				}if (arr[0].type() == QJsonValue::Bool) {
					QList<bool> list_result;
					QJsonValue value;

					for each (QJsonValue temp_value in arr)
					{
						list_result.append(temp_value.toBool());
					}
					QMetaObject::invokeMethod(qobject, func_name, Q_ARG(QList<bool>, list_result));
				}
			}
			catch (const QException& e) {
				South_Tools::WriteErrorMessage("JsonArrayDeserialization", "JsonArrayDeserialization", e.what());
			}
		}
	};


#pragma region Example调用实例
	//日志相关方法调用实例：
	//
	// 1.调用错误日志
	// 参数:1.模块文件名称 2.报错函数名称 3.错误信息内容
	//LBD_BaseTools::WriteErrorMessage(ModuleName, FuncName,Message);
	// 2.调用关键信息日志
	// 参数：1.模块文件名称 2.关键信息函数或参数名称 3.关键信息内容
	//LBD_BaseTools::WriteNormalMessage(ModuleName,FuncName, Message);
		//向本地写入错误信息日志参数：1.报错的文件名 2.报错的函数名 3.错误信息

	//Json相关方法调用实例：
	//
	// 如果想要调用JsonMaker类来把你的类成员元素，假设是A a，其中包含元素qint32 a1,QString a2,bool a3进行封装，那么你需要使用Q_PROPERTY来
	// 声明封装a1,a2,a3元素和其set/get方法（如果需要解析就需要set方法，如果需要封装就需要get方法)，set/get方法命名规则为set/get+元素名称
	// 比如seta1,geta2,其中必须对大小写做出规范，set/get后面的元素名称必须与大小写保持一致，否则可能
	// 无法正常使用该JsonMaker
	//
	// 调用方法如下:
	// 1.封装字符串
	// A a;
	// QString result = JsonMaker::Singleton().JsonSerialization<Tester1>(tester);
	// 2.解析字符串
	// QSharedPointer<A> a;
	// JsonMaker::Singleton().JsonDeserialization<Tester1>(a, Json);
	// 调用完毕后a中的对应数据都会被Json字符串中的数据覆盖
	// 详情可以参考Tester1类
	//

#pragma endregion
