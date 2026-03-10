/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *textGroup;
    QLabel *label;
    QLineEdit *textInput;
    QPushButton *showTextButton;
    QPushButton *clearBoardButton;
    QGroupBox *windowGroup;
    QPushButton *showBoardButton;
    QPushButton *fullscreenButton;
    QCheckBox *alwaysOnTopCheck;
    QPushButton *toggleTickerButton;
    QPushButton *toggleWeatherButton;
    QPushButton *toggleClockButton;
    QPushButton *toggleLogoButton;
    QGroupBox *quickGroup;
    QPushButton *quickNewsButton;
    QPushButton *quickWeatherButton;
    QPushButton *quickTimeButton;
    QPushButton *quickLogoButton;
    QWidget *tab_2;
    QGroupBox *colorGroup;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *setCustomColorButton;
    QPushButton *setRedBackgroundButton;
    QPushButton *setBlueBackgroundButton;
    QPushButton *setGreenBackgroundButton;
    QPushButton *setBlackBackgroundButton;
    QGroupBox *sizeGroup;
    QRadioButton *textSmall;
    QRadioButton *textMedium;
    QRadioButton *textHuge;
    QRadioButton *textLarge;
    QGroupBox *styleGroup;
    QComboBox *fontFamilyCombo;
    QCheckBox *boldTextCheck;
    QCheckBox *italicTextCheck;
    QWidget *tab_3;
    QWidget *tab_4;
    QGroupBox *groupBox;
    QPushButton *saveDesignButton;
    QPushButton *loadDesignButton;
    QPushButton *resetDesignButton;
    QPushButton *addElement;
    QPushButton *removeElement;
    QGroupBox *groupBox_2;
    QPushButton *clearDisegner;
    QPushButton *dragStartPosition;
    QPushButton *previewDesignButton;
    QPushButton *applyDesignButton;
    QPushButton *resetBoardButton;
    QPushButton *setLogoImageButton;
    QPushButton *loadConfigButton;
    QPushButton *saveConfigButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(890, 757);
        MainWindow->setMinimumSize(QSize(500, 600));
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: #f0f0f0;\n"
"}\n"
"\n"
"QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px 0 5px;\n"
"    color: #2c3e50;\n"
"}\n"
"\n"
"QPushButton {\n"
"    background-color: #3498db;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"    min-height: 30px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #2980b9;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #1c5a8a;\n"
"}\n"
"\n"
"QPushButton[text^=\"\360\237\223\260\"], \n"
"QPushButton[text^=\"\342\230\200\357\270\217\"], \n"
"QPushButton[text^=\"\360\237\225\220\"], \n"
"QPushButton[text^=\"\360\237\217\242\"] {\n"
"    background-color: #27ae60;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
""
                        "QPushButton[text^=\"\360\237\223\260\"]:hover,\n"
"QPushButton[text^=\"\342\230\200\357\270\217\"]:hover,\n"
"QPushButton[text^=\"\360\237\225\220\"]:hover,\n"
"QPushButton[text^=\"\360\237\217\242\"]:hover {\n"
"    background-color: #229954;\n"
"}\n"
"\n"
"QLineEdit {\n"
"    padding: 8px;\n"
"    border: 2px solid #bdc3c7;\n"
"    border-radius: 4px;\n"
"    background-color: white;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border-color: #3498db;\n"
"}\n"
"\n"
"QTabWidget::pane {\n"
"    border: 2px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background-color: #e0e0e0;\n"
"    padding: 10px 20px;\n"
"    margin-right: 2px;\n"
"    border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5px;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background-color: white;\n"
"    border-bottom: 3px solid #3498db;\n"
"}\n"
"\n"
"QTabBar::tab:hover {\n"
"    background-color: #d0d0d0;\n"
"}\n"
"\n"
"QRadioButton, QCheckBo"
                        "x {\n"
"    spacing: 5px;\n"
"    color: #2c3e50;\n"
"}\n"
"\n"
"QRadioButton::indicator, QCheckBox::indicator {\n"
"    width: 18px;\n"
"    height: 18px;\n"
"}\n"
"\n"
"QComboBox {\n"
"    padding: 5px;\n"
"    border: 2px solid #bdc3c7;\n"
"    border-radius: 4px;\n"
"    background-color: white;\n"
"    min-height: 25px;\n"
"}\n"
"\n"
"QComboBox:hover {\n"
"    border-color: #3498db;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #2c3e50;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(-4, -1, 801, 751));
        tabWidget->setMinimumSize(QSize(500, 600));
        tab = new QWidget();
        tab->setObjectName("tab");
        verticalLayout_3 = new QVBoxLayout(tab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        textGroup = new QGroupBox(tab);
        textGroup->setObjectName("textGroup");
        label = new QLabel(textGroup);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 15, 201, 71));
        textInput = new QLineEdit(textGroup);
        textInput->setObjectName("textInput");
        textInput->setGeometry(QRect(190, 30, 351, 41));
        showTextButton = new QPushButton(textGroup);
        showTextButton->setObjectName("showTextButton");
        showTextButton->setGeometry(QRect(10, 120, 181, 46));
        clearBoardButton = new QPushButton(textGroup);
        clearBoardButton->setObjectName("clearBoardButton");
        clearBoardButton->setGeometry(QRect(200, 120, 141, 46));

        verticalLayout_3->addWidget(textGroup);

        windowGroup = new QGroupBox(tab);
        windowGroup->setObjectName("windowGroup");
        showBoardButton = new QPushButton(windowGroup);
        showBoardButton->setObjectName("showBoardButton");
        showBoardButton->setGeometry(QRect(10, 30, 141, 46));
        fullscreenButton = new QPushButton(windowGroup);
        fullscreenButton->setObjectName("fullscreenButton");
        fullscreenButton->setGeometry(QRect(160, 30, 141, 46));
        alwaysOnTopCheck = new QCheckBox(windowGroup);
        alwaysOnTopCheck->setObjectName("alwaysOnTopCheck");
        alwaysOnTopCheck->setGeometry(QRect(310, 30, 181, 20));
        toggleTickerButton = new QPushButton(windowGroup);
        toggleTickerButton->setObjectName("toggleTickerButton");
        toggleTickerButton->setGeometry(QRect(300, 90, 131, 46));
        toggleWeatherButton = new QPushButton(windowGroup);
        toggleWeatherButton->setObjectName("toggleWeatherButton");
        toggleWeatherButton->setGeometry(QRect(10, 90, 141, 46));
        toggleClockButton = new QPushButton(windowGroup);
        toggleClockButton->setObjectName("toggleClockButton");
        toggleClockButton->setGeometry(QRect(160, 90, 131, 46));
        toggleLogoButton = new QPushButton(windowGroup);
        toggleLogoButton->setObjectName("toggleLogoButton");
        toggleLogoButton->setGeometry(QRect(440, 90, 121, 46));

        verticalLayout_3->addWidget(windowGroup);

        quickGroup = new QGroupBox(tab);
        quickGroup->setObjectName("quickGroup");
        quickNewsButton = new QPushButton(quickGroup);
        quickNewsButton->setObjectName("quickNewsButton");
        quickNewsButton->setGeometry(QRect(10, 20, 141, 46));
        quickWeatherButton = new QPushButton(quickGroup);
        quickWeatherButton->setObjectName("quickWeatherButton");
        quickWeatherButton->setGeometry(QRect(160, 20, 141, 46));
        quickTimeButton = new QPushButton(quickGroup);
        quickTimeButton->setObjectName("quickTimeButton");
        quickTimeButton->setGeometry(QRect(310, 20, 141, 46));
        quickLogoButton = new QPushButton(quickGroup);
        quickLogoButton->setObjectName("quickLogoButton");
        quickLogoButton->setGeometry(QRect(460, 20, 141, 46));

        verticalLayout_3->addWidget(quickGroup);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        colorGroup = new QGroupBox(tab_2);
        colorGroup->setObjectName("colorGroup");
        colorGroup->setGeometry(QRect(10, 0, 531, 161));
        layoutWidget = new QWidget(colorGroup);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 30, 502, 48));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        setCustomColorButton = new QPushButton(layoutWidget);
        setCustomColorButton->setObjectName("setCustomColorButton");

        horizontalLayout->addWidget(setCustomColorButton);

        setRedBackgroundButton = new QPushButton(layoutWidget);
        setRedBackgroundButton->setObjectName("setRedBackgroundButton");

        horizontalLayout->addWidget(setRedBackgroundButton);

        setBlueBackgroundButton = new QPushButton(layoutWidget);
        setBlueBackgroundButton->setObjectName("setBlueBackgroundButton");

        horizontalLayout->addWidget(setBlueBackgroundButton);

        setGreenBackgroundButton = new QPushButton(layoutWidget);
        setGreenBackgroundButton->setObjectName("setGreenBackgroundButton");

        horizontalLayout->addWidget(setGreenBackgroundButton);

        setBlackBackgroundButton = new QPushButton(layoutWidget);
        setBlackBackgroundButton->setObjectName("setBlackBackgroundButton");

        horizontalLayout->addWidget(setBlackBackgroundButton);

        sizeGroup = new QGroupBox(tab_2);
        sizeGroup->setObjectName("sizeGroup");
        sizeGroup->setGeometry(QRect(9, 159, 531, 131));
        textSmall = new QRadioButton(sizeGroup);
        textSmall->setObjectName("textSmall");
        textSmall->setGeometry(QRect(10, 30, 151, 51));
        textSmall->setChecked(true);
        textMedium = new QRadioButton(sizeGroup);
        textMedium->setObjectName("textMedium");
        textMedium->setGeometry(QRect(10, 70, 141, 51));
        textHuge = new QRadioButton(sizeGroup);
        textHuge->setObjectName("textHuge");
        textHuge->setGeometry(QRect(160, 80, 111, 31));
        textLarge = new QRadioButton(sizeGroup);
        textLarge->setObjectName("textLarge");
        textLarge->setGeometry(QRect(160, 30, 141, 51));
        styleGroup = new QGroupBox(tab_2);
        styleGroup->setObjectName("styleGroup");
        styleGroup->setGeometry(QRect(10, 290, 541, 141));
        fontFamilyCombo = new QComboBox(styleGroup);
        fontFamilyCombo->addItem(QString());
        fontFamilyCombo->addItem(QString());
        fontFamilyCombo->addItem(QString());
        fontFamilyCombo->addItem(QString());
        fontFamilyCombo->addItem(QString());
        fontFamilyCombo->setObjectName("fontFamilyCombo");
        fontFamilyCombo->setGeometry(QRect(10, 30, 131, 39));
        boldTextCheck = new QCheckBox(styleGroup);
        boldTextCheck->setObjectName("boldTextCheck");
        boldTextCheck->setGeometry(QRect(10, 80, 151, 41));
        italicTextCheck = new QCheckBox(styleGroup);
        italicTextCheck->setObjectName("italicTextCheck");
        italicTextCheck->setGeometry(QRect(150, 80, 131, 41));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName("tab_4");
        groupBox = new QGroupBox(tab_4);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(0, 10, 801, 81));
        saveDesignButton = new QPushButton(groupBox);
        saveDesignButton->setObjectName("saveDesignButton");
        saveDesignButton->setGeometry(QRect(20, 20, 151, 46));
        loadDesignButton = new QPushButton(groupBox);
        loadDesignButton->setObjectName("loadDesignButton");
        loadDesignButton->setGeometry(QRect(190, 20, 151, 46));
        resetDesignButton = new QPushButton(groupBox);
        resetDesignButton->setObjectName("resetDesignButton");
        resetDesignButton->setGeometry(QRect(360, 20, 141, 46));
        addElement = new QPushButton(groupBox);
        addElement->setObjectName("addElement");
        addElement->setGeometry(QRect(520, 20, 121, 46));
        removeElement = new QPushButton(groupBox);
        removeElement->setObjectName("removeElement");
        removeElement->setGeometry(QRect(650, 20, 131, 46));
        groupBox_2 = new QGroupBox(tab_4);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(10, 460, 781, 80));
        clearDisegner = new QPushButton(groupBox_2);
        clearDisegner->setObjectName("clearDisegner");
        clearDisegner->setGeometry(QRect(20, 20, 131, 46));
        dragStartPosition = new QPushButton(groupBox_2);
        dragStartPosition->setObjectName("dragStartPosition");
        dragStartPosition->setGeometry(QRect(160, 20, 131, 46));
        previewDesignButton = new QPushButton(groupBox_2);
        previewDesignButton->setObjectName("previewDesignButton");
        previewDesignButton->setGeometry(QRect(300, 20, 131, 46));
        applyDesignButton = new QPushButton(groupBox_2);
        applyDesignButton->setObjectName("applyDesignButton");
        applyDesignButton->setGeometry(QRect(440, 20, 141, 46));
        resetBoardButton = new QPushButton(tab_4);
        resetBoardButton->setObjectName("resetBoardButton");
        resetBoardButton->setGeometry(QRect(20, 90, 141, 46));
        setLogoImageButton = new QPushButton(tab_4);
        setLogoImageButton->setObjectName("setLogoImageButton");
        setLogoImageButton->setGeometry(QRect(470, 90, 141, 46));
        loadConfigButton = new QPushButton(tab_4);
        loadConfigButton->setObjectName("loadConfigButton");
        loadConfigButton->setGeometry(QRect(170, 90, 141, 46));
        saveConfigButton = new QPushButton(tab_4);
        saveConfigButton->setObjectName("saveConfigButton");
        saveConfigButton->setGeometry(QRect(320, 90, 141, 46));
        tabWidget->addTab(tab_4, QString());
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\245\320\276\321\200\320\276\321\210\320\276, \320\274\320\276\320\266\320\265\321\210\321\214 \321\202\320\276\320\263\320\264\320\260 \321\201\320\272\320\260\320\267\320\260\321\202\321\214 \321\207\321\202\320\276 \320\275\321\203\320\266\320\275\320\276 \321\201\320\264\320\265\320\273\320\260\321\202\321\214 \320\262 \320\264\320\270\320\267\320\260\320\271\320\275\320\265 \320\277\320\276\320\264 \320\272\320\276\320\264 \320\272\320\276\321\202\320\276\321\200\321\213\320\271 \321\202\321\213 \320\277\321\200\320\270\321\201\321\213\320\273\320\260\320\273 \320\274\320\275\320\265 \321\200\320\260\320\275\321\214\321\210\320\265", nullptr));
        textGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\242\320\265\320\272\321\201\321\202\320\276\320\262\321\213\320\265 \321\201\320\276\320\276\320\261\321\211\320\265\320\275\320\270\321\217 ", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \321\202\320\265\320\272\321\201\321\202:", nullptr));
        textInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\277\321\200\320\270\320\274\320\265\321\200: \320\241\321\200\320\276\321\207\320\275\320\260\321\217 \320\275\320\276\320\262\320\276\321\201\321\202\321\214...", nullptr));
        showTextButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\320\260\321\202\321\214 \321\202\320\265\320\272\321\201\321\202 \320\275\320\260 \320\264\320\276\321\201\320\272\320\265", nullptr));
        clearBoardButton->setText(QCoreApplication::translate("MainWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \320\264\320\276\321\201\320\272\321\203", nullptr));
        windowGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265 \320\276\320\272\320\275\320\260\320\274\320\270", nullptr));
        showBoardButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\320\260\321\202\321\214 \320\276\320\272\320\275\320\276 \320\264\320\276\321\201\320\272\320\270", nullptr));
        fullscreenButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\273\320\275\321\213\320\271 \321\215\320\272\321\200\320\260\320\275", nullptr));
        alwaysOnTopCheck->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\262\320\265\321\200\321\205 \320\262\321\201\320\265\321\205 \320\276\320\272\320\276\320\275", nullptr));
        toggleTickerButton->setText(QCoreApplication::translate("MainWindow", "\320\272\320\275\320\276\320\277\320\272\320\260 \320\277\320\265\321\200\320\265\320\272\320\273\321\216\321\207\320\265\320\275\320\270\321\217 \321\207\320\260\321\201\320\276\320\262", nullptr));
        toggleWeatherButton->setText(QCoreApplication::translate("MainWindow", "\320\261\321\213\321\201\321\202\321\200\320\276\320\263\320\276 \320\277\321\200\320\276\320\263\320\275\320\276\320\267\320\270\321\200\320\276\320\262\320\260\320\275\320\270\321\217 \320\277\320\276\320\263\320\276\320\264\321\213", nullptr));
        toggleClockButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\265\321\200\320\265\320\272\320\273\321\216\321\207\320\265\320\275\320\270\320\265 \321\207\320\260\321\201\320\276\320\262\n"
"", nullptr));
        toggleLogoButton->setText(QCoreApplication::translate("MainWindow", "toggleLogoButton", nullptr));
        quickGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\221\321\213\321\201\321\202\321\200\321\213\320\265 \320\272\320\276\320\274\320\260\320\275\320\264\321\213", nullptr));
        quickNewsButton->setText(QCoreApplication::translate("MainWindow", "\320\235\320\276\320\262\320\276\321\201\321\202\320\270", nullptr));
        quickWeatherButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\263\320\276\320\264\320\260", nullptr));
        quickTimeButton->setText(QCoreApplication::translate("MainWindow", "\320\222\321\200\320\265\320\274\321\217", nullptr));
        quickLogoButton->setText(QCoreApplication::translate("MainWindow", "\320\233\320\276\320\263\320\276\321\202\320\270\320\277", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265", nullptr));
        colorGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260", nullptr));
        setCustomColorButton->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214 \321\206\320\262\320\265\321\202", nullptr));
        setRedBackgroundButton->setText(QCoreApplication::translate("MainWindow", "\320\232\321\200\320\260\321\201\320\275\321\213\320\271 \321\204\320\276\320\275", nullptr));
        setBlueBackgroundButton->setText(QCoreApplication::translate("MainWindow", "\320\241\320\270\320\275\320\270\320\271 \321\204\320\276\320\275", nullptr));
        setGreenBackgroundButton->setText(QCoreApplication::translate("MainWindow", "\320\227\320\265\320\273\320\265\320\275\321\213\320\271 \321\204\320\276\320\275", nullptr));
        setBlackBackgroundButton->setText(QCoreApplication::translate("MainWindow", "\320\247\320\265\321\200\320\275\321\213\320\271 \321\204\320\276\320\275", nullptr));
        sizeGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\240\320\260\320\267\320\274\320\265\321\200 \321\202\320\265\320\272\321\201\321\202\320\260", nullptr));
        textSmall->setText(QCoreApplication::translate("MainWindow", "\320\234\320\260\320\273\320\265\320\275\321\214\320\272\320\270\320\271 (24px)", nullptr));
        textMedium->setText(QCoreApplication::translate("MainWindow", "\320\241\321\200\320\265\320\264\320\275\320\270\320\271 (48px)", nullptr));
        textHuge->setText(QCoreApplication::translate("MainWindow", "\320\236\320\263\321\200\320\276\320\274\320\275\321\213\320\271 (96px)", nullptr));
        textLarge->setText(QCoreApplication::translate("MainWindow", "\320\232\321\200\321\203\320\277\320\275\321\213\320\271 (72px)", nullptr));
        styleGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\270\320\273\321\214 \321\202\320\265\320\272\321\201\321\202\320\260", nullptr));
        fontFamilyCombo->setItemText(0, QCoreApplication::translate("MainWindow", "Arial", nullptr));
        fontFamilyCombo->setItemText(1, QCoreApplication::translate("MainWindow", "Times New Roman", nullptr));
        fontFamilyCombo->setItemText(2, QCoreApplication::translate("MainWindow", "Courier New", nullptr));
        fontFamilyCombo->setItemText(3, QCoreApplication::translate("MainWindow", "Verdana", nullptr));
        fontFamilyCombo->setItemText(4, QCoreApplication::translate("MainWindow", "\320\235\320\276\320\262\321\213\320\271 \321\215\320\273\320\265\320\274\320\265\320\275\321\202", nullptr));

        boldTextCheck->setText(QCoreApplication::translate("MainWindow", "\320\226\320\270\321\200\320\275\321\213\320\271", nullptr));
        italicTextCheck->setText(QCoreApplication::translate("MainWindow", "\320\232\321\203\321\200\321\201\320\270\320\262", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\320\236\321\204\320\276\321\200\320\274\320\273\320\265\320\275\320\270\320\265", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWindow", "\320\234\320\265\320\264\320\270\320\260", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        saveDesignButton->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\275\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
        loadDesignButton->setText(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \320\275\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
        resetDesignButton->setText(QCoreApplication::translate("MainWindow", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214 \320\272 defaults", nullptr));
        addElement->setText(QCoreApplication::translate("MainWindow", "addElement", nullptr));
        removeElement->setText(QCoreApplication::translate("MainWindow", "removeElement", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        clearDisegner->setText(QCoreApplication::translate("MainWindow", "clearDisegner", nullptr));
        dragStartPosition->setText(QCoreApplication::translate("MainWindow", "dragStartPosition", nullptr));
        previewDesignButton->setText(QCoreApplication::translate("MainWindow", " \320\237\321\200\320\265\320\264\320\277\321\200\320\276\321\201\320\274\320\276\321\202\321\200", nullptr));
        applyDesignButton->setText(QCoreApplication::translate("MainWindow", "\320\237\321\200\320\270\320\274\320\265\320\275\320\270\321\202\321\214 \320\264\320\270\320\267\320\260\320\271\320\275", nullptr));
        resetBoardButton->setText(QCoreApplication::translate("MainWindow", "Reset Board ", nullptr));
        setLogoImageButton->setText(QCoreApplication::translate("MainWindow", "Set logo Image", nullptr));
        loadConfigButton->setText(QCoreApplication::translate("MainWindow", "Load config", nullptr));
        saveConfigButton->setText(QCoreApplication::translate("MainWindow", "Save config", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
