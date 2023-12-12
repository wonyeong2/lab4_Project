#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new QTcpServer();

    if(m_server->listen(QHostAddress::Any, 8080))
    {
       connect(this, &MainWindow::newMessage, this, &MainWindow::displayMessage);
       connect(m_server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
       ui->statusBar->showMessage("연결중입니다..");
    }
    else
    {
        QMessageBox::critical(this,"QTCPServer",QString("서버가 시작되지 않습니다 : %1.").arg(m_server->errorString()));
        exit(EXIT_FAILURE);
    }
}

MainWindow::~MainWindow()
{
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    m_server->close();
    m_server->deleteLater();

    delete ui;
}

void MainWindow::newConnection() // 새로운 연결
{
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());
}

void MainWindow::appendToSocketList(QTcpSocket* socket)
{
    connection_set.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &MainWindow::displayError);
    ui->comboBox_receiver->addItem(QString::number(socket->socketDescriptor()));
    displayMessage(QString("NOTICE ) %1번 사용자 입장").arg(socket->socketDescriptor()));
}

void MainWindow::readSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_6);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 ) 데이터를 기다리는 중..").arg(socket->socketDescriptor());
        emit newMessage(message);
        return;
    }

    QString header = buffer.mid(0,128);
    QString fileType = header.split(",")[0].split(":")[1];

    buffer = buffer.mid(128);

    if(fileType=="attachment"){ // 메시지 유형이 "첨부파일"인 경우
        QString fileName = header.split(",")[1].split(":")[1];
        QString ext = fileName.split(".")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

        if (QMessageBox::Yes == QMessageBox::question(this, "QTCPServer", QString("사용자로부터 수신된 파일을 받으시겠습니까?").arg(socket->socketDescriptor()).arg(size).arg(fileName))) // 파일을 받는 경우
        {
            QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/"+fileName, QString("File (*.%1)").arg(ext));

            QFile file(filePath);
            if(file.open(QIODevice::WriteOnly)){
                file.write(buffer);
                QString message = QString("%2 사용자가 전송한 파일이 해당 경로에 저장됨").arg(socket->socketDescriptor()).arg(QString(filePath));
                emit newMessage(message);
            }else  // 오류 발생
                QMessageBox::critical(this,"QTCPServer", "첨부 파일 저장 과정에서 오류 발생");
        }else{// 파일을 받지 않는 경우
            QString message = QString("첨부 파일 삭제").arg(socket->socketDescriptor());
            emit newMessage(message);
        }
    }else if(fileType=="message"){ // 메시지 유형이 "메시지"인 경우
        QString message = QString("%1 ) %2").arg(socket->socketDescriptor()).arg(QString::fromStdString(buffer.toStdString())); //사용자 :: 메시지
        emit newMessage(message);
    }
}

void MainWindow::discardSocket() // 소켓 삭제
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    if (it != connection_set.end()){
        displayMessage(QString("사용자가 채팅방을 나갔습니다.").arg(socket->socketDescriptor()));
        connection_set.remove(*it);
    }
    refreshComboBox();
    
    socket->deleteLater();
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "QTCPServer", "호스트를 찾을 수 없습니다.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "QTCPServer", "호스트 이름 및 포트 설정이 올바른지 확인");
        break;
        default:
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
            QMessageBox::information(this, "QTCPServer", QString("오류가 발생했습니다.: %1.").arg(socket->errorString()));
        break;
    }
}

void MainWindow::on_pushButton_sendMessage_clicked()
{
    QString receiver = ui->comboBox_receiver->currentText();

    if(receiver=="Server") // 받는 사람이 서버인 경우
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            sendMessage(socket);
        }
    }
    else // 받는 사람이 일반 사용자인 경우
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            if(socket->socketDescriptor() == receiver.toLongLong())
            {
                sendMessage(socket);
                break;
            }
        }
    }
    ui->lineEdit_message->clear();
}


void MainWindow::on_pushButton_sendAttachment_clicked()
{
    QString receiver = ui->comboBox_receiver->currentText();

    QString filePath = QFileDialog::getOpenFileName(this, ("파일을 선택하세요."), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), ("File (*.json *.txt *.png *.jpg *.jpeg)"));

    if(filePath.isEmpty()){
        QMessageBox::critical(this,"QTCPClient","첨부파일이 선택되지 않았습니다.");
        return;
    }

    if(receiver=="Server") // 받는 사람이 서버인 경우
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            sendAttachment(socket, filePath);
        }
    }
    else // 받는 사람이 일반 사용자인 경우
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            if(socket->socketDescriptor() == receiver.toLongLong())
            {
                sendAttachment(socket, filePath);
                break;
            }
        }
    }
    ui->lineEdit_message->clear();
}

void MainWindow::sendMessage(QTcpSocket* socket) // 메시지 전송
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QString str = ui->lineEdit_message->text();
            ui->textBrowser_receivedMessages->append(str);
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_6);

            QByteArray header;
            header.prepend(QString("fileType:message,fileName:null,fileSize:%1;").arg(str.size()).toUtf8());
            header.resize(128);

            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            socketStream.setVersion(QDataStream::Qt_5_6);
            socketStream << byteArray;
        }
        else
            QMessageBox::critical(this,"QTCPServer","소켓이 열려있지 않습니다.");
    }
    else
        QMessageBox::critical(this,"QTCPServer","연결되지 않았습니다.");
}

void MainWindow::sendAttachment(QTcpSocket* socket, QString filePath) // 첨부파일 전송
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QFile m_file(filePath);
            if(m_file.open(QIODevice::ReadOnly)){

                QFileInfo fileInfo(m_file.fileName());
                QString fileName(fileInfo.fileName());

                QDataStream socketStream(socket);
                socketStream.setVersion(QDataStream::Qt_5_6);

                QByteArray header;
                header.prepend(QString("fileType:attachment,fileName:%1,fileSize:%2;").arg(fileName).arg(m_file.size()).toUtf8());
                header.resize(128);

                QByteArray byteArray = m_file.readAll();
                byteArray.prepend(header);

                socketStream << byteArray;
            }else
                QMessageBox::critical(this,"QTCPClient","첨부파일을 열 수 없습니다.");
        }
        else
            QMessageBox::critical(this,"QTCPServer","소켓이 열려있지 않습니다.");
    }
    else
        QMessageBox::critical(this,"QTCPServer","연결되지 않았습니다.");
}

void MainWindow::displayMessage(const QString& str)
{
    ui->textBrowser_receivedMessages->append(str);
}

void MainWindow::refreshComboBox(){
    ui->comboBox_receiver->clear();
    ui->comboBox_receiver->addItem("Server");
    foreach(QTcpSocket* socket, connection_set)
        ui->comboBox_receiver->addItem(QString::number(socket->socketDescriptor()));
}
