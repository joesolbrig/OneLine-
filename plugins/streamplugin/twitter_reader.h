#ifndef TWITTER_READER_H
#define TWITTER_READER_H

#include "feed_reader.h"

#include <QtOAuth>

const QString TWITTER_LOGIN_URL("http://twitter.com/account/verify_credentials.xml");
const QString TWITTER_GET_FRIENDS_URL("http://twitter.com/statuses/friends.xml");
const QString TWITTER_GET_FOLLOWERS_URL("http://twitter.com/statuses/followers.xml");
const QString TWITTER_GET_PUBLIC_STATUS_URL("http://twitter.com/statuses/public_timeline.xml");
const QString TWITTER_GET_FRIENDS_STATUS_URL("http://twitter.com/statuses/friends_timeline.xml"); // same as /home
const QString TWITTER_GET_MY_STATUS_URL("http://twitter.com/statuses/user_timeline.xml");
const QString TWITTER_GET_USER_STATUS_URL("http://twitter.com/statuses/user_timeline/%1.xml");
const QString TWITTER_UPDATE_STATUS_URL("http://twitter.com/statuses/update.xml");

const QString TwitterAccessTokenURL  = "https://api.twitter.com/oauth/access_token";
const QString TwitterRequestTokenURL = "https://api.twitter.com/oauth/request_token";
const QString TwitterAuthorizeURL = "https://api.twitter.com/oauth/authorize";

//Oauth constants..
const QByteArray ParamCallback = "oauth_callback";
const QByteArray ParamCallbackValue = "oob";
const QByteArray ParamVerifier = "oauth_verifier";
const QByteArray ParamScreenName = "screen_name";

//These are hard-coded, being app-specific following the terms of services...
const QByteArray apiKey = "m4Dx9OiELn7KLIui2Aik9Q";
const QByteArray ConsumerKey = "m4Dx9OiELn7KLIui2Aik9Q";
const QByteArray ConsumerSecret = "Iv5VeRWwg21FoZya7247t4OjKTNpvlmX1KTXFqZ1NY";

class TwitterReader : public JsonReader {
    Q_OBJECT

    QOAuth::Interface *m_qoauth;
    QOAuth::HttpMethod m_method;

    QByteArray m_token;
    QByteArray m_tokenSecret;

    QByteArray m_accessToken;
    QByteArray m_accessTokenSecret;

public:

    TwitterReader(QObject* parent, CatItem it): JsonReader(parent, it) {
        m_qoauth = new QOAuth::Interface( this );
        m_qoauth->setConsumerKey( ConsumerKey );
        m_qoauth->setConsumerSecret( ConsumerSecret );
        m_qoauth->setRequestTimeout( 100000 );

        m_method = QOAuth::GET;
    }

    void createTokenStuff(){
        QOAuth::ParamMap requestToken =
                m_qoauth->requestToken(
                        TwitterRequestTokenURL, QOAuth::GET, QOAuth::HMAC_SHA1 );

        if ( m_qoauth->error() != QOAuth::NoError ) {
            qDebug() << "createTokenStuff - Oauth err:" << m_qoauth->error();
            Q_ASSERT(false);
            return;
        }

        m_token = requestToken.value( QOAuth::tokenParameterName() );
        qDebug() << "got token: " << m_token;
        m_tokenSecret = requestToken.value( QOAuth::tokenSecretParameterName() );
        qDebug() << "got token secret: " << m_tokenSecret;
    }

    QString createPermissionURL(){

        createTokenStuff();

        QString url = TwitterAuthorizeURL;//TwitterRequestTokenURL;

        url.append( "?" );
        url.append( "&" + QOAuth::tokenParameterName() + "=" + m_token );
        url.append( "&" + ParamCallback + "=" + ParamCallbackValue );
        qDebug() << "Oauth twitter url" << url;
        return url;

    }


    QOAuth::ParamMap getExtraParams(){
        QOAuth::ParamMap res;
        QString statusCount =
                QString::number(
                        m_filt.getCustomValue(TWITTER_SOURCE_ITEM_COUNT_DESIRED_KEY, 200));
        res.insert( "count", statusCount.toUtf8() );
        if(m_filt.hasLabel(TWITTER_LAST_ID)){
            QString lastID = m_filt.getCustomString(TWITTER_LAST_ID, "");
            res.insert( QByteArray("since_id"), lastID.toAscii() );
        }
        //Entities could be included also
        return res;
    }

    QOAuth::ParamMap createAccessToken(QString pin){
        qDebug() << "pin: " << pin;
        //createTokenStuff();
        QOAuth::ParamMap otherArgs;

        qDebug() << "Oauth pin: " << pin;
        otherArgs.insert( ParamVerifier, pin.toAscii());

        QOAuth::ParamMap accessTokenParamaters = m_qoauth->accessToken( TwitterAccessTokenURL, QOAuth::POST, m_token,
                                            m_tokenSecret, QOAuth::HMAC_SHA1, otherArgs );
        if ( m_qoauth->error() != QOAuth::NoError ) {
            qDebug() << "createAccessToken - Oauth error:" << m_qoauth->error();
            Q_ASSERT(false);
            return QOAuth::ParamMap();
        }
        qDebug() << "screen name:" << accessTokenParamaters.value( ParamScreenName );
        m_accessToken = accessTokenParamaters.value( QOAuth::tokenParameterName() );
        m_accessTokenSecret = accessTokenParamaters.value( QOAuth::tokenSecretParameterName() );

        Q_ASSERT(!m_accessToken.isEmpty());
        Q_ASSERT(!m_accessTokenSecret.isEmpty());
        qDebug() << "access token: " << m_accessToken;
        qDebug() << "access Secret " << m_accessTokenSecret;

        m_filt.setPassword(m_token.constData());
        m_filt.setLoginString(m_tokenSecret.constData());
        return accessTokenParamaters;

    }

    void setAuthorization(QNetworkRequest* request ) {
        qDebug() << "Setting Oauth11 authorization";


        QString userToken = m_filt.getCustomString(TWITTER_CONSUMER_KEY_KEY);
        QString userSecret = m_filt.getCustomString(TWITTER_CONSUMER_SECRET_KEY);

        QOAuth::ParamMap params = getExtraParams();

        QByteArray content = m_qoauth->createParametersString( m_filt.getRequestUrl(), QOAuth::GET,
                    userToken.toAscii(), userSecret.toAscii(),
                    QOAuth::HMAC_SHA1, params, QOAuth::ParseForHeaderArguments );

        qDebug() << "Oauth content: " << content;
        QByteArray t = m_qoauth->inlineParameters( params, QOAuth::ParseForInlineQuery ) ;
        QString ru = m_filt.getRequestUrl();
        m_filt.setRequestUrl(ru.append( t));
        QUrl url = QUrl(m_filt.getRequestUrl());
        request->setUrl(url);
        request->setRawHeader( "Authorization", content );
        //request->setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
        qDebug() << "Oauth11 content: " << content;
    }

};


#endif // TWITTER_READER_H
