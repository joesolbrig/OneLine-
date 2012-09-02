#include "fb_rest.h"



FacebookREST_FeedReader::FacebookREST_FeedReader(QObject* parent, RemoteItemSource filt) : XSLTFeedReader(parent,filt) {
    m_FBSession = FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab",
                                                  "3b86a756f77967dea4674f080fa5d345", QString());
    connect (m_FBSession,SIGNAL(sessionDidLogin(FBUID)), this, SLOT(sessionDidLogin(FBUID)));
    connect (m_FBSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));
}

FacebookREST_FeedReader::FacebookREST_FeedReader(QObject* parent, CatItem it) : XSLTFeedReader(parent,it) {
    m_FBSession = FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab",
                                                   "3b86a756f77967dea4674f080fa5d345", QString());
    connect (m_FBSession,SIGNAL(sessionDidLogin(FBUID)), this, SLOT(sessionDidLogin(FBUID)));
    connect (m_FBSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));
}


void FacebookREST_FeedReader::run(){

    qDebug() << "Getting fb URL: " << m_filt.m_requestUrl;
    QEventLoop loop;

    m_fbrequest = FBRequest::request();
    Dictionary params;
    QString query = m_filt.m_filterStr;
    query = query.arg(m_FBUID);
    params["query"] = query;

    connect (m_fbrequest, SIGNAL(requestDidLoad(QVariant)), this, SLOT(parseFBResult(QVariant)));
    connect (m_fbrequest, SIGNAL(requestFailedWithFacebookError(FBError)),
             this, SLOT(requestFailedWithFacebookError ()));
    connect (this, SIGNAL(downloadDone()), &loop, SLOT(quit()));
    m_fbrequest->call("facebook.fql.query",params);
    //IE, wait this request finishes
    loop.exec();
    //by now, the result have been parsed (or not) by a function called by a signal ...
    m_finished = true;
    quit();

}


void FacebookREST_FeedReader::sessionDidLogin(FBUID aUid){
    m_FBUID = aUid;
}

void FacebookREST_FeedReader::sessionDidLogout(){
    m_FBUID = 0;
}

void FacebookREST_FeedReader::parseFBResult(const QVariant& aContainer){

    if (aContainer.type() == QVariant::List){
        QVariantList list = aContainer.toList();
        for (int i = 0 ; i < list.count(); i ++) {
            QHash<QString, QVariant> dictionary = list.at(i).toHash();
            CatItem item(m_filt.m_requestUrl, dictionary);
            m_results.append(item);
        }
        m_fbrequest->deleteLater();
    }

    m_finished = true;
    emit downloadDone();
}


void FacebookREST_FeedReader::requestFailedWithFacebookError ( const FBError& aError )
{
    qDebug() << "facebook error is " << aError.code() << " - " << aError.description();
    m_fbrequest->deleteLater();
    m_finished = true;
    quit();
}
