#ifndef MAIN_CPP
#define MAIN_CPP

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include "hexMerger.h"
#include "hexMerger.cpp"

int main(int argc, char **argv) {

	Q_INIT_RESOURCE(hexMerger);

    	QApplication app(argc, argv);

    	QTranslator translator;
    	QString locale = QLocale::system().name().section('_', 0, 0);
#ifdef Q_OS_MAC
	QDir topDir(QApplication::applicationDirPath());
	topDir.cdUp();
	topDir.cd("Resources");
    	translator.load(("hexMerger_" + locale + ".qm"), topDir.absolutePath() + QDir::separator() + "lang");
#else
    translator.load(("hexMerger_" + locale + ".qm"), (QApplication::applicationDirPath() + QDir::separator() + "lang"));
#endif
    	app.installTranslator(&translator);

	app.setWindowIcon(QIcon(":/appicon.png"));

    	app.setOrganizationName("HexMerger");
	app.setApplicationName("HexMerger");

#ifdef Q_OS_MAC
	app.setStyleSheet((topDir.absolutePath() + QDir::separator() + "styles/macStyles.css"));
#endif
#ifdef Q_OS_LINUX
	app.setStyleSheet((QApplication::applicationDirPath() + QDir::separator() + "styles/lnxStyles.css"));
#endif
#ifdef Q_OS_WIN
	app.setStyleSheet((QApplication::applicationDirPath() + QDir::separator() + "styles\\winStyles.css"));
#endif

	hexMerger mainWindow;
	
	mainWindow.show();

	return app.exec();

}

#endif
