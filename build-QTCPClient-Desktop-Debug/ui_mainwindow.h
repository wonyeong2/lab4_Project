/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QTextBrowser *textBrowser_receivedMessages;
    QGridLayout *gridLayout;
    QPushButton *pushButton_sendMessage;
    QPushButton *pushButton_sendAttachment;
    QLineEdit *lineEdit_message;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(369, 305);
        MainWindow->setStyleSheet(QStringLiteral(""));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        textBrowser_receivedMessages = new QTextBrowser(centralWidget);
        textBrowser_receivedMessages->setObjectName(QStringLiteral("textBrowser_receivedMessages"));
        textBrowser_receivedMessages->setStyleSheet(QStringLiteral(""));

        gridLayout_2->addWidget(textBrowser_receivedMessages, 0, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton_sendMessage = new QPushButton(centralWidget);
        pushButton_sendMessage->setObjectName(QStringLiteral("pushButton_sendMessage"));
        pushButton_sendMessage->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(pushButton_sendMessage, 1, 0, 1, 1);

        pushButton_sendAttachment = new QPushButton(centralWidget);
        pushButton_sendAttachment->setObjectName(QStringLiteral("pushButton_sendAttachment"));
        pushButton_sendAttachment->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(pushButton_sendAttachment, 1, 1, 1, 1);

        lineEdit_message = new QLineEdit(centralWidget);
        lineEdit_message->setObjectName(QStringLiteral("lineEdit_message"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit_message->sizePolicy().hasHeightForWidth());
        lineEdit_message->setSizePolicy(sizePolicy);
        lineEdit_message->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(lineEdit_message, 0, 0, 1, 2);


        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 369, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\355\201\264\353\235\274\354\235\264\354\226\270\355\212\270", Q_NULLPTR));
        pushButton_sendMessage->setText(QApplication::translate("MainWindow", "\353\251\224\354\213\234\354\247\200 \354\240\204\354\206\241\355\225\230\352\270\260", Q_NULLPTR));
        pushButton_sendAttachment->setText(QApplication::translate("MainWindow", "\354\262\250\353\266\200\355\214\214\354\235\274 \353\263\264\353\202\264\352\270\260", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
