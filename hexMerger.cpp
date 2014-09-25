#ifndef HEXMERGER_CPP
#define HEXMERGER_CPP

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets>
#include <QtGui/QCloseEvent>
#include <QtCore/QEvent>
#include <QtCore/QMetaObject>
#include <QtGui/QCursor>
#include "hexMerger.h"

QString ArduinoPath;
QString DocumentPath;
QString CustomBoards;   // user-defined boards.txt files
QStringList Boards;   // automagically detected boards.txt files
QSettings settings("HexMerger", "HexMerger");
bool SettingsDiffer = false;
hintWidget *showHint;
int xPos;
int yPos;

static const QString WindowsHelpURL = "http://youtu.be/sHHZ8enEXnw";
static const QString MacintoshHelpURL = "http://youtu.be/j2aVs7zskNs";
static const QString LinuxHelpURL = "http://youtu.be/cL5yy5cGGXo";

#ifdef Q_OS_WIN
static const QString BoardsTxtPath = "\\hardware\\arduino\\boards.txt";
static const QString BootloadersPath = "\\hardware\\arduino\\bootloaders\\";
static const QString ArduinoExecutable = "\\arduino.exe";
static const QString ArduinoBasePath = "\\hardware\\arduino\\";
#endif

#ifdef Q_OS_MAC
static const QString BoardsTxtPath = "/Contents/Resources/Java/hardware/arduino/boards.txt";
static const QString BootloadersPath = "/Contents/Resources/Java/hardware/arduino/bootloaders/";
static const QString ArduinoExecutable = "/Contents/MacOS/JavaApplicationStub";
static const QString ArduinoBasePath = "/Contents/Resources/Java/hardware/arduino/";
#endif

#ifdef Q_OS_LINUX
static const QString BoardsTxtPath = "/hardware/arduino/boards.txt";
static const QString BootloadersPath = "/hardware/arduino/bootloaders/";
static const QString ArduinoExecutable = "/arduino";
static const QString ArduinoBasePath = "/hardware/arduino/";
#endif

void hexMerger::closeEvent(QCloseEvent *event) {
        showHint->hide();
		event->accept();
}

void hexMerger::findAdditionalBoards() {
    if (settings.value("Preferences/ArduinoPath").toString().isEmpty()) { return; }
    if (settings.value("Preferences/DocumentsPath").toString().isEmpty()) { return; }
    QDirIterator dirItArduino((settings.value("Preferences/ArduinoPath").toString()), QDirIterator::Subdirectories);
    while (dirItArduino.hasNext()) {
        dirItArduino.next();
        if (QFileInfo(dirItArduino.filePath()).isFile()) {
            if (QFileInfo(dirItArduino.fileName()).fileName() == "boards.txt") {
                Boards << dirItArduino.filePath();
            }
        }
    }
    QDirIterator dirItDocs((settings.value("Preferences/DocumentsPath").toString()), QDirIterator::Subdirectories);
    while (dirItDocs.hasNext()) {
        dirItDocs.next();
        if (QFileInfo(dirItDocs.filePath()).isFile()) {
            if (QFileInfo(dirItDocs.fileName()).fileName() == "boards.txt") {
                Boards << dirItDocs.filePath();
            }
        }
    }
    if (!(CustomBoards.isEmpty())) {
        QStringList testCustomBoards = CustomBoards.split(";", QString::SkipEmptyParts);
        for (int i=0; i < testCustomBoards.length(); i++) {
            if (!(QFileInfo(testCustomBoards.at(i)).isFile())) { continue; }
            if (!(QFileInfo(testCustomBoards.at(i)).isReadable())) { continue; }
            if (!(QFileInfo(testCustomBoards.at(i)).suffix() == "txt")) { continue; }
            if (!(QFileInfo(testCustomBoards.at(i)).fileName() == "boards.txt")) { continue; }
            if (!(QFileInfo(testCustomBoards.at(i)).size() > 1)) { continue; }
            Boards << testCustomBoards.at(i);
        }
    }
}

void hexMerger::createActions(void) {
    openSketchAct = new QAction(QIcon(":/openicon.png"), tr("Open Arduino Sketch..."), this);
    connect(openSketchAct, SIGNAL(triggered()), this, SLOT(openSketch()));
    openBootloaderAct = new QAction(QIcon(":/openicon.png"), tr("Open Bootloader File..."), this);
    connect(openBootloaderAct, SIGNAL(triggered()), this, SLOT(openBootloader()));
    outputFileAct = new QAction(QIcon(":/saveicon.png"), tr("Set Output File..."), this);
    connect(outputFileAct, SIGNAL(triggered()), this, SLOT(locateOutputFile()));
    outputFileAct->setShortcuts(QKeySequence::SaveAs);
    mergeAct = new QAction(QIcon(":/runicon.png"), tr("Merge HEX Files"), this);
    connect(mergeAct, SIGNAL(triggered()), this, SLOT(mergeHex()));
    exitAct = new QAction(QIcon(":/quiticon.png"), tr("Quit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setToolTip(tr("Exit the Application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    settingsAct = new QAction(QIcon(":/settingsicon.png"), tr("Settings..."), this);
    settingsAct->setShortcuts(QKeySequence::Preferences);
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(showSettings()));
    aboutAct = new QAction(QIcon(":/infoicon.png"), tr("About..."), this);
    aboutAct->setShortcuts(QKeySequence::HelpContents);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    licenseAct = new QAction(QIcon(":/licenseicon.png"), tr("License..."), this);
    connect(licenseAct, SIGNAL(triggered()), this, SLOT(license()));
    donotaskagainRevertAct = new QAction(QIcon(":/dontaskicon.png"), tr("Do not ask again"), this);
    donotaskagainRevertAct->setCheckable(true);
    donotaskagainRevertAct->setChecked(true);
    connect(donotaskagainRevertAct, SIGNAL(changed()), this, SLOT(revertDoNotAskAgainSetting()));
    customBoardsAct = new QAction(QIcon(":/experticon.png"), tr("[Expert] Specify custom \"boards.txt\"..."), this);
    connect(customBoardsAct, SIGNAL(triggered()), this, SLOT(setCustomBoards()));
}

void hexMerger::createMenus(void) {
    fileMenu = appMenuBar->addMenu(tr("&File"));
    fileMenu->addAction(openSketchAct);
    fileMenu->addAction(openBootloaderAct);
    fileMenu->addAction(outputFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(mergeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    editMenu = appMenuBar->addMenu(tr("&Edit"));
    editMenu->addAction(settingsAct);
    appMenuBar->addSeparator();
    helpMenu = appMenuBar->addMenu(tr("&Help"));
    helpMenu->addAction(licenseAct);
    helpMenu->addAction(aboutAct);
    if (settings.value("BootloaderChange/DontAskAgain").toBool() == true) {
        editMenu->addAction(donotaskagainRevertAct);
    }
    editMenu->addAction(customBoardsAct);
}

void hexMerger::parseAvailableBoards() {
    QFile boards;
    QStringList boardsList;
    for(int i=0; i < Boards.length(); i++) {
        boards.setFileName(Boards.at(i));
        if (!boards.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        while (!boards.atEnd()) {
            QString boardsLine = boards.readLine();
            boardsLine.replace("\n", "");
            QRegExp rx("\\w.name=.+");
            int pos = rx.indexIn(boardsLine);
            if (pos > -1) {
                QString board = rx.cap(0);
                board = board.right(board.length()-7);
                boardsList << board;
            }
        }
        boards.close();
    }
    arduinoBoardCombo->insertItems(0, boardsList);
    bootldrLine->setText("");
}

void hexMerger::revertDoNotAskAgainSetting() {
    if (donotaskagainRevertAct->isChecked() == true) {
        settings.setValue("BootloaderChange/DontAskAgain", true);
    } else {
        settings.setValue("BootloaderChange/DontAskAgain", false);
    }
}

void hexMerger::pleaseRestart() {
    QMessageBox(QMessageBox::Information, "HexMerger", tr("The configuration changed. Please restart the Application in order to have the new settings take effect."), QMessageBox::Ok, this);
}

void settingsWidget::pleaseRestart() {
    QMessageBox(QMessageBox::Information, "HexMerger", tr("The configuration changed. Please restart the Application in order to have the new settings take effect."), QMessageBox::Ok, this);
}

QString hexMerger::parseBootloader(QString boardName) {
    QString path = settings.value("Preferences/ArduinoPath").toString();
    QString bootloaderPath;
    QString bootloaderFile;
    QFile boards;
    for (int i=0; i < Boards.length(); i++) {
        boards.setFileName(Boards.at(i));
        if (!boards.open(QIODevice::ReadOnly | QIODevice::Text))
            return "";

        QString searchString;
        QString boardShortName;

        while (!boards.atEnd()) {
            QString boardsLine = boards.readLine();
            boardsLine.replace("\n", "");
            if (boardsLine.right(boardsLine.length()-boardsLine.indexOf(".name=")-6) == boardName) {
                searchString = boardsLine;
                QRegExp rx("\\w.name=.+");
                int pos = rx.indexIn(boardsLine);
                boardShortName = boardsLine.left(pos+1);
            }
            if ((!searchString.isEmpty()) && (boardsLine.startsWith(boardShortName + ".bootloader.path="))) {
                bootloaderPath = boardsLine;
            } else if ((!searchString.isEmpty()) && (boardsLine.startsWith(boardShortName + ".bootloader.file="))) {
                bootloaderFile = boardsLine;
            }
        }
        boards.close();
        if (!(bootloaderFile.isEmpty())) {
            bootloaderPath.replace((boardShortName + ".bootloader.path="), "");
            bootloaderFile.replace((boardShortName + ".bootloader.file="), "");

	    QString tmpBootloaderFile = QFileInfo(boards.fileName()).absolutePath() + QDir::separator() + bootloaderPath + QDir::separator() + bootloaderFile;
	    if (QFile(tmpBootloaderFile).exists()) {
		bootloaderFile = tmpBootloaderFile;
	    } else {
		tmpBootloaderFile = QFileInfo(boards.fileName()).absolutePath() + QDir::separator() + "bootloaders" + QDir::separator() + bootloaderPath + QDir::separator() + bootloaderFile;
	    	if (QFile(tmpBootloaderFile).exists()) {
		  bootloaderFile = tmpBootloaderFile;
		} else {
		  bootloaderFile = "";
		}
	    }

            return bootloaderFile;
        }
    }
}

void hexMerger::createStatusBar() {
    statusBar()->showMessage((tr("Released under the GPLv2 by %1").arg("Tobias Vogel")), 2000);
}

void hexMerger::openBootloader() {
    QFileDialog openBootloaderDialog;
    QString path = settings.value("Preferences/ArduinoPath").toString();
    path += BootloadersPath;
    QString filename = openBootloaderDialog.getOpenFileName(this, tr("Open Bootloader File"), path, tr("HEX Files (*.HEX *.hex)"));
    if (filename.isEmpty()) {
        return;
    }
    if (QFile(filename).exists()) {
        bootldrLine->setText(filename);
    }
}

void hexMerger::openSketch() {
    QFileDialog openSketchDialog;
    QString path = settings.value("Preferences/DocumentsPath").toString();
    QString filename = openSketchDialog.getOpenFileName(this, tr("Open Sketch File"), path, tr("HEX Files (*.HEX *.hex)"));
    if (filename.isEmpty()) {
        return;
    }
    if (QFile(filename).exists()) {
        sketchLine->setText(filename);
    }
}

void hexMerger::locateOutputFile() {
    QFileDialog getSaveDialog;
    QString path = settings.value("Preferences/DocumentsPath").toString();
    QString filename = getSaveDialog.getSaveFileName(this, tr("Save Merged HEX File As..."), path, tr("HEX Files (*.hex *.HEX)"));
    if (filename.isEmpty()) {
        return;
    } else {
        if (filename.right(4).contains(".hex", Qt::CaseInsensitive)) {
            outputLine->setText(filename);
        } else {
            filename += ".hex";
            outputLine->setText(filename);
        }
    }
}

void hexMerger::boardChanged(QString boardName) {
    QString bootloaderPath = parseBootloader(boardName);
    if (bootldrLine->displayText().isEmpty()) {
        bootldrLine->setText(bootloaderPath);
    } else if (bootldrLine->displayText() != bootloaderPath) {
        if (settings.value("BootloaderChange/DontAskAgain").toBool() == true && settings.value("BootloaderChange/AlwaysChange").toBool() == true) {
            bootldrLine->setText(bootloaderPath);
        } else if (settings.value("BootloaderChange/DontAskAgain").toBool() == true && settings.value("BootloaderChange/AlwaysChange").toBool() == false) {
            statusBar()->showMessage(tr("Arduino Board Model and Bootloader File mismatch!"), 2000);
        } else {
            QMessageBox msgBox;
            msgBox.setText("HexMerger");
            msgBox.setIconPixmap(QPixmap(":/appicon.png"));
            msgBox.setInformativeText(tr("The Board selected below uses another Bootloader according to the Arduino configuration file.\n\nDo you want to change the Bootloader file accordingly?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            donotaskagainCB = new QCheckBox(tr("Dont ask me again"), this);
            donotaskagainCB->setTristate(false);
            msgBox.setCheckBox(donotaskagainCB);
            int ret = msgBox.exec();
            if (donotaskagainCB->checkState() == Qt::Checked) {
                settings.setValue("BootloaderChange/DontAskAgain", true);
                if (editMenu->actions().contains(donotaskagainRevertAct) == true) { donotaskagainRevertAct->setChecked(true); }
                if (ret == QMessageBox::Yes) {
                    settings.setValue("BootloaderChange/AlwaysChange", true);
                } else {
                    settings.setValue("BootloaderChange/AlwaysChange", false);
                }
                settings.sync();
            }
            if (ret == QMessageBox::Yes) {
                bootldrLine->setText(bootloaderPath);
            }
        }
    }
}

void hexMerger::about() {
    QMessageBox msgBox;
    msgBox.setText(tr("HexMerger"));
    msgBox.setIconPixmap(QPixmap(":/appicon.png"));
    msgBox.setInformativeText((tr("This application is intended to merge two HEX-Files:\n\n - one compiled Arduino-Sketch\n\n - one Arduino Bootloader\n\nThis application is provided \"AS IS\", without warranty of any kind. Check out \"License\" in the \"Help\"-Menu for more information.\n\nExtra credits go to the Oxygen Icons Team from which the Icons used in this Application are taken from.\nSee %1 for more information.\n\n%2 2014 by %3. Released under the GPLv2.").arg("http://www.oxygen-icons.org/").arg("\u00A9").arg("Tobias Vogel")));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void hexMerger::license() {
    licenseWidget *licenseDialog = new licenseWidget();
    licenseDialog->show();
}

licenseWidget::licenseWidget() {

    QFont standardFont;
    standardFont.setStyleHint(QFont::TypeWriter);
    standardFont.setPointSize(12);

    QFont smallFont;
    smallFont.setStyleHint(QFont::SansSerif);
    smallFont.setPointSize(9);

    QFont boldFont;
    boldFont.setStyleHint(QFont::SansSerif);
    boldFont.setPointSize(12);
    boldFont.setBold(true);

    setFixedSize(500, 470);
    topLabel = new QLabel(this);
    topLabel->setText(tr("This Application is licensed under the GNU General Public License v2"));
    topLabel->setGeometry(10, 5, 480, 30);
    topLabel->setFont(boldFont);
    topLabel->setAlignment(Qt::AlignCenter);
    QString licenseString;
    QFile licenseFile(":/gplv2.txt");
    if (!licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not open the license file.\nPlease find a copy of the license here:\n%1").arg("http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt"), QMessageBox::Ok, this);
        error.exec();
        return;
    }

    while (!licenseFile.atEnd()) {
        licenseString += licenseFile.readLine();
    }
    textEdit = new QTextEdit(this);
    textEdit->setGeometry(10, 40, 480, 380);
    textEdit->setText(licenseString);
    textEdit->setFont(standardFont);
    textEdit->setAlignment(Qt::AlignLeft);
    textEdit->setReadOnly(true);
    textEdit->setStyleSheet("text-align: left;");
    acceptButton = new QPushButton(this);
    acceptButton->setGeometry(150, 425, 200, 45);
    acceptButton->setText(tr("Accept"));
    acceptButton->setIcon(QIcon(":/okicon.png"));
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(close()));
}

void hexMerger::mergeHex() {

    QString path;

    QFile BootloaderFile(bootldrLine->displayText());
    {
    QFileInfo fileInfo(BootloaderFile);
    path = fileInfo.absoluteFilePath();
    #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(path);
        if (!(type.name() == "text/plain")) {
            QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not open/read Bootloader File.\nError: The Filetype is invalid."), QMessageBox::Ok, this);
            error.exec();
            return;
        }
    #endif
    }

    QFile SketchFile(sketchLine->displayText());
    {
    QFileInfo fileInfo(SketchFile);
    path = fileInfo.absoluteFilePath();
    #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(path);
        if (!(type.name() == "text/plain")) {
            QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not open/read Sketch-File.\nError: The Filetype is invalid."), QMessageBox::Ok, this);
            error.exec();
            return;
        }
    #endif
    }

    QFile OutputFile(outputLine->displayText());
    QFileInfo fileInfo(OutputFile);
    if (OutputFile.exists() && fileInfo.isWritable() == false) {
            QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not write Output-File.\nError: The File is not writable."), QMessageBox::Ok, this);
            error.exec();
            return;
    }


    if (!(BootloaderFile.open(QFile::ReadOnly))) {
        QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not open/read Bootloader-File.\nError: The File couldn't be opened."), QMessageBox::Ok, this);
        error.exec();
        return;
    }
    if (!(SketchFile.open(QFile::ReadOnly))) {
        QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not open/read Sketch-File.\nError: The File couldn't be opened."), QMessageBox::Ok, this);
        error.exec();
        return;
    }
    if (!(OutputFile.open(QFile::WriteOnly))) {
        QMessageBox error(QMessageBox::Critical, "HexMerger", tr("Could not open/write Output-File.\nError: The File is not writable."), QMessageBox::Ok, this);
        error.exec();
        return;
    }

    QTextStream BootloaderStream(&BootloaderFile);
    QTextStream SketchStream(&SketchFile);
    QTextStream OutputStream(&OutputFile);
    QString Line;

    do {
        Line = SketchStream.readLine();
        if (!Line.contains(":00000001FF")) {
            if (!(Line.isEmpty())) {
                OutputStream << Line << endl;
            }
        }
    }  while (!Line.isNull());

    do {
        Line = BootloaderStream.readLine();
        if (!(Line.isEmpty())) {
            OutputStream << Line << endl;
        }
    } while (!Line.isNull());

    BootloaderStream.flush();
    SketchStream.flush();
    OutputStream.flush();

    BootloaderFile.close();
    SketchFile.close();
    OutputFile.flush();
    OutputFile.close();

    QMessageBox successMsg(QMessageBox::NoIcon, "HexMerger", (tr("Merging complete!\n\nPlease find your Merged HEX-File under:\n") + QFileInfo(OutputFile).absoluteFilePath()), QMessageBox::Ok, this);
    successMsg.setIconPixmap(QPixmap(":/appicon.png"));
    successMsg.exec();
}

hintIcon::hintIcon(QWidget *parent) : QLabel(parent) {
    connect(this, SIGNAL(clicked()), this, SLOT(hintIconClicked()));
}

void hintIcon::hintIconClicked() {
    if (showHint->isVisible() == false) {
    showHint->move(xPos, (yPos - 270));
    showHint->show();
    showHint->installEventFilter(showHint);
    } else {
    showHint->hide();
    }
}

void hintIcon::mouseReleaseEvent (QMouseEvent *event) {
    xPos = event->globalX();
    yPos = event->globalY();
    emit clicked();
}

hexMerger::hexMerger() {

    if (settings.value("Preferences/CustomBoards").toString().isEmpty() == false) {
        CustomBoards = settings.value("Preferences/CustomBoards").toString();
    }

    QFont standardFont;
    standardFont.setStyleHint(QFont::SansSerif);
    standardFont.setPointSize(12);

    QFont mediumFont;
    mediumFont.setStyleHint(QFont::SansSerif);
    mediumFont.setPointSize(11);

    QFont smallFont;
    smallFont.setStyleHint(QFont::SansSerif);
    smallFont.setPointSize(9);

    setFixedSize(400,460);
	setWindowTitle("HexMerger");
    appMenuBar = new QMenuBar(0);
    createActions();
    createMenus();
    createStatusBar();
    setMenuBar(appMenuBar);
    appMenuBar->setNativeMenuBar(false);
    inputFiles = new QGroupBox(this);
    outputGroupBox = new QGroupBox(this);
    inputLineSketchLabel = new QLabel(inputFiles);
    inputLineBootldrLabel = new QLabel(inputFiles);
    outputLineLabel = new QLabel(outputGroupBox);
    inputFiles->setTitle(tr("Input Files"));
    outputGroupBox->setTitle(tr("Output File"));
    inputFiles->setGeometry(10, 35, 380, 160);
    outputGroupBox->setGeometry(10, 205, 380, 133);
    sketchLine = new QLineEdit(inputFiles);
    sketchLine->setGeometry(10, 55, 240, 22);
    sketchLine->setFont(mediumFont);
    bootldrLine = new QLineEdit(inputFiles);
    bootldrLine->setGeometry(10, 120, 240, 22);
    bootldrLine->setFont(mediumFont);
    outputLine = new QLineEdit(outputGroupBox);
    outputLine->setGeometry(10, 55, 240, 22);
    outputLine->setFont(mediumFont);
    sketchBrowse = new QPushButton(inputFiles);
    sketchBrowse->setGeometry(250, 49, 120, 34);
    bootldrBrowse = new QPushButton(inputFiles);
    bootldrBrowse->setGeometry(250, 114, 120, 34);
    outputBrowse = new QPushButton(outputGroupBox);
    outputBrowse->setGeometry(250, 49, 120, 34);
    inputLineSketchLabel->setText(tr("Arduino Sketch (HEX-File):"));
    inputLineBootldrLabel->setText(tr("Bootloader (HEX-File):"));
    outputLineLabel->setText(tr("Output HEX-File:"));
    inputLineSketchLabel->setGeometry(10, 32, 200, 18);
    inputLineBootldrLabel->setGeometry(10, 97, 200, 18);
    outputLineLabel->setGeometry(10, 32, 200, 18);
    bootldrBrowse->setText(tr("Browse..."));
    sketchBrowse->setText(tr("Browse..."));
    outputBrowse->setText(tr("Browse..."));
    bootldrBrowse->setIcon(QIcon(":/openicon.png"));
    sketchBrowse->setIcon(QIcon(":/openicon.png"));
    outputBrowse->setIcon(QIcon(":/saveicon.png"));
    bootldrLine->setPlaceholderText(tr("Full path to Bootloader HEX-File"));
    sketchLine->setPlaceholderText(tr("Full path to Arduino Sketch HEX-File"));
    outputLine->setPlaceholderText(tr("Full path to output HEX-File"));
    sketchBrowse->setFocusPolicy(Qt::StrongFocus);
    bootldrBrowse->setFocusPolicy(Qt::StrongFocus);
    outputBrowse->setFocusPolicy(Qt::StrongFocus);
    setTabOrder(sketchLine, sketchBrowse);
    setTabOrder(sketchBrowse, bootldrLine);
    setTabOrder(bootldrLine, bootldrBrowse);
    setTabOrder(bootldrBrowse, outputLine);
    setTabOrder(outputLine, outputBrowse);
    buttonsGroupBox = new QGroupBox(this);
    buttonsGroupBox->setGeometry(10, 360, 380, 65);
    buttonsGroupBox->setTitle("");
    exitButton = new QPushButton(buttonsGroupBox);
    exitButton->setFocusPolicy(Qt::StrongFocus);
    settingsButton = new QPushButton(buttonsGroupBox);
    settingsButton->setFocusPolicy(Qt::StrongFocus);
    mergeButton = new QPushButton(buttonsGroupBox);
    mergeButton->setFocusPolicy(Qt::StrongFocus);
    exitButton->setText(tr("Quit"));
    settingsButton->setText(tr("Settings"));
    mergeButton->setText(tr("Merge!"));
    exitButton->setIcon(QIcon(":/quiticon.png"));
    settingsButton->setIcon(QIcon(":/settingsicon.png"));
    mergeButton->setIcon(QIcon(":/runicon.png"));
    exitButton->setGeometry(5, 12, 120, 46);
    settingsButton->setGeometry(140, 12, 120, 46);
    mergeButton->setGeometry(255, 12, 120, 46);
    boardSelectLabel = new QLabel(outputGroupBox);
    boardSelectLabel->setGeometry(10, 97, 120, 24);
    boardSelectLabel->setText(tr("Select Board:"));
    arduinoBoardCombo = new QComboBox(outputGroupBox);
    arduinoBoardCombo->setGeometry(140, 97, 227, 26);
    arduinoBoardCombo->setFocusPolicy(Qt::StrongFocus);
    connect(arduinoBoardCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(boardChanged(QString)));
    setTabOrder(outputBrowse, arduinoBoardCombo);
    setTabOrder(arduinoBoardCombo, exitButton);
    setTabOrder(exitButton, settingsButton);
    setTabOrder(settingsButton, mergeButton);
    connect(exitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(showSettings()));
    connect(bootldrBrowse, SIGNAL(clicked()), this, SLOT(openBootloader()));
    connect(sketchBrowse, SIGNAL(clicked()), this, SLOT(openSketch()));
    connect(outputBrowse, SIGNAL(clicked()), this, SLOT(locateOutputFile()));
    connect(mergeButton, SIGNAL(clicked()), this, SLOT(mergeHex()));
    inactiveHintLabel = new QLabel(inputFiles);
    inactiveHintLabel->setPixmap(QPixmap(":/noideaicon.png"));
    inactiveHintLabel->setScaledContents(true);
    inactiveHintLabel->setGeometry(210, 22, 28, 28);
    inactiveHintLabel->setVisible(false);
    inactiveHintLabel->setObjectName("InactiveHintLabel");
    hintLabel = new hintIcon(inputFiles);
    hintLabel->setPixmap(QPixmap(":/ideaicon.png"));
    hintLabel->setScaledContents(true);
    hintLabel->setGeometry(210, 22, 28, 28);
    hintLabel->setObjectName("ActiveHintLabel");
    sketchLine->installEventFilter(this);
    showHint = new hintWidget();
    showHint->hide();
    QString path = settings.value("Preferences/ArduinoPath").toString();
    if (path.isEmpty()) {
        return;
    }
    Boards << (path + BoardsTxtPath);
    findAdditionalBoards();
    parseAvailableBoards();
}

void hexMerger::setCustomBoards() {
    QString previousSetting = settings.value("Preferences/CustomBoards").toString();
    QInputDialog boardsDialog(this);
    if (previousSetting.isEmpty() == false) {
        boardsDialog.setTextValue(previousSetting);
    }
    boardsDialog.setLabelText(tr("Specify custom locations of \"boards.txt\"-Files that are not automatically detected below.\nUse a semicolon ( \";\" ) to seperate multiple values."));
    boardsDialog.setInputMode(QInputDialog::TextInput);
    boardsDialog.setWindowTitle("HexMerger");
    CustomBoards = boardsDialog.exec();
    settings.setValue("Preferences/CustomBoards", CustomBoards);
    settings.sync();
    pleaseRestart();
}

bool hexMerger::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::FocusIn) {
        this->inactiveHintLabel->setVisible(false);
        this->hintLabel->show();
    } else if (event->type() == QEvent::FocusOut && showHint->isVisible() == false) {
        this->inactiveHintLabel->setVisible(true);
        this->hintLabel->hide();
    }
}

void hexMerger::showSettings() {
    settingsWidget *settingsDialog = new settingsWidget();
    settingsDialog->show();
}

settingsWidget::settingsWidget() {

    QFont standardFont;
    standardFont.setStyleHint(QFont::SansSerif);
    standardFont.setPointSize(12);

    QFont mediumFont;
    mediumFont.setStyleHint(QFont::SansSerif);
    mediumFont.setPointSize(11);

    QFont smallFont;
    smallFont.setStyleHint(QFont::SansSerif);
    smallFont.setPointSize(9);

    setFixedSize(500, 360);
    setWindowTitle(tr("Settings"));
    arduinoApplicationBox = new QGroupBox(this);
    arduinoApplicationBox->setGeometry(10, 15, 480, 125);
    arduinoApplicationBox->setTitle(tr("Arduino Executable Path"));
    settingsButtonsGroupBox = new QGroupBox(this);
    settingsButtonsGroupBox->setGeometry(165, 295, 325, 50);
    includePathGroupBox = new QGroupBox(this);
    includePathGroupBox->setGeometry(10, 150, 480, 125);
    includePathGroupBox->setTitle(tr("Arduino Document Path"));
    arduinoApplicationBrowse = new QPushButton(arduinoApplicationBox);
    arduinoApplicationBrowse->setGeometry(355, 44, 120, 34);
    arduinoApplicationBrowse->setText(tr("Browse..."));
    arduinoApplicationBrowse->setFont(standardFont);
    arduinoApplicationBrowse->setIcon(QIcon(":/openicon.png"));
    includePathBrowse = new QPushButton(includePathGroupBox);
    includePathBrowse->setGeometry(355, 44, 120, 34);
    includePathBrowse->setText(tr("Browse..."));
    includePathBrowse->setFont(standardFont);
    includePathBrowse->setIcon(QIcon(":/openicon.png"));
    arduinoApplicationLine = new QLineEdit(arduinoApplicationBox);
    arduinoApplicationLine->setGeometry(10, 50, 345, 22);
    arduinoApplicationLine->setPlaceholderText(tr("Full Path to Arduino Executable"));
    arduinoApplicationLine->setFont(mediumFont);
    includePathLine = new QLineEdit(includePathGroupBox);
    includePathLine->setGeometry(10, 50, 345, 22);
    includePathLine->setPlaceholderText(tr("Full Path to your Arduino Documents"));
    includePathLine->setFont(mediumFont);
    dirObject = new QDir();
    fileObject = new QFile();
    applySettingsBtn = new QPushButton(settingsButtonsGroupBox);
    applySettingsBtn->setIcon(QIcon(":/okicon.png"));
    applySettingsBtn->setText(tr("&Apply Settings"));
    applySettingsBtn->setFont(standardFont);
    applySettingsBtn->setGeometry(5, 5, 160, 44);
    closeSettingsBtn = new QPushButton(settingsButtonsGroupBox);
    closeSettingsBtn->setIcon(QIcon(":/closeicon.png"));
    closeSettingsBtn->setText(tr("&Close Window"));
    closeSettingsBtn->setFont(standardFont);
    closeSettingsBtn->setGeometry(160, 5, 160, 44);
    arduinoApplicationBrowse->setFocusPolicy(Qt::StrongFocus);
    includePathBrowse->setFocusPolicy(Qt::StrongFocus);
    applySettingsBtn->setFocusPolicy(Qt::StrongFocus);
    closeSettingsBtn->setFocusPolicy(Qt::StrongFocus);
    arduinoApplicationLabel = new QLabel(arduinoApplicationBox);
    arduinoApplicationLabel->setGeometry(10, 30, 340, 18);
    arduinoApplicationLabel->setText(tr("The Path of the Arduino executable"));
    arduinoApplicationLabel->setFont(standardFont);
    WinLabel1 = new QLabel(arduinoApplicationBox);
    WinLabel1->setGeometry(10, 76, 45, 13);
    WinLabel1->setFont(smallFont);
    WinLabel1->setText(tr("Windows:"));
    MacLabel1 = new QLabel(arduinoApplicationBox);
    MacLabel1->setGeometry(10, 89, 45, 13);
    MacLabel1->setFont(smallFont);
    MacLabel1->setText(tr("Mac OS:"));
    LinuxLabel1 = new QLabel(arduinoApplicationBox);
    LinuxLabel1->setGeometry(10, 102, 45, 13);
    LinuxLabel1->setFont(smallFont);
    LinuxLabel1->setText(tr("Linux: "));
    arduinoApplicationHintWin = new QLabel(arduinoApplicationBox);
    arduinoApplicationHintWin->setGeometry(60, 76, 380, 13);
    arduinoApplicationHintWin->setText(tr("Usually C:\\Program Files (x86)\\Arduino\\"));
    arduinoApplicationHintWin->setFont(smallFont);
    arduinoApplicationHintMac = new QLabel(arduinoApplicationBox);
    arduinoApplicationHintMac->setGeometry(60, 89, 380, 13);
    arduinoApplicationHintMac->setText(tr("Usually /Applications/Arduino.app"));
    arduinoApplicationHintMac->setFont(smallFont);
    arduinoApplicationHintLin = new QLabel(arduinoApplicationBox);
    arduinoApplicationHintLin->setGeometry(60, 102, 380, 13);
    arduinoApplicationHintLin->setText(tr("If installed in your Home-Directory then usually $HOME/arduino-x.x.x/"));
    arduinoApplicationHintLin->setFont(smallFont);
    includePathLabel = new QLabel(includePathGroupBox);
    includePathLabel->setGeometry(10, 30, 340, 18);
    includePathLabel->setText(tr("The Path where Arduino stores your Sketches"));
    includePathLabel->setFont(standardFont);
    WinLabel2 = new QLabel(includePathGroupBox);
    WinLabel2->setGeometry(10, 76, 45, 13);
    WinLabel2->setFont(smallFont);
    WinLabel2->setText(tr("Windows:"));
    MacLabel2 = new QLabel(includePathGroupBox);
    MacLabel2->setGeometry(10, 89, 45, 13);
    MacLabel2->setFont(smallFont);
    MacLabel2->setText(tr("Mac OS:"));
    LinuxLabel2 = new QLabel(includePathGroupBox);
    LinuxLabel2->setGeometry(10, 102, 45, 13);
    LinuxLabel2->setFont(smallFont);
    LinuxLabel2->setText(tr("Linux: "));
    includePathHintWin = new QLabel(includePathGroupBox);
    includePathHintWin->setGeometry(60, 76, 380, 13);
    includePathHintWin->setText(tr("Usually C:\\User\\$username$\\Documents\\Arduino"));
    includePathHintWin->setFont(smallFont);
    includePathHintMac = new QLabel(includePathGroupBox);
    includePathHintMac->setGeometry(60, 89, 380, 13);
    includePathHintMac->setText(tr("Usually /User/$username$/Documents/Arduino"));
    includePathHintMac->setFont(smallFont);
    includePathHintLin = new QLabel(includePathGroupBox);
    includePathHintLin->setGeometry(60, 102, 380, 13);
    includePathHintLin->setText(tr("Usually $HOME/Documents/Arduino"));
    includePathHintLin->setFont(smallFont);
    setTabOrder(arduinoApplicationLine, arduinoApplicationBrowse);
    setTabOrder(arduinoApplicationBrowse, includePathLine);
    setTabOrder(includePathLine, includePathBrowse);
    setTabOrder(includePathBrowse, applySettingsBtn);
    setTabOrder(applySettingsBtn, closeSettingsBtn);
    connect(closeSettingsBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(arduinoApplicationBrowse, SIGNAL(clicked()), this, SLOT(browseArduino()));
    connect(includePathBrowse, SIGNAL(clicked()), this, SLOT(browseIncludePath()));
    connect(applySettingsBtn, SIGNAL(clicked()), this, SLOT(applySettings()));
    loadSettings();
}

void settingsWidget::closeEvent(QCloseEvent *event) {
    settingsChanged();
    if (SettingsDiffer) {
        QMessageBox msgBox;
        msgBox.setText(tr("The settings changed."));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int answer = msgBox.exec();
        if(answer == QMessageBox::Discard) {
            event->accept();
        } else if (answer == QMessageBox::Save) {
            saveSettings();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void settingsWidget::applySettings() {
    saveSettings();
    pleaseRestart();
    close();
}

bool settingsWidget::validateArduinoPath(QString pathName) {
    fileObject->setFileName(pathName + ArduinoExecutable);
    if (fileObject->exists() == false) {
        return false;
    } else if (pathName.contains("arduino", Qt::CaseInsensitive) == false) {
        return false;
    } else {
        fileObject->setFileName(pathName + BoardsTxtPath);
        if (fileObject->exists()) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool settingsWidget::validateDocumentPath(QString pathName) {
    fileObject->setFileName(pathName);
    if (fileObject->exists() == false) {
        return false;
    } else if (pathName.contains("arduino", Qt::CaseInsensitive) == false) {
        return false;
    }
    fileObject->setFileName(pathName + "libraries");
    if (fileObject->exists() == true) { return true; }
    fileObject->setFileName(pathName + "hardware");
    if (fileObject->exists() == true) { return true; }
    return true;
}

void settingsWidget::browseArduino() {
    fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
#ifdef Q_OS_MAC
    fileDialog->setFileMode(QFileDialog::Directory);
    ArduinoPath = fileDialog->getOpenFileName(this, tr("Select Arduino Executable"), "/Applications", tr("Applications (*.app)"));
#endif
#ifdef Q_OS_WIN
    QByteArray envArray = qgetenv("%ProgramFiles%");
    QString WinProgramPath;
    WinProgramPath = envArray.constData();
    fileDialog->setFileMode(QFileDialog::Directory);
    ArduinoPath = fileDialog->getExistingDirectory(this, tr("Select Arduino Executable Path"), WinProgramPath, QFileDialog::ShowDirsOnly);
#endif
#ifdef Q_OS_LINUX
    fileDialog->setFileMode(QFileDialog::Directory);
    ArduinoPath = fileDialog->getExistingDirectory(this, tr("Select Arduino Executable Path"), QDir::homePath(), QFileDialog::ShowDirsOnly);
#endif
    if (validateArduinoPath(ArduinoPath)) {
        arduinoApplicationLine->setText(ArduinoPath);
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("The Arduino-Executable you specified could not be verified."));
        msgBox.setInformativeText(tr("Do you want to use the Path specified nonetheless?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int answer = msgBox.exec();
        if(answer == QMessageBox::Yes) {
            arduinoApplicationLine->setText(ArduinoPath);
        } else {
            arduinoApplicationLine->setText("");
        }
    }
    delete fileDialog;
}

void settingsWidget::browseIncludePath() {
    fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    DocumentPath = fileDialog->getExistingDirectory(this, tr("Select Arduino Sketch-Directory"), QDir::homePath());
    if (validateDocumentPath(DocumentPath)) {
        includePathLine->setText(DocumentPath);
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("The Document-Path you specified could not be verified."));
        msgBox.setInformativeText(tr("Do you want to use the Path specified nonetheless?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int answer = msgBox.exec();
        if(answer == QMessageBox::Yes) {
            includePathLine->setText(DocumentPath);
        } else {
            includePathLine->setText("");
        }
    }
    delete fileDialog;
}

void settingsWidget::loadSettings() {
    QString SettingString;
    SettingString = settings.value("Preferences/ArduinoPath").toString();
    if (!(SettingString.isEmpty())) {
        arduinoApplicationLine->setText(SettingString);
    }
    SettingString = settings.value("Preferences/DocumentsPath").toString();
    if (!(SettingString.isEmpty())) {
        includePathLine->setText(SettingString);
    }
}

void settingsWidget::saveSettings() {
    settings.setValue("Preferences/ArduinoPath", arduinoApplicationLine->displayText());
    settings.setValue("Preferences/DocumentsPath", includePathLine->displayText());
    settings.sync();
}

void settingsWidget::settingsChanged() {
    bool checkArduinoPath;
    bool checkDocumentsPath;
    if (settings.value("Preferences/ArduinoPath").toString() == arduinoApplicationLine->displayText()) { checkArduinoPath = true; } else { checkArduinoPath = false; }
    if (settings.value("Preferences/DocumentsPath").toString() == includePathLine->displayText()) { checkDocumentsPath = true; } else { checkDocumentsPath = false; }
    if (checkDocumentsPath && checkArduinoPath) { SettingsDiffer = false; } else { SettingsDiffer = true; }
}


void hintWidget::paintEvent(QPaintEvent *) {

    windowsIcon = new QPixmap(":/winicon.png");
    appleIcon = new QPixmap(":/macicon.png");
    linuxIcon = new QPixmap(":/linuxicon.png");

    QFont standardFont;
    standardFont.setStyleHint(QFont::SansSerif);
    standardFont.setPointSize(12);

    QFont mediumFont;
    mediumFont.setStyleHint(QFont::SansSerif);
    mediumFont.setPointSize(11);

    QFont smallFont;
    smallFont.setStyleHint(QFont::SansSerif);
    smallFont.setPointSize(9);
    QPainterPath OuterPath;
    QPainter Painter(this);
    OuterPath.setFillRule(Qt::WindingFill);

    const QRegion ellipseTopLeft(60, 140, 24, 24, QRegion::Ellipse);
    const QRegion ellipseTopRight(266, 140, 24, 24, QRegion::Ellipse);
    const QRegion ellipseBottomLeft(60, 236, 24, 24, QRegion::Ellipse);
    const QRegion ellipseBottomRight(266, 236, 24, 24, QRegion::Ellipse);

    const QRegion verticalRectangle(72, 140, 206, 120, QRegion::Rectangle);
    const QRegion horizontalRectangle(60, 152, 230, 96, QRegion::Rectangle);

    QPolygon arcPolygon;
    arcPolygon << QPoint(60, 180) << QPoint(60, 222) << QPoint(0, 268);

    QRegion widgetRegion(arcPolygon);
    widgetRegion = widgetRegion.united(horizontalRectangle);
    widgetRegion = widgetRegion.united(verticalRectangle);
    widgetRegion = widgetRegion.united(ellipseTopLeft);
    widgetRegion = widgetRegion.united(ellipseTopRight);
    widgetRegion = widgetRegion.united(ellipseBottomLeft);
    widgetRegion = widgetRegion.united(ellipseBottomRight);

    OuterPath.addRegion(widgetRegion);

    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.fillPath(OuterPath, QColor(255,255,127));
    Painter.strokePath(OuterPath.simplified(), QPen(Qt::black, 1));
    this->setMask(widgetRegion);

    Painter.setPen(QPen(Qt::black, 1));
    Painter.setFont(mediumFont);
    QTextOption blockText;
    blockText.setAlignment(Qt::AlignLeft);
    blockText.setWrapMode(QTextOption::WordWrap);
    Painter.drawText(QRectF(75, 150, 210, 30), tr("Need some help to find the your Arduino Sketch HEX-File?"), blockText);
    Painter.drawText(QRectF(75, 180, 210, 30), tr("Click on the corresponding Logo of your Operating System below."), blockText);

    QBrush ellipseBrush;
    ellipseBrush.setColor(QColor(255, 255, 221));
    ellipseBrush.setStyle(Qt::SolidPattern);

    QPainterPath innerEllipseOne;
    QPainterPath innerEllipseTwo;
    QPainterPath innerEllipseThree;

    innerEllipseOne.addEllipse(QPoint(115, 232), 20, 20);
    innerEllipseTwo.addEllipse(QPoint(175, 232), 20, 20);
    innerEllipseThree.addEllipse(QPoint(235, 232), 20, 20);

    Painter.fillPath(innerEllipseOne, ellipseBrush);
    Painter.fillPath(innerEllipseTwo, ellipseBrush);
    Painter.fillPath(innerEllipseThree, ellipseBrush);

    Painter.drawPixmap(99, 216, 32, 32, *windowsIcon);
    Painter.drawPixmap(159, 216, 32, 32, *appleIcon);
    Painter.drawPixmap(219, 216, 32, 32, *linuxIcon);
}

hintWidget::hintWidget(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
    windowsRegion = new QRegion(99, 216, 32, 32, QRegion::Ellipse);
    appleRegion = new QRegion(159, 216, 32, 32, QRegion::Ellipse);
    linuxRegion = new QRegion(219, 216, 32, 32, QRegion::Ellipse);

    this->setFixedSize(290, 268);
    this->repaint();
    this->show();
    this->activateWindow();
    this->setWindowState(Qt::WindowActive);
    this->installEventFilter(this);
    this->raise();
    this->setMouseTracking(true);
}

bool hintWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::WindowDeactivate) {
        if (showHint->isVisible()) {
                QApplication::restoreOverrideCursor();
                showHint->hide();
                this->removeEventFilter(this);
        }
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint mousePoint(mouseEvent->x(), mouseEvent->y());
        if (windowsRegion->contains(mousePoint) == true || appleRegion->contains(mousePoint) == true || linuxRegion->contains(mousePoint) == true) {
            QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
        } else {
            QApplication::restoreOverrideCursor();
        }
    }
}

void hintWidget::mouseReleaseEvent(QMouseEvent *event) {
    QPoint mousePoint(event->x(), event->y());
    if (windowsRegion->contains(mousePoint)) {
        openBrowser(WindowsHelpURL);
    } else if (appleRegion->contains(mousePoint)) {
        openBrowser(MacintoshHelpURL);
    } else if (linuxRegion->contains(mousePoint)) {
        openBrowser(LinuxHelpURL);
    } else {
        QApplication::restoreOverrideCursor();
        this->close();
    }
}

void hintWidget::openBrowser(QString url) {
    showHint->hide();
    QClipboard *clipboard = QApplication::clipboard();
    QApplication::restoreOverrideCursor();
    if (QDesktopServices::openUrl(QUrl(url)) == false) {
        QMessageBox browserMsg(QMessageBox::Critical, "HexMerger", tr("Sorry, couldn't find the System Default Webbrowser.\n\nPlease Copy & Paste the Url below manually into any Webbrowser of your choice.\n"), QMessageBox::Ok, this);
        browserMsg.setInformativeText(url + "\n");
        browserMsg.addButton(tr("Copy to Clipboard"), QMessageBox::HelpRole);
        int ret = browserMsg.exec();
        if (ret != QMessageBox::Ok) {
            clipboard->setText(url);
            QMessageBox copied(QMessageBox::Information, "HexMerger", tr("Url Copied to Clipboard Successfully!"), QMessageBox::Ok, this);
            copied.exec();
        }
    }
    QApplication::restoreOverrideCursor();
}

#endif
