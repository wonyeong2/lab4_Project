#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>     // 기본 응용 프로그램 창을 제공
#include <QDebug>          // 디버깅 정보에 대한 출력 스트림을 제공
#include <QFile>           // 파일을 읽고 쓰기 위한 인터페이스를 제공
#include <QFileDialog>     // 파일이나 디렉토리를 선택할 수 있는 대화 상자 제공
#include <QMessageBox>     // 모달 대화 상자 제공
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>      // 들어오는 TCP 연결을 수락
#include <QTcpSocket>      // TCP 연결을 설정하고 데이터 스트림을 전송할 수 있게 해줌

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void newMessage(QString);
private slots:
    void newConnection(); // 새연결
    void appendToSocketList(QTcpSocket* socket); // 소켓추가

    void readSocket();// 소켓 읽기
    void discardSocket(); // 소켓 삭제
    void displayError(QAbstractSocket::SocketError socketError);//에러검출

    void displayMessage(const QString& str);//출력메시지
    void sendMessage(QTcpSocket* socket);// 메시지 전송
    void sendAttachment(QTcpSocket* socket, QString filePath); // 첨부 파일

    //버튼 클릭
    void on_pushButton_sendMessage_clicked();
    void on_pushButton_sendAttachment_clicked();

    void refreshComboBox();
private:
    Ui::MainWindow *ui;

    QTcpServer* m_server;
    QSet<QTcpSocket*> connection_set;
};

#endif // MAINWINDOW_H
