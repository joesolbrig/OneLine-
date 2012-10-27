#ifndef FEEDREADER_H
#define FEEDREADER_H


//#include <QtOAuth>

//#include "fbsession.h"
//#include "fbrequest.h"
//#include "fberror.h"
#include "streamplugin.h"
#include "remote_item_source.h"
#include "fillInItem.h"
//#include "authenticator.h"

// This inherentence tree of readers is a bit messy,
// Child inherent parents without a true "is-a" relation
// But it let's me deal with a series of messy protocols
// Minimizing repeated code. If the protocols weren't shit
// the process would be nicer - []

class AbstractReader : public QThread {
    Q_OBJECT
protected:

    QUrl m_url;
    QNetworkReply* m_reply;
    QNetworkAccessManager* m_netAccessMan;
    QList<CatItem> m_results;
    CatItem m_baseResult;
    CatItem m_filt;
    bool m_finished;

public:
    AbstractReader(QObject* , CatItem filt): m_url(), m_reply(0),m_netAccessMan(0),
        m_results(), m_filt(filt) {
        m_finished = false;
    }
    ~AbstractReader(){
    }

    virtual QList<CatItem> getSources(){return QList<CatItem>();}
    bool isFinished(){ return m_finished; }
    QList<CatItem> getResults(){ return m_results; }
    CatItem getBaseResult(){ return m_baseResult; }
    CatItem getFilter(){return m_filt;}
    void addError(QString err){
        CatItem accountItem = m_filt.getAccountItem();
        qDebug() << "Error: " << err;
        if(accountItem.isEmpty()){
            accountItem.setTemporaryDescription(err + ":" + accountItem.getDescription());
            accountItem.setExternalWeight(MAX_MAX_EXTERNAL_WEIGHT,CatItem(addPrefix(TYPE_PREFIX, ERROR_TYPE)));
            accountItem.setSeen(false);
            m_results.append(accountItem);
        }

    }


protected:

    virtual QByteArray processRawStream(QByteArray response)=0;
    virtual void parseTransformedStream(QByteArray transformedXML)=0;
    //virtual CatItem takeAnItemFromReader(QXmlStreamReader* reader)=0;;
    virtual void run()=0;

    signals: void download_finished();
    public slots:
        virtual void do_abort(){
            m_finished = true;
            if(m_reply){
                m_reply->abort();
                m_reply->deleteLater();
                m_reply = 0;
            }
            if(m_netAccessMan){
                //m_netAccessMan->disconnect();
                m_netAccessMan->deleteLater();
                m_netAccessMan = 0;
            }
            quit();
        }
};

class AbstractFeedReader : public AbstractReader {
    Q_OBJECT

protected:
    QHash<QString, EntryField> m_extraGlobals;

public:
    void setValue(QString key, QString value, CatItem it=CatItem()){
        m_extraGlobals[key].value = value;
        m_extraGlobals[key].valueItem = it;
    }
    AbstractFeedReader(QObject* parent, CatItem filt): AbstractReader(parent, filt) {
        m_finished = false;
        connect(parent, SIGNAL(end_current_downloads()), this, SLOT(do_abort()));
    }

    void run();
    virtual QString getRequestURL() {return m_filt.getRequestUrl();}
    virtual void sendRequest();
    virtual void cacheRawStream(QByteArray& response);
    QString downloadURLResult(QString url, QString accessTokenStr);
    virtual QByteArray getResult(){ return m_reply->readAll();}
    virtual QByteArray processRawStream(QByteArray response) = 0;
    virtual void parseTransformedStream(QByteArray transformedData) =0;
protected:
    virtual void setAuthorization(QNetworkRequest* )=0;

public slots:

    virtual void processRequestResult();
    bool shouldReplaceParent(){ return false;}

};

//Generic html/xml retrieval for rss, atom, and related
class XSLTFeedReader : public AbstractFeedReader {
    Q_OBJECT

public:
    XSLTFeedReader(QObject* parent, CatItem it):
            AbstractFeedReader(parent, CatItem(it))  {
        m_finished = false;

        connect(parent, SIGNAL(end_current_downloads()), this, SLOT(do_abort()));
    }

//    XSLTFeedReader(QObject* parent, CatItem filt): AbstractFeedReader(parent, filt) {
//        Q_ASSERT(m_filt == filt);
//        m_finished = false;
//        connect(parent, SIGNAL(end_current_downloads()), this, SLOT(do_abort()));
//    }

    //void sendRequest();
    //void processRequestResult();
    void setFilter(CatItem filt){m_filt = filt;}
    QByteArray processRawStream(QByteArray response);
    void parseTransformedStream(QByteArray transformedXML);
    CatItem takeAnItemFromReader(QXmlStreamReader* reader);
protected:

    //Basic authorization
    void setAuthorization(QNetworkRequest* request){

        if(m_filt.getLoginString().isEmpty())
            { return; }

        QString basicAuth = m_filt.getLoginString().arg(m_filt.getUserName()
                                ).arg(m_filt.getPassword());
        request->setRawHeader("Authorization", basicAuth.toUtf8());
    }

};


class JsonReader : public AbstractFeedReader {
    Q_OBJECT

public:
    JsonReader (QObject* parent, CatItem it): AbstractFeedReader(parent, CatItem(it)){}
    QByteArray processRawStream(QByteArray response){return response;}
    void parseTransformedStream(QByteArray transformedData);
    void beginParseStream();

    bool shouldReplaceParent(){ return m_resultAccumulator.isJustTakeChildren();}
    virtual void setAuthorization(QNetworkRequest* ){};

protected:

    void scanJsonResults(FillinItem fi, QVariant parsedResult, int recur=0);
    FillinItem m_resultAccumulator;

};

class QXmlStreamReader;
class HttpHeadReader : public XSLTFeedReader {
    Q_OBJECT

    QXmlStreamReader* m_reader;

public:
    HttpHeadReader(QObject* parent, CatItem it): XSLTFeedReader(parent,it)  {
        m_filt.setRequestUrl(it.getPath());
        QString filterPath = QString(SCRIPT_DEFAULT_PATH) + XLS_SUBDIR + RSS_HEADER_XSLT;
        m_filt.setFilterFilePath(filterPath);
        m_finished = false;
        connect(parent, SIGNAL(end_current_downloads()), this, SLOT(do_abort()));
    }

    void sendRequest();
    void run();
    //inherent void processRequestResult();
    //void gotData(){ }

    QByteArray processRawStream(QByteArray raw){return raw;}
    void setAuthorization(QNetworkRequest* ){ }
public slots:
    void parseStreamIncrementally(qint64 i=0, qint64 j=0);
    void parseStreamIncrementallyNA(){parseStreamIncrementally();}

signals:
    void done();

};

class XmlFileReader : public XSLTFeedReader {
public:
    XmlFileReader (QObject* parent, CatItem filt):XSLTFeedReader(parent, filt){}
    void run();

};



#endif // FEEDREADER_H
