#ifndef REMOTE_ITEM_SOURCE_H
#define REMOTE_ITEM_SOURCE_H

#include "streamplugin.h"

/*
class StreamPlugin;

class RemoteItemSource: public CatItem {

public:

    //CatItem getitem();

    //QString getname();



protected:

//    CatItem m_item;
//    QString m_name;
//    QString m_nameUser;
//    QString m_password;
//    QString m_loginString;
//    QString m_requestUrl;

    //QString m_filterStr;
    //QHash<QByteArray, QByteArray> m_extraParams;


    //sig = 0-2: 0 none, 1 some, 2: none or some
//    int tagSig;
//    int timeSig;
//    int keysSig;
//    int contactSig;
//    int loginSig;


//    QString auth(){
//        return m_loginString.arg(m_name).arg(m_password);
//    }
public:

    RemoteItemSource(): CatItem() {
//        tagSig=0;
//        timeSig=0;
//        keysSig=0;
//        contactSig=0;
//        loginSig=0;
    }

//    QString toString(){
//        Q_ASSERT(m_item.isEmpty());
//        QString s;
//        QTextStream txs(&s);
//        txs
//            << m_name << TYPE_LIST_SEP
//            << m_password << TYPE_LIST_SEP
//            << m_loginString << TYPE_LIST_SEP
//
//            << m_requestUrl << TYPE_LIST_SEP
//            << m_filterStr << TYPE_LIST_SEP
//
//            << tagSig << TYPE_LIST_SEP
//            << timeSig << TYPE_LIST_SEP
//            << keysSig << TYPE_LIST_SEP
//
//            << contactSig << TYPE_LIST_SEP
//            << loginSig << TYPE_LIST_SEP;
//
////        return txs.readAll();
//        return s;
//    }

//    RemoteItemSource(QString toDecode){ //field should match above function,
//        //qDebug() << "initial string " << toDecode;
//        QStringList fields = toDecode.split(TYPE_LIST_SEP);
//        Q_ASSERT(fields.count()>3);
//
//        m_name = fields.first(); fields.pop_front();
//        m_password = fields.first(); fields.pop_front();
//        m_loginString = fields.first(); fields.pop_front();
//
//        m_requestUrl = fields.first();fields.pop_front();
//        m_filterStr = fields.first(); fields.pop_front();
//
//        tagSig = fields.first().toInt(); fields.pop_front();
//        timeSig = fields.first().toInt(); fields.pop_front();
//        keysSig = fields.first().toInt(); fields.pop_front();
//
//        contactSig = fields.first().toInt(); fields.pop_front();
//        loginSig = fields.first().toInt();
//    }


    RemoteItemSource(const RemoteItemSource& d): CatItem(d) {

//        m_loginString = d.m_loginString;
//        m_password = d.m_password;
//
//        m_requestUrl = d.m_requestUrl;
//        //m_filterStr = d.m_filterStr;
//
//        tagSig = d.tagSig;
//        timeSig = d.timeSig;
//        keysSig = d.keysSig;
//
//        contactSig = d.contactSig;
//        loginSig = d.loginSig;
//        m_item = d.m_item;

    }


    RemoteItemSource(QSettings* set):CatItem(set) {
        ;


//        m_loginString = set->value("loginString").toString();
//        m_requestUrl = set->value("requestUrl").toString();

//        QString filterStringFileName = set->value("filterStringFile").toString();
//        QString settingsPath("");
//        QString baseDirName = (set)->fileName();
//
//        int lastSlash = baseDirName.lastIndexOf(QLatin1Char('/'));
//        if (lastSlash != -1){
//            settingsPath = baseDirName.mid(0, lastSlash);
//        }
//        settingsPath += FILTER_SUBDIR;
//        QString fullFilterPath = settingsPath + QLatin1Char('/') + filterStringFileName;
//        QFile sourceFile(fullFilterPath);
//        if(sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//            m_filterStr = sourceFile.readAll();
//        } else {
//            qDebug() << "Filter XLS File: "
//                     << fullFilterPath << " not found ";
//        }
//        tagSig = set->value(SOURCE_TAG_SIG).toInt();
//        timeSig = set->value(SOURCE_TIME_SIG).toInt();
//        keysSig = set->value(SOURCE_KEY_SIG).toInt();
//        contactSig = set->value(SOURCE_CONTACT_SIG).toInt();
//        loginSig = set->value(SOURCE_LOGIN_SIG).toInt();




    }



    RemoteItemSource(CatItem it): CatItem(it) {

//        m_item = *it;
//        m_name = it->getName();
//        m_loginString = it->getCustomString(XSLT_PLUGIN_LOGIN_PATH);
//        m_password = it->getCustomString(XSLT_PLUGIN_PASSWORD_PATH );
//        m_nameUser = it->getCustomString(XSLT_PLUGIN_USERNAME_PATH);
//
//        m_requestUrl = it->getCustomString(XSLT_PLUGIN_SOURCE_PATH);
//        QString fullFilterPath = it->getCustomString(XSLT_PLUGIN_FILTER_SOURCE);
//        if(!fullFilterPath.isEmpty()){
//            QFile sourceFile(fullFilterPath);
//            qDebug() << "Filter XLS File: " << fullFilterPath;
//            if(sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//                m_filterStr = sourceFile.readAll();
//            } else {
//                qDebug() << " not found ";
//                Q_ASSERT(false);
//            }
//            tagSig = it->getCustomValue(SOURCE_TAG_SIG);
//            timeSig = it->getCustomValue(SOURCE_TIME_SIG);
//            keysSig = it->getCustomValue(SOURCE_KEY_SIG);
//            contactSig = it->getCustomValue(SOURCE_CONTACT_SIG);
//            loginSig = it->getCustomValue(SOURCE_LOGIN_SIG);
//        }
    }

};



*/
#endif // REMOTE_ITEM_SOURCE_H
