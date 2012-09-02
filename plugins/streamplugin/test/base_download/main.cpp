
 /****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #include <QCoreApplication>
 #include <QFile>
 #include <QFileInfo>
 #include <QList>
 #include <QNetworkAccessManager>
 #include <QNetworkRequest>
 #include <QNetworkReply>
 #include <QStringList>
 #include <QTimer>
 #include <QUrl>
 #include <QThread>

 #include <stdio.h>

 #include <QDebug>

 class DownloadManager: public QThread
 {
     Q_OBJECT
     QNetworkAccessManager* m_manager_ptr;
     QNetworkReply * m_reply;

 public:
     bool m_done;
     DownloadManager() {
        m_done = false;
     }

     void run(){
         beginDownload();
         //QTimer::singleShot(10, this, SLOT(beginDownload()));
         //connect(this,SIGNAL(started()),this,SLOT(beginDownload()));
         //exec();

     }

     QString saveFileName(const QUrl &url);
     bool saveToDisk(const QString &filename, QIODevice *data);

 public slots:
    void beginDownload();
    void testPeek();
    void downloadFinished();//QNetworkReply *reply);
 };

 class DownloadManagerStarter : public QObject {
     Q_OBJECT

 public slots:
     void do_it(){
         DownloadManager* dm_ptr = new DownloadManager();
         dm_ptr->start();
         dm_ptr->wait();
         Q_ASSERT(dm_ptr->m_done);
         delete dm_ptr;
         QCoreApplication::instance()->quit();
     }
 };

 void DownloadManager::beginDownload() {
     m_manager_ptr = new QNetworkAccessManager;
     QNetworkRequest request(
             QUrl("http://libcom.org/forums/thought-0/feed"));
     m_reply = m_manager_ptr->get(request);
     Q_ASSERT(m_reply);

    QEventLoop loop;
    connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //connect(m_reply, SIGNAL(finished()), &loop, SLOT(downloadFinished()));
    //connect(m_reply, SIGNAL(readyRead()), &loop, SLOT(testPeek()));
    loop.exec();
    downloadFinished();

 }

 void DownloadManager::testPeek(){
     qDebug() << "Got data" << m_reply->peek(300);
 }

 void DownloadManager::downloadFinished()
 {
     QUrl url = m_reply->url();
     if (m_reply->error()) {
         fprintf(stderr, "Download of %s failed: %s\n",
                 url.toEncoded().constData(),
                 qPrintable(m_reply->errorString()));
     } else {
         QString filename = saveFileName(url);
         if (saveToDisk(filename, m_reply))
             printf("Download of %s succeded (saved to %s)\n",
                    url.toEncoded().constData(), qPrintable(filename));
     }

     m_reply->deleteLater();
     m_done = true;
     quit();
 }

 QString DownloadManager::saveFileName(const QUrl &url)
 {
     QString path = url.path();
     QString basename = QFileInfo(path).fileName();

     if (basename.isEmpty()){ basename = "download";}

     if (QFile::exists(basename)) {
         // already exists, don't overwrite
         int i = 0;
         basename += '.';
         while (QFile::exists(basename + QString::number(i))) {++i;}
         basename += QString::number(i);
     }

     return basename;
 }

 bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
 {
     QFile file(filename);
     if (!file.open(QIODevice::WriteOnly)) {
         fprintf(stderr, "Could not open %s for writing: %s\n",
                 qPrintable(filename),
                 qPrintable(file.errorString()));
         return false;
     }

     file.write(data->readAll());
     file.close();
     return true;
 }






 int main(int argc, char **argv)
 {
     QCoreApplication app(argc, argv);

     DownloadManagerStarter* starter_ptr = new DownloadManagerStarter();
     QTimer::singleShot(0, starter_ptr, SLOT(do_it()));
     app.exec();
     delete starter_ptr;
 }

 #include "main.moc"

