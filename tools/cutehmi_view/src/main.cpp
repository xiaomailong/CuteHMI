#include "../cutehmi.metadata.hpp"
#include "../cutehmi.dirs.hpp"
#include "cutehmi/view/logging.hpp"

#include <cutehmi/ErrorInfo.hpp>
#include <cutehmi/Dialog.hpp>
#include <cutehmi/Dialogist.hpp>
#include <cutehmi/Singleton.hpp>

#include <cutehmi/app/CuteApp.hpp>

//<cutehmi_view-4.workaround target="Qt" cause="bug">
#include <QApplication>
// Instead of:
//  #include <QGuiApplication>
//</cutehmi_view-4.workaround>

#include <QQmlApplicationEngine>
#include <QIcon>
#include <QDir>
#include <QtDebug>
#include <QCommandLineParser>
#include <QQmlContext>
#include <QUrl>
#include <QCursor>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

using namespace cutehmi::view;

/**
 * Main function.
 * @param argc number of arguments passed to the program.
 * @param argv list of arguments passed to the program.
 * @return return code.
 */
int main(int argc, char * argv[])
{
	QCoreApplication::setOrganizationDomain(QString(CUTEHMI_VIEW_VENDOR).toLower());
	QCoreApplication::setApplicationName(CUTEHMI_VIEW_VENDOR " " CUTEHMI_VIEW_FRIENDLY_NAME);
	QCoreApplication::setApplicationVersion(QString("%1.%2.%3").arg(CUTEHMI_VIEW_MAJOR).arg(CUTEHMI_VIEW_MINOR).arg(CUTEHMI_VIEW_MICRO));

	try {

		if (qgetenv("QT_IM_MODULE").isEmpty())
			qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
		CUTEHMI_DEBUG("Input method: " << qgetenv("QT_IM_MODULE"));

		if (qgetenv("QT_IM_MODULE") == "qtvirtualkeyboard") {
			if (qgetenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH").isEmpty())
				qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", QByteArray(QDir("../layouts").absolutePath().toLocal8Bit()));
			CUTEHMI_DEBUG("Qt Virtual Keyboard layouts path: " << qgetenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH"));
		}

	//<Qt-Qt_5_7_0_Reference_Documentation-Threads_and_QObjects-QObject_Reentrancy-creating_QObjects_before_QApplication.assumption>
	// "In general, creating QObjects before the QApplication is not supported and can lead to weird crashes on exit, depending on the
	//	platform. This means static instances of QObject are also not supported. A properly structured single or multi-threaded application
	//	should make the QApplication be the first created, and last destroyed QObject."

		//<cutehmi_view-4.workaround target="Qt" cause="bug">
		QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
		cutehmi::app::CuteApp app(argc, argv);
		// Instead of:
		//	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
		//	QGuiApplication app(argc, argv);
		//</cutehmi_view-4.workaround>
		app.setWindowIcon(QIcon(":/img/icon.png"));

		QJsonDocument metadataJson;
		{
			QFile metadataFile(":/" CUTEHMI_VIEW_NAME "/cutehmi.metadata.json");
			if (metadataFile.open(QFile::ReadOnly)) {
				metadataJson = QJsonDocument::fromJson(metadataFile.readAll());
			} else
				qFatal("Could not open ':/" CUTEHMI_VIEW_NAME "/cutehmi.metadata.json' file.");
		}

		QCommandLineParser cmd;
		cmd.setApplicationDescription(QCoreApplication::applicationName() + "\n" + metadataJson.object().value("description").toString());
		cmd.addHelpOption();
		cmd.addVersionOption();
		QCommandLineOption fullScreenOption({"f", "fullscreen"}, QCoreApplication::translate("main", "Run application in full screen mode."));
		cmd.addOption(fullScreenOption);
		QCommandLineOption projectOption({"p", "project"}, QCoreApplication::translate("main", "Load QML project <URL>."), QCoreApplication::translate("main", "URL"));
		cmd.addOption(projectOption);
		QCommandLineOption hideCursorOption({"t", "touch"}, QCoreApplication::translate("main", "Touch screen (hides mouse cursor)."));
		cmd.addOption(hideCursorOption);
		QCommandLineOption styleOption("qstyle", QCoreApplication::translate("main", "Set Qt Quick <style>."), QCoreApplication::translate("main", "style"));
		cmd.addOption(styleOption);
		QCommandLineOption langOption("lang", QCoreApplication::translate("main", "Choose application <language>."), QCoreApplication::translate("main", "language"));
		cmd.addOption(langOption);
		QCommandLineOption basedirOption("basedir", QCoreApplication::translate("main", "Set base directory to <dir>."), QCoreApplication::translate("main", "dir"));
		cmd.addOption(basedirOption);
		cmd.process(app);

		CUTEHMI_DEBUG("Default locale: " << QLocale());

		QTranslator qtTranslator;
		if (cmd.isSet(langOption))
			qtTranslator.load("qt_" + cmd.value(langOption), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		else
			qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		app.installTranslator(& qtTranslator);

		if (cmd.isSet(styleOption)) {
			qputenv("QT_QUICK_CONTROLS_STYLE", cmd.value(styleOption).toLocal8Bit());
			CUTEHMI_DEBUG("Qt Quick style: " << cmd.value(styleOption));
		}

		if (cmd.isSet(hideCursorOption))
			QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
		//<cutehmi_view-5.workaround target="Qt" cause="bug">
		// When run on raw Xorg server application does not show up cursor unless some controls are hovered.
		//<cutehmi_view-6.workaround target="Qt" cause="bug">
		// On Windows cursor does not refresh properly when using QGuiApplication::setOverrideCursor().
#ifndef Q_OS_WIN
		else
			QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
#endif
		//</cutehmi_view-6.workaround>
		//</cutehmi_view-5.workaround>

		QDir baseDir(QCoreApplication::applicationDirPath() + "/..");
		if (cmd.isSet(basedirOption))
			baseDir = cmd.value(basedirOption);
		QString baseDirPath = baseDir.absolutePath() + "/";
		CUTEHMI_DEBUG("Base directory: " << baseDirPath);

		CUTEHMI_DEBUG("Library paths: " << QCoreApplication::libraryPaths());

		std::unique_ptr<QQmlApplicationEngine> engine(new QQmlApplicationEngine);
		engine->addImportPath(baseDirPath + CUTEHMI_DIRS_EXTENSION_INSTALL_DIRNAME);
		CUTEHMI_DEBUG("QML import paths: " << engine->importPathList());
		engine->rootContext()->setContextProperty("cutehmi_view_mainScreenURL", "qrc:/qml/DefaultScreen.qml");
		engine->load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

		if (!cmd.value(projectOption).isNull()) {
			CUTEHMI_DEBUG("Project:" << cmd.value(projectOption));
			QUrl projectUrl(cmd.value(projectOption));
			if (projectUrl.isValid()) {
				// Assure that URL is not mixing relative path with explicitly specified scheme, which is forbidden. QUrl::isValid() doesn't check this out.
				if (!projectUrl.scheme().isEmpty() && QDir::isRelativePath(projectUrl.path()))
					cutehmi::Dialog::Critical(QObject::tr("URL '%1' contains relative path along with URL scheme, which is forbidden.").arg(projectUrl.url()));
				else {
					// If source URL is relative (does not contain scheme), then make absolute URL: file:///baseDirPath/sourceUrl.
					if (projectUrl.isRelative())
						projectUrl = QUrl::fromLocalFile(baseDirPath).resolved(projectUrl);
					// Check if file exists and eventually set context property.
					if (projectUrl.isLocalFile() && !QFile::exists(projectUrl.toLocalFile()))
						cutehmi::Dialog::Critical(QObject::tr("Project file '%1' does not exist.").arg(projectUrl.url()));
					else
						engine->rootContext()->setContextProperty("cutehmi_view_mainScreenURL", projectUrl.url());
				}
			} else
				cutehmi::Dialog::Critical(QObject::tr("Invalid format of project URL '%1'.").arg(cmd.value(projectOption)));
		}

		//<Qt-Qt_5_9_1_Reference_Documentation-Qt_Core-C++_Classes-QCoreApplication-exec.assumption>
		// "We recommend that you connect clean-up code to the aboutToQuit() signal, instead of putting it in your application's main() function because on some
		//  platforms the exec() call may not return. For example, on Windows when the user logs off, the system terminates the process after Qt closes all top-level
		//  windows. Hence, there is no guarantee that the application will have time to exit its event loop and execute code at the end of the main() function after
		//  the exec() call."
		QObject::connect(& app, & cutehmi::app::CuteApp::aboutToQuit, [&]() {
			// It's quite important to destroy "engine" before cutehmi::CuteHMI::Instance() members, because they
			// may still be used by some QML components (for example in "Component.onDestroyed" handlers).
			engine.reset();

			cutehmi::destroySingletonInstances();

			if (cmd.isSet(hideCursorOption))
				QGuiApplication::restoreOverrideCursor();
			//<cutehmi_view-5.workaround>
			//<cutehmi_view-6.workaround target="Qt" cause="bug">
			// On Windows cursor does not refresh properly when using QGuiApplication::setOverrideCursor().
#ifndef Q_OS_WIN
			else
				QGuiApplication::restoreOverrideCursor();
#endif
			//</cutehmi_view-6.workaround>
			//</cutehmi_view-5.workaround>
		});

		return app.exec();

		//</Qt-Qt_5_9_1_Reference_Documentation-Qt_Core-C++_Classes-QCoreApplication-exec.assumption>

	} catch (const cutehmi::Dialogist::NoAdvertiserException & e) {
		CUTEHMI_CRITICAL("Dialog message: " << e.dialog()->text());
		if (!e.dialog()->informativeText().isEmpty())
			CUTEHMI_CRITICAL("Informative text: " << e.dialog()->informativeText());
		if (!e.dialog()->detailedText().isEmpty())
			CUTEHMI_CRITICAL("Detailed text: " << e.dialog()->detailedText());
		CUTEHMI_CRITICAL("Available buttons: " << e.dialog()->buttons());
	} catch (const QException & e) {
		CUTEHMI_CRITICAL(e.what());
	} catch (const std::exception & e) {
		CUTEHMI_CRITICAL(e.what());
	} catch (...) {
		CUTEHMI_CRITICAL("Caught unrecognized exception.");
		throw;
	}

	return  EXIT_FAILURE;

	//</Qt-Qt_5_7_0_Reference_Documentation-Threads_and_QObjects-QObject_Reentrancy-creating_QObjects_before_QApplication.assumption>
}

//(c)MP: Copyright © 2019, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
