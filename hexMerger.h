#ifndef HEXMERGER_H
#define HEXMERGER_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QMetaObject>
#include <QtWidgets/QPushButton>
#include <QtGui/QPainter>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QWidget>
#include <QtCore/QMimeType>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QCheckBox>
#include <QtGui/QPainterPath>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QObject>


class hintIcon : public QLabel
{
Q_OBJECT
public:
    hintIcon(QWidget * parent = 0);
    ~hintIcon(){}

signals:
    void clicked();

public slots:
    void hintIconClicked();

protected:
    void mouseReleaseEvent (QMouseEvent *event) ;
};

class hintWidget : public QWidget
{
Q_OBJECT
public:
    hintWidget(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
private:
    QPixmap *windowsIcon;
    QPixmap *appleIcon;
    QPixmap *linuxIcon;
    QRegion *windowsRegion;
    QRegion *appleRegion;
    QRegion *linuxRegion;
    void openBrowser(QString url);
};

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class hexMerger : public QMainWindow
{
Q_OBJECT

public:
	hexMerger();

protected:
	void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void openSketch();
    void openBootloader();
    void locateOutputFile();
	void about();
    void parseAvailableBoards();
	void mergeHex();
    void showSettings();
    void license();
    void boardChanged(QString);
    void revertDoNotAskAgainSetting();
    void setCustomBoards();

private:
    void createMenus();
    void createActions();
    void createStatusBar();
    void findAdditionalBoards();
    void pleaseRestart();
    QString parseBootloader(QString);

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QAction *openSketchAct;
    QAction *openBootloaderAct;
    QAction *outputFileAct;
    QAction *exitAct;
    QAction *settingsAct;
    QAction *customBoardsAct;
    QAction *aboutAct;
    QAction *licenseAct;
    QAction *mergeAct;
    QAction *donotaskagainRevertAct;

    QMenuBar *appMenuBar;

    QLabel *inputLineSketchLabel;
    QLabel *inputLineBootldrLabel;
    QLabel *outputLineLabel;
    QLabel *boardSelectLabel;

    QComboBox *arduinoBoardCombo;

    QCheckBox *donotaskagainCB;

    QLineEdit *sketchLine;
    QLineEdit *bootldrLine;
    QLineEdit *outputLine;

    QGroupBox *inputFiles;
    QGroupBox *outputGroupBox;
    QGroupBox *buttonsGroupBox;

    QPushButton *sketchBrowse;
    QPushButton *bootldrBrowse;
    QPushButton *outputBrowse;

    QPushButton *exitButton;
    QPushButton *settingsButton;
    QPushButton *mergeButton;

    hintIcon *hintLabel;
    QLabel *inactiveHintLabel;
};

class settingsWidget : public QWidget
{
Q_OBJECT

public:
    settingsWidget();
protected:
    void closeEvent(QCloseEvent *event);
    bool validateArduinoPath(QString pathName);
    bool validateDocumentPath(QString pathName);
    void settingsChanged();
private slots:
    void loadSettings();
    void applySettings();
    void saveSettings();
    void browseArduino();
    void browseIncludePath();
private:
    void pleaseRestart();
    QGroupBox *arduinoApplicationBox;
    QGroupBox *settingsButtonsGroupBox;
    QGroupBox *includePathGroupBox;
    QPushButton *arduinoApplicationBrowse;
    QPushButton *includePathBrowse;
    QLineEdit *arduinoApplicationLine;
    QLineEdit *includePathLine;
    QDir *dirObject;
    QFile *fileObject;
    QFileDialog *fileDialog;
    QPushButton *applySettingsBtn;
    QPushButton *closeSettingsBtn;
    QLabel *arduinoApplicationLabel;
    QLabel *arduinoApplicationHintWin;
    QLabel *arduinoApplicationHintMac;
    QLabel *arduinoApplicationHintLin;
    QLabel *includePathLabel;
    QLabel *WinLabel1;
    QLabel *MacLabel1;
    QLabel *LinuxLabel1;
    QLabel *WinLabel2;
    QLabel *MacLabel2;
    QLabel *LinuxLabel2;
    QLabel *includePathHintWin;
    QLabel *includePathHintMac;
    QLabel *includePathHintLin;
};

class licenseWidget : public QWidget
{
Q_OBJECT

public:
    licenseWidget();
    QLabel *topLabel;
    QTextEdit *textEdit;
    QPushButton *acceptButton;
};

#endif
