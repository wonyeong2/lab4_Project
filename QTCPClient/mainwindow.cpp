#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent) :  //생성자 
    QMainWindow(parent),
    ui(new Ui::MainWindow)  //ui 설정
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    connect(this, SIGNAL(newMessage(QString)), this, SLOT(displayMessage(QString)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(discardSocket()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket));

    socket->connectToHost(QHostAddress::LocalHost, 8080);  //로컬 호스트의 8080 포트에 소켓 연결

    if (socket->waitForConnected())  //서버와의 연결 확인
        ui->statusBar->showMessage("정상적으로 서버에 연결되었습니다");
    else {
        QMessageBox::critical(this, "QTCPClient", QString("오류 발생 : %1.").arg(socket->errorString()));  //오류 메시지 출력
        exit(EXIT_FAILURE);
    }
}

MainWindow::~MainWindow()  //소멸자 
{
    if (socket->isOpen())
        socket->close();
    delete ui;
}

void MainWindow::discardSocket()  //소켓 삭제
{
    socket->deleteLater();
    socket = nullptr;
    ui->statusBar->showMessage("사용자가 채팅방을 나갔습니다");
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)  //소켓 오류 처리
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:  //원격 호스트가 연결을 닫은 경우
        break;
    case QAbstractSocket::HostNotFoundError:  //호스트를 찾을 수 없는 경우
        QMessageBox::information(this, "QTCPServer", "호스트를 찾을 수 없음");
        break;
    case QAbstractSocket::ConnectionRefusedError:  //호스트 이름과 포트 설정이 올바르지 않은 경우
        QMessageBox::information(this, "QTCPServer", "호스트 이름, 포트 설정이 올바른지 확인");
        break;
    default:
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());  //기타 오류. QTcpSocket 포인터를 사용해 오류메시지를 가져옴
        QMessageBox::information(this, "QTCPServer", QString("오류 발생 : %1").arg(socket->errorString()));
        break;
    }
}

void MainWindow::readSocket()  //소켓으로부터 데이터를 읽고 처리
{
    QByteArray buffer;
    QDataStream socketStream(socket);  //소켓에서 데이터를 읽음
    socketStream.setVersion(QDataStream::Qt_5_6);
    socketStream.startTransaction();  //데이터를 읽어들이기 전 트랜잭션 시작
    socketStream >> buffer;  //데이터를 buffer에 저장

    if (!socketStream.commitTransaction())  //데이터 트랜잭션 마치지 못했을 시
    {
        QString message = QString("%1 ) 데이터를 기다리는 중..").arg(socket->socketDescriptor());
        emit newMessage(message);  //새로운 메시지를 보내고 함수 종료
        return;
    }

    QString header = buffer.mid(0, 128);  //버퍼 헤더 정보 추출 (파일 유형, 이름, 크기)
    QString fileType = header.split(",")[0].split(":")[1];
    buffer = buffer.mid(128);

    if (fileType == "attachment") { // 메시지 유형이 "첨부파일"인 경우
        QString fileName = header.split(",")[1].split(":")[1];
        QString ext = fileName.split(".")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

        if (QMessageBox::Yes == QMessageBox::question(this, "QTCPServer", QString("사용자로부터 수신된 파일을 받으시겠습니까?").arg(socket->socketDescriptor()).arg(size).arg(fileName)))
        {
            QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + fileName, QString("File (*.%1)").arg(ext));
            QFile file(filePath);  //받기로 한 경우 파일을 저장
            if (file.open(QIODevice::WriteOnly)) {
                file.write(buffer);
                QString message = QString("%2 사용자가 전송한 파일이 해당 경로에 저장됨").arg(socket->socketDescriptor()).arg(QString(filePath));
                emit newMessage(message);
            }
            else // 오류 발생
                QMessageBox::critical(this, "QTCPServer", "첨부 파일 저장 과정에서 오류 발생");
        }
        else {// 파일을 받지 않는 경우
            QString message = QString("첨부 파일 삭제").arg(socket->socketDescriptor());
            emit newMessage(message);
        }

    }
    else if (fileType == "message") {// 메시지 유형이 "메시지"인 경우
        QString message = QString("%1 ) %2").arg(socket->socketDescriptor()).arg(QString::fromStdString(buffer.toStdString()));
        emit newMessage(message);
    }
}

void MainWindow::displayMessage(const QString& str)
{
    ui->textBrowser_receivedMessages->append(str);  //텍스트 브라우저에 str추가
}

void MainWindow::on_pushButton_sendMessage_clicked()  //메시지 보내기 버튼이 클릭될 때 호출
{
    if (socket)  //소켓이 존재하는지 확인
    {
        if (socket->isOpen())  //소켓이 열려있는 경우
        {
            QString str = ui->lineEdit_message->text();  //lineEdit_message에 입력된 문자열을 가져옴
            ui->textBrowser_receivedMessages->append(str);  //텍스트 브라우저에 해당 문자열 추가

            QDataStream socketStream(socket);  //소켓과 연결
            socketStream.setVersion(QDataStream::Qt_5_6);  //버전 설정

            QByteArray header;
            header.prepend(QString("fileType:message,fileName:null,fileSize:%1;").arg(str.size()).toUtf8());
            //파일 유형, 이름, 크기 정보를 담는 헤더 생성
            header.resize(128);
            QByteArray byteArray = str.toUtf8();  //생성된 헤더 저장
            byteArray.prepend(header);  //헤더를 byteArray의 앞에 추가
            socketStream << byteArray;  //byteArray를 소켓으로 전송
            ui->lineEdit_message->clear();  //lineEdit_message의 내용을 지움
        }
        else
            QMessageBox::critical(this, "QTCPClient", "소켓이 열려있지 않음");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "연결되지 않음");
}

void MainWindow::on_pushButton_sendAttachment_clicked()  //첨부파일 보내기 버튼이 클릭될 때 호출
{
    if (socket)
    {
        if (socket->isOpen())
        {
            QString filePath = QFileDialog::getOpenFileName(this, ("파일 선택"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), ("File (*.json *.txt *.png *.jpg *.jpeg)"));
            if (filePath.isEmpty()) {
                QMessageBox::critical(this, "QTCPClient", "첨부파일 선택되지 않음");
                return;
            }
            QFile m_file(filePath);  //선택된 파일을 QFile 객체로 열기
            if (m_file.open(QIODevice::ReadOnly)) {

                QFileInfo fileInfo(m_file.fileName());  //파일 정보 추출
                QString fileName(fileInfo.fileName());  //파일 이름 추출
                QDataStream socketStream(socket);  //소켓과 연결
                socketStream.setVersion(QDataStream::Qt_5_6);  

                QByteArray header;
                header.prepend(QString("fileType:attachment,fileName:%1,fileSize:%2;").arg(fileName).arg(m_file.size()).toUtf8());
                header.resize(128);
                QByteArray byteArray = m_file.readAll();
                byteArray.prepend(header);
                socketStream.setVersion(QDataStream::Qt_5_6);
                socketStream << byteArray;
            }
            else
                QMessageBox::critical(this, "QTCPClient", "첨부파일 열 수 없음");
        }
        else
            QMessageBox::critical(this, "QTCPClient", "소켓이 열려있지 않음");
    }
    else
        QMessageBox::critical(this, "QTCPClient", "연결되지 않음");
}
