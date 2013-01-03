#ifndef JSON_READER_H
#define JSON_READER_H

#endif // JSON_READER_H

#include <parser.h>
#include "serializer.h"
#include "feed_reader.h"
#include "fillInItem.h"

#define FB_LOGIN_URL "https://www.facebook.com/dialog/oauth"
const QString redirect_uri = "http://www.facebook.com/connect/login_success.html";
const QString confirmToken_uri = "https://graph.facebook.com/oauth/access_token";


const QString FB_App_ID = "152825198104910";
const QString FB_API_Key ="58e3d3263c86c52c82cbde48e0c62d5f";
const QString FB_App_Secret ="309749607205bf286435be6d9f38cc0e";

const QString accessTokenStr = "access_token=";
const QString codeTokenStr = "code=";

const QString userNameURL = "https://graph.facebook.com/me";


// This is primarily for Facebook but intended to
// generic enough for any system using json
class FacebookReader : public JsonReader {
    Q_OBJECT

public:
    FacebookReader (QObject* parent, CatItem it);

    QByteArray processRawStream(QByteArray response){return response;}

    void setAuthorization(QNetworkRequest* request) {
        
        QByteArray AcccessToken;
        if(!m_filt.getCustomString(JSON_ACCESS_TOKEN).isEmpty()){
            AcccessToken = m_filt.getCustomString(
                    JSON_ACCESS_TOKEN).toLatin1();
            qDebug() << "access token from item: " << AcccessToken;
        } else {
            CatItem accountParent = m_filt.getParentByLabel(JSON_ACCESS_TOKEN);
            AcccessToken = accountParent.getCustomString(
                    JSON_ACCESS_TOKEN).toLatin1();
            qDebug() << "access token from parent: " << AcccessToken;
        }
        QString url = request->url().toString();
        if(m_filt.getSourceUpdateTime()>(time_t)1){
            url=url + "?since=" + QString::number(m_filt.getSourceUpdateTime());
        }

        //LAST - must LAST
        QString extra_access = accessTokenStr + AcccessToken;
        m_filt.setCustomPluginInfo(AUTHENTICATION_STR_INHERITED_KEY,extra_access);

        url = url + "?" + extra_access;
        request->setUrl(url);

        //request->setRawHeader( "access_token", AcccessToken );
    }

    QString createPermissionURL(){
//        VerbItem permFillin(it,false, true);
//        QString url =permFillin.getFormattedDescription("FB_PERMISSION_TOKEN_URL");

        QString url(FB_LOGIN_URL);
        url+=QString("?client_id=") + FB_App_ID + "&redirect_uri="
             + redirect_uri + "&scope=offline_access,read_stream,publish_stream";
        qDebug() << "Facebook login url: " << url;
        return url;
    }

    QString scanResultURL(QString resultURL){
        int codeTokenIndex = resultURL.indexOf(codeTokenStr);
        if(codeTokenIndex ==-1){
            return "";
        }
        int start =  codeTokenIndex + codeTokenStr.length();
        int end = resultURL.indexOf("&");
        QString fb_accessCode = resultURL.mid(start,end-start);
        m_filt.setCustomPluginInfo(JSON_ACCESS_TOKEN, fb_accessCode);
        return fb_accessCode;
    }

    QString getUserName(QString key);

    QString tokenGetter(QString code);
    QString extraTokenGetter(QString code);

};
