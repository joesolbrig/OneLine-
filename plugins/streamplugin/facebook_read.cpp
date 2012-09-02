#include <qvariant.h>
#include <qsettings.h>
#include "facebook_reader.h"

FacebookReader::FacebookReader (QObject* parent, CatItem it): JsonReader(parent, it) {

}

QString FacebookReader::getUserName(QString key) {
    m_filt.setCustomPluginInfo(JSON_ACCESS_TOKEN,key);
    m_filt.setRequestUrl(userNameURL);

    if(m_netAccessMan){m_netAccessMan->deleteLater();}
    m_netAccessMan = new QNetworkAccessManager;
    sendRequest();
    QEventLoop loop;
    connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray rawBytes = m_reply->readAll();

    bool ok;
    QJson::Parser parser;

    QVariant userInfoResult = parser.parse(rawBytes, &ok);
    if (!ok) {
        qWarning() << "Error while parsing network reply: " << rawBytes;
        return "";
    }

    QMap<QString, QVariant> userInfoHash = userInfoResult.toMap();
    return userInfoHash["name"].toString();
}

void JsonReader::beginParseStream() {
    QString fillinFile = m_filt.getCustomString(FILLIN_GEN_FILE_KEY);
    QString fillinKey = m_filt.getCustomString(FILLIN_GEN_SUBKEY_KEY);

    Q_ASSERT(!fillinFile.isEmpty() &&
       !fillinKey.isEmpty());

    QSettings set(fillinFile, QSettings::IniFormat);
    SettingsTree st(&set,fillinKey);
    m_resultAccumulator.setGeneratingItem(m_filt);
    m_resultAccumulator.addData(&st,0,0);
    m_resultAccumulator.setGenerating();
    qDebug() << "beginParseStream: " << fillinFile << " | " << fillinKey;

}

void JsonReader::parseTransformedStream(QByteArray response){

    //qDebug() << "parseTransformedStream: " << response;

    bool ok;
    QJson::Parser parser;

    QVariant parsedResult = parser.parse(response, &ok);
    if (!ok) {
        qWarning() << "Error while parsing network reply: " << response;
        return;
    }
    VariantTree vt(parsedResult);

    beginParseStream();
    m_resultAccumulator.addData(&vt,0,0);
    VerbItem vi(m_filt);
    QHash<QString, EntryField> globals = m_extraGlobals;
    QString auth = m_filt.getCustomString(AUTHENTICATION_STR_INHERITED_KEY);
    EntryField e;
    e.name = AUTHENTICATION_STR_INHERITED_KEY;
    e.value = auth;
    globals[AUTHENTICATION_STR_INHERITED_KEY] = e;
    m_baseResult = m_resultAccumulator.createTheItem(vi, m_extraGlobals);
    m_results.append(m_baseResult);

    //Set last update time
    if(m_filt.supportsSourceUpdateTime() and m_baseResult.getChildRelationCount()>0){
        m_filt.setSourceUpdateTime(appGlobalTime());
        m_results.append(m_filt);
    }

}


/* https://graph.facebook.com/oauth/access_token?
 client_id=YOUR_APP_ID&redirect_uri=YOUR_URL&
 client_secret=YOUR_APP_SECRET&code=THE_CODE_FROM_ABOVE

Does a whole extra song-and-dance... to get "token" from "code"
*/
QString FacebookReader::tokenGetter(QString code){
    qDebug() << "OauthCode: " << code;
    QString urlPartial =confirmToken_uri + ("?client_id=" + QString(FB_App_ID) + "&redirect_uri=" + redirect_uri
                                            +"&client_secret="+FB_App_Secret+"&code=");
    qDebug() << "Facebook partial url: " << urlPartial;
    QString url = urlPartial.append(code);
    qDebug() << "Facebook get-token url: " << url;

    QString token = downloadURLResult(url, accessTokenStr);
    if(!token.isEmpty()){
        m_filt.setCustomPluginInfo(JSON_ACCESS_TOKEN, token);
        return token;
    } else { return "";}

}


/*
The fricken' "extra" token, WTF

https://graph.facebook.com/oauth/access_token?
    client_id=APP_ID&
    client_secret=APP_SECRET&
    grant_type=fb_exchange_token&
    fb_exchange_token=EXISTING_ACCESS_TOKEN

Does a whole other extra song-and-dance... to get "extraToken" from "token"
*/
QString FacebookReader::extraTokenGetter(QString code){
    qDebug() << "code: " << code;
    QString urlPartial =confirmToken_uri + ("?client_id=" + QString(FB_App_ID)
                                            +"&client_secret="+FB_App_Secret
                                            +"&grant_type=fb_exchange_token"
                                            +"&fb_exchange_token=");
    qDebug() << "Facebook partial url: " << urlPartial;
    QString url = urlPartial.append(code);
    qDebug() << "Facebook get-token url: " << url;

    QString token = downloadURLResult(url, accessTokenStr);
    if(!token.isEmpty()){
        m_filt.setCustomPluginInfo(JSON_ACCESS_TOKEN, token);
        return token;
    } else { return "";}
}


