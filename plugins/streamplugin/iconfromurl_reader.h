#ifndef ICONFROMURL_READER_H
#define ICONFROMURL_READER_H

#include "feed_reader.h"

class IconFromUrlReader : public AbstractFeedReader
{
public:
    IconFromUrlReader(QObject* parent, CatItem filt);
    QByteArray processRawStream(QByteArray response){return response;};
    void parseTransformedStream(QByteArray );
    QString getRequestURL(){return m_filt.getCustomString(ICON_URL_KEY);}
    void setAuthorization(QNetworkRequest* request){
        if(m_filt.hasLabel(AUTHENTICATION_STR_INHERITED_KEY)){
            QString url = request->url().toString();
            url = url + "?" + m_filt.getCustomString(AUTHENTICATION_STR_INHERITED_KEY);
            request->setUrl(url);
        }
    }

};

#endif // ICONFROMURL_READER_H
