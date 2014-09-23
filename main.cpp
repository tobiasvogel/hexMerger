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
#elif
    translator.load(("hexMerger_" + locale + ".qm"), (QApplication::applicationDirPath() + QDir::separator() + "lang"));
#endif
    app.installTranslator(&translator);

	app.setWindowIcon(QIcon(":/appicon.png"));

    app.setOrganizationName("HexMerger");
	app.setApplicationName("HexMerger");

	hexMerger mainWindow;
	
	mainWindow.show();

	return app.exec();

}

#endif
