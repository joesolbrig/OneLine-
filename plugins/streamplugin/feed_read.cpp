#include "feed_reader.h"

void XmlFileReader::run(){

    //qDebug() << "Getting File url: " << getRequestURL();
    QString filePath = getRequestURL();
    if(filePath.startsWith(FILE_PREFIX))
        { filePath = filePath.mid(FILE_PREFIX.length()); }

    QFile f(filePath);
    //qDebug() << "openning xml file : " << filePath;
    f.open(QIODevice::ReadOnly |QIODevice::Text);
    if(!f.isOpen()){
        addError(QString("error openning") + filePath);
    } else if(!f.isReadable()){
        addError(QString("error reading") + filePath);
    } else {
        QByteArray rawXML = f.readAll();
        if(rawXML.isEmpty()){
            addError(QString("error empty XML file") + filePath);
        }
        QByteArray transformedXML = processRawStream(rawXML);
        if(transformedXML.isEmpty()){
            //qDebug() << "error processing transformed XML stream: ";
            Q_ASSERT(false);
        } else {
            parseTransformedStream(transformedXML);
        }
    }
    f.close();
    m_finished = true;

}

void AbstractFeedReader::run(){
    qDebug() << "Getting (Remote) URL: " << getRequestURL();
    if(getRequestURL().isEmpty()) {
        addError("null download URL");
        m_finished = true;
    } else {
        if(m_netAccessMan){m_netAccessMan->deleteLater();}
        m_netAccessMan = new QNetworkAccessManager;
        sendRequest();
        QEventLoop loop;
        connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()));
        connect(m_reply, SIGNAL(error()), &loop, SLOT(quit()));
        loop.exec();
        processRequestResult();
        m_finished = true;
        quit();
    }
}



QString AbstractFeedReader::downloadURLResult(QString url, QString delimitedTokenStr){
    QNetworkAccessManager nam;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", ONELINE_AGENT_STR);
    QNetworkReply* reply = nam.get(request);
    if(!reply) {
        qDebug() << "failed to create download object";
        return "";
    }
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << "Download Error" << reply->errorString();
        CatItem sourceParent = m_filt.getTypeParent(BaseChildRelation::ACCOUNT_SOURCE_PARENT);
        if(!sourceParent.isEmpty()){
            sourceParent.setSeen(false);
            m_results.append(sourceParent);
        }
        return "";
    }
    QByteArray rawBytes = reply->readAll();
    int accessTokenIndex = rawBytes.indexOf(delimitedTokenStr);
    if(accessTokenIndex ==-1){
        qDebug() << "Download Error, " << delimitedTokenStr << "not found in bytes:" << rawBytes;
        return "";
    }
    int start =  accessTokenIndex + delimitedTokenStr.length();
    QString result = rawBytes.mid(start);
    return result;
}


void AbstractFeedReader::sendRequest(){
    m_url = QUrl(getRequestURL());
    QNetworkRequest request(m_url);

    qDebug() << "caching enum" << request.attribute(QNetworkRequest::CacheLoadControlAttribute).toInt();

//    Accept: */*
//    Host: www.reddit.com
//    Connection: Keep-Alive

    setAuthorization(&request);

    if(request.url().isEmpty()){
        request.setUrl(m_url);
    }
    request.setRawHeader("User-Agent", ONELINE_AGENT_STR);
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "Keep-Alive");
//    QString host = m_url.host();
//    request.setRawHeader("Host", host);

    qDebug() << "Request url" << request.url().toString();
    QList<QByteArray> bl = request.rawHeaderList();
    QByteArray output;
    for(int i=0; i<bl.count();i++){
        output+=bl[i] + ": " + request.rawHeader(bl[i])+ "\n";
    }
    qDebug() << "sent header" << output;

    m_reply = m_netAccessMan->get(request);
    if(!m_reply){
        addError(QString("empty reply error") );
        m_finished = true;
        quit();
    } else if (m_reply->error()){
        qDebug() << "Start Error:" << m_reply->errorString();
        qDebug() << "Error #:" << m_reply->error();
        addError(QString(m_reply->errorString()) );
    }
}


void AbstractFeedReader::processRequestResult(){

    if (m_reply->error() != QNetworkReply::NoError) {
        addError(m_reply->errorString());
    } else {
        QVariant attr =m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QByteArray rawBytes = getResult();
        if(!attr.canConvert(QVariant::Int)){
            addError("no http return code");
        } else if(attr.toInt()!=200){
            qDebug() << "return code:" << attr.toInt();
            if(attr.toInt()==301){
                QUrl newURL = m_reply->header(QNetworkRequest::LocationHeader).toUrl();
                m_filt.setRequestUrl(newURL.toString());
                m_results.append(m_filt);
            } else {
                qDebug() << "don't know how to deal with code";
            }
        } else {
            qDebug() << "reply url" <<  m_reply->url();
            qDebug() << "rawHeader list" <<  m_reply->rawHeaderList();
            QList<QByteArray> bl = m_reply->rawHeaderList();
            QByteArray output;
            for(int i=0; i<bl.count();i++){
                output+=bl[i] + ": " + m_reply->rawHeader(bl[i]) + "\n";
            }
            //qDebug() << "received header" << output;
            cacheRawStream(rawBytes);
            QByteArray ba = processRawStream(rawBytes);
            parseTransformedStream(ba);
        }
    }
    m_finished = true;
    delete m_reply;//can't "deleteLater();" if the event loop stops...
    m_reply=0;
    quit();

    //emit download_finished();
}

void AbstractFeedReader::cacheRawStream(QByteArray& ba){
    QString cachePath = CatItem::itemCacheDir();
//            QDir::convertSeparators(
//            PUSER_APP_DIR
//            + FEED_CACHE_DIR
//            + "/"+ QString::number(stringHash(m_filt.getPath()))
//            + "."+ FEED_CACHE_SUFFIX);
    cachePath = cachePath + "/"+ QString::number(stringHash(m_filt.getPath()))
                + "."+ FEED_CACHE_SUFFIX;
    qDebug() << "AbstractFeedReader writing cache: " << cachePath;
    QFile cacheFile(cachePath);
    cacheFile.open(QIODevice::WriteOnly |QIODevice::Text);

    cacheFile.write((m_filt.getPath() + "\n").toAscii());
    cacheFile.write(ba);
    cacheFile.close();
}


void HttpHeadReader::run(){
    qDebug() << "Getting (Remote) URL: " << getRequestURL();
    if(getRequestURL().isEmpty()) {
        qDebug() << "error null URL";
        m_finished = true;
    } else {
        if(m_netAccessMan){m_netAccessMan->deleteLater();}
        m_netAccessMan = new QNetworkAccessManager;
        sendRequest();
        m_reader = new QXmlStreamReader(m_reply);
        QEventLoop loop;
        connect(m_reply, SIGNAL(readyRead()), this, SLOT(parseStreamIncrementallyNA()));
        connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()));
        connect(this, SIGNAL(done()), &loop, SLOT(quit()));
        loop.exec();
        qDebug() << "after incremental..";
        if(!m_finished){
            m_finished = true;
            parseStreamIncrementally();
        }
        delete m_reader;
        quit();
    }
}


void HttpHeadReader::sendRequest(){
    m_url = QUrl(getRequestURL());

    QNetworkRequest request(m_url);
    request.setRawHeader("User-Agent", ONELINE_AGENT_STR);

    m_reply = m_netAccessMan->get(request);
    if(!m_reply){
        qDebug() << "Failed to create reply object";
        m_finished = true;
        quit();
    }
}


//This does all the processing of the header at this point...
void HttpHeadReader::parseStreamIncrementally(qint64, qint64){

    qDebug() << "parsing (RSS?) header at:" << m_filt.getPath();
    bool linkFound = false;
    while(true){
        qDebug() << m_reader->tokenType() << " is the token type";
        if (m_reader->error()  ) {
            if(m_reader->error() == QXmlStreamReader::PrematureEndOfDocumentError){
                if(m_finished){
                    emit done();
                } else {
                    qDebug() << "Waiting for more data";
                }
                return;
            } else{
                qDebug() << "XSLT ERROR: " << m_reader->errorString() << ":" << this->m_url.toString();
                emit done();
                return;
            }
            break;
        } else if (m_reader->isStartElement()) {
            QString tag = m_reader->name().toString();
            qDebug() << "tag name:" << tag;
            if(tag.toLower()=="body"){
                m_reply->abort();
                emit done();
                m_finished = true;
                return;
            } else  if(tag.toLower()=="link" ){
                linkFound = true;
                QXmlStreamAttributes sa = m_reader->attributes();
                QStringRef t = sa.value("type");
                if(t == "application/rss+xml"){
                    QUrl url(m_filt.getPath());
                    QUrl br("/");
                    QUrl baseUrl = url.resolved(br);
                    QUrl relative(sa.value("href").toString());
                    qDebug() << "RSS host:" << baseUrl.toString();
                    qDebug() << "relative:" << relative.toString();
                    QString rss_link = baseUrl.resolved(relative).toString();
                    QString rss_title = sa.value("title").toString();
                    CatItem toReturn = m_filt;
                    toReturn.setRequestUrl(rss_link);
                    toReturn.setDescription(rss_title);
                    toReturn.setTagLevel(CatItem::EXTERNAL_SOURCE);
                    toReturn.setLabel(HTML_HEADER_PROCESSED_KEY);
                    toReturn.setIsUpdatableSource(true);
                    m_results.append(toReturn);
                }
            }
        } else if(m_reader->atEnd()){
            m_filt.setLabel(HTML_HEADER_PROCESSED_KEY);
            m_filt.setIsUpdatableSource(false);
            m_results.append(m_filt);
            emit done();
            return;
        }
        m_reader->readNext();
    }
}



QByteArray XSLTFeedReader::processRawStream(QByteArray response){
    qDebug() << " -- processing string response:---\n " ;
    QXmlQuery query(QXmlQuery::XSLT20);
    QBuffer xml(&response);
    xml.open(QIODevice::ReadOnly);
    if(!xml.isOpen()){
        qDebug() << "error openning response ";
        return QByteArray();
    }
    if(!xml.isReadable()){
        qDebug() << "error stream not readable";
        return QByteArray();
    }

    QString code;
    if(m_filt.hasLabel(STREAM_FILTER_SOURCE)){
        code = m_filt.getCustomString(STREAM_FILTER_SOURCE);
    } else if (m_filt.hasLabel(STREAM_FILTER_FILESOURCE)){
        //This is kludge 'cause I can't find
        //a nice way to tell the file where the home directory is
//        QString pathFile = (*ggSettings)->fileName();
//        int lastSlash = pathFile.lastIndexOf(QLatin1Char('/'));
//        if (lastSlash == -1){
//                qWarning() <<  "Empty home file";
//                quit();
//                return "";
//        }
//        pathFile = pathFile.mid(0, lastSlash);
//        pathFile = pathFile + "/" + m_filt.getFilterFilePath();
        QString pathFile = m_filt.getFilterFilePath();
        QFile codeFile(pathFile);
        if(!codeFile.exists()){
            qWarning() <<  "Empty Filter file: " << pathFile << '\n';
            return "";
        }
        codeFile.open(QIODevice::ReadOnly |QIODevice::Text);
        code = codeFile.readAll();
    }

    if(code.isEmpty()){
        qWarning() <<  "Empty Filter Str: " << m_filt.getPath() << '\n';
        quit();
        return "";
    }

    qDebug() << " -- query string code:---\n " << code;
    try {
        query.setFocus(&xml);
        query.setQuery(code);
    }  catch( char * str ) {
        qDebug() <<  "Exception raised in xslt query: " << str << '\n';
        quit();
        return "";
    }

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    try {
        query.evaluateTo(&buffer);
    }  catch( char * str ) {
        qDebug() <<  "Exception raised in xslt processing: " << str << '\n';
        quit();
        return "";
    }
    buffer.close();
    //qDebug() << "processed XML stream: \n\n" << ba;
    return ba;
}


//Do simple parsing of transformed
void XSLTFeedReader::parseTransformedStream(QByteArray transformedXML){

    QXmlStreamReader reader;
    reader.addData(transformedXML);
    CatItem res;
    do {
        res = takeAnItemFromReader(&reader);
        if(!res.isEmpty()) {
            res.addParent(m_filt,STREAM_SOURCE_PATH,BaseChildRelation::MESSAGE_SOURCE_PARENT);
            m_results.append(res);
        }
    } while( !(reader.atEnd()) && !(reader.error()) );
}


//This assumes the feed has been transformed into an ultra-simple
//XML fragment
CatItem XSLTFeedReader::takeAnItemFromReader(QXmlStreamReader* readerPtr){
    if(readerPtr->atEnd()){ return CatItem();}

    CatItem it;
    bool done = false;
    bool nameFound = false;
    QString currentTag("");
    while(!done){
        //qDebug() << readerPtr->tokenType() << " is the token type";
        if (readerPtr->isStartElement()) {
            QString tag = readerPtr->name().toString();
            if(tag.toLower()=="name" || tag.toLower()=="shortname")
                {if(nameFound) {break;} else {nameFound=true;} }
            if(tag!="root")
                { currentTag = tag; }
        } else if(readerPtr->isEndElement() && currentTag==XSLT_PLUGIN_END_TAG){
            done = true;
        } else if( readerPtr->isCharacters() ) {
            //QString curValue = readerPtr->readElementText();
            QString curValue = readerPtr->text().toString();
            //qDebug() << "Current Tag: " << currentTag;
            //qDebug() << "Current Tag Value: " << curValue;
            if(!currentTag.isEmpty() && !readerPtr->isWhitespace())
                { it.setValue(currentTag, curValue);}
        } else if (readerPtr->error() || readerPtr->atEnd() ) {
            if(readerPtr->error()){
                qDebug() << "XML ERROR:" << readerPtr->lineNumber() << ": " << readerPtr->errorString();
            } else{
                qDebug() << "XSLT ERROR: Premature End Of processed xml stream" << this->m_url.toString();
            }
            return CatItem();
        }
        readerPtr->readNext();
    }
    return it;
}



