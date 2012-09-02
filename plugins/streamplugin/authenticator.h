#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include "streamplugin.h"

#include "remote_item_source.h"


//These are helper classes for reading more or less any
//kind of nested data anywhere and turning it into Nested CatItems

//This class and children exist to make changing authentication methods easier
class AbstractAuthenticator {
public:
    virtual void setAuthorization(QNetworkRequest* request)=0;
    AbstractAuthenticator(CatItem* filt){
        Q_ASSERT(filt);
        m_filt = filt;
    }

    CatItem* m_filt;
};

class BasicAuthenticator : AbstractAuthenticator {
    BasicAuthenticator(CatItem* filt): AbstractAuthenticator(filt){ }
    void setAuthorization(QNetworkRequest* request){
        QString basicAuth = m_filt->m_loginString.arg(m_filt->m_name).arg(m_filt->m_password);
        request->setRawHeader("Authorization", basicAuth.toUtf8());
    }
};

//This is definitely kind of a pain...
class Oauth11Authenticator : AbstractAuthenticator {
    QOAuth::Interface* m_qoauth;
    QOAuth::HttpMethod* m_method;
public:
    Oauth11Authenticator(CatItem* filt, QOAuth::Interface *qoauth,
                         QOAuth::HttpMethod* method):
            AbstractAuthenticator(filt), m_qoauth(qoauth), m_method(method) { }

    void setAuthorization(QNetworkRequest* request) {
        qDebug() << "Setting Oauth11 authorization";

        QOAuth::ParamMap params;
        QString statusCount =
                QString::number(
                        m_filt->m_item.getCustomValue(
                                TWITTER_SOURCE_ITEM_COUNT_DESIRED_KEY, 200));
        params.insert( "count", statusCount.toUtf8() );
        int index = m_filt->m_password.indexOf( '&' );
        QByteArray token = m_filt->m_name.left( index ).toAscii();
        QByteArray tokenSecret = m_filt->m_name.right(
                m_filt->m_name.length() - index - 1 ).toAscii();
        QByteArray content = m_qoauth->createParametersString(
                m_filt->m_requestUrl,
                m_method ,
                token,
                tokenSecret,
                QOAuth::HMAC_SHA1, params, QOAuth::ParseForHeaderArguments );
        qDebug() << "Oauth11 content " << content;
        request->setRawHeader( "Authorization", content );
    }
};

//Oauth2 Authentication is much easier...
class Oauth2Authenticator : AbstractAuthenticator {

    //Eventually test and refresh token if stale...
    Oauth2Authenticator(CatItem* filt): AbstractAuthenticator(filt){ }
    void setAuthorization(QNetworkRequest* request) {
        QByteArray AcccessToken = m_filt->m_item.getCustomString(TWITTER_SOURCE_ITEM_COUNT_DESIRED_KEY);
        request->setRawHeader( "access_token", AcccessToken );
    }
};



#endif // AUTHENTICATOR_H
