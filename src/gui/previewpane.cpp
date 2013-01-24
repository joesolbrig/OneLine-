#include <QWebFrame>
#include <QWebInspector>
#include "previewpane.h"

#include "appearance_window.h"


//#define TEST_WEBPREVIEW 1

QWebInspector* PreviewPane::mp_inspectorPage=0;

PreviewPane::PreviewPane(QWidget *parent) : QWebView(parent), m_item() {
    m_reply = 0;
    setWindowFlags(Qt::FramelessWindowHint);
    setVisible(false);
    m_imageLabel.setVisible(false);
    setHtml(HTTP_CONTENT_LOADING_MSG);
    setMouseTracking(true);
    connect(this, SIGNAL(loadFinished(bool)),this, SLOT(finishLoading(bool)));
    if(gMainWidget){
        hookHover(gMainWidget);
    }
    settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
    settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls,true);
    settings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
    settings()->setAttribute(QWebSettings::AutoLoadImages,true);
//#ifdef TEST_WEBPREVIEW
//    if(!mp_inspectorPage){ mp_inspectorPage= new QWebInspector;}
//    settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
//    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
//    mp_inspectorPage->setPage(page());
//    mp_inspectorPage->raise();
//    connect(page(), SIGNAL(webInspectorTriggered(QWebElement)),
//            mp_inspectorPage, SLOT(show()));
//    mp_inspectorPage->setVisible(true);
//#endif
//
//    QWebInspector wi = new QWebInspector;
//    wi.setPage(page());

}


void PreviewPane::setGeometry(QRect r){
    //m_imageLabel.setGeometry(r);
    QWebView::setGeometry(r);
    //page()->setViewportSize(r.size());
}

void PreviewPane::paintEvent ( QPaintEvent * event ){
    QWebView::paintEvent(event);

//    QPainter painter(this);
//    QRect extent = geometry();
//    extent.setTopLeft(QPoint(0,0));
//
//    QPen pen;
//    pen.setColor(Qt::black);
//    pen.setWidth(1);
//    extent.adjust(1,1,-1,-1);
//    painter.save();
//    painter.setPen(pen);
//    painter.drawRect(extent);
//    painter.restore();

}

bool PreviewPane::setItem(CatItem it, bool justTesting) {
    m_item = it;
    m_loadType = UNKNOWN_HTML;
    if(!justTesting){
        settings()->setFontSize(QWebSettings::MinimumFontSize,18);
        settings()->setFontSize(QWebSettings::MinimumLogicalFontSize,18);
        settings()->setFontSize(QWebSettings::DefaultFontSize,18);
        settings()->setFontSize(QWebSettings::DefaultFixedFontSize,18);
    //    QString styleSheetPath = (USER_APP_DIR + BROWSER_OPTION_DIR + BROWSER_STYLE_SHEET);
        QString styleSheetPath = m_item.getPreviewCSSPath();
        settings()->setUserStyleSheetUrl(QUrl(styleSheetPath));
    }
    if(justTesting ){
        if((int)it.getTagLevel() >=(int)CatItem::KEY_WORD){
            return false;
        }
        if(it.getItemType() == CatItem::LOCAL_DATA_FOLDER){
            return false;
        }
        QFileInfo f(it.getPath());
        if(f.isDir()){
            return false;
        }
    }


    if(!justTesting){
        setVisible(false);
    }
    if(it.hasLabel(PREVIEW_URL_KEY)){
        if(!justTesting){
            QUrl url(it.getCustomString(PREVIEW_URL_KEY));
            qDebug() << "setting url:" << url.toString();
            setUrl(url);
            load(url);
            loadingStart();
        }
        return true;
    } else if(it.hasLabel(FIREFOX_PLUGIN_NAME)
        && !it.hasLabel(STREAM_SOURCE_PATH)){
        if(!justTesting){
            setHtml (HTTP_CONTENT_LOADING_MSG);
            QUrl url = it.previewUrl();


            qDebug() << "setting url:" << url.toString();
            //startGettingUrlHtml(url);
            setUrl(url);
            load(url);
            loadingStart();
            //setReaderScript(true);
            connect(this, SIGNAL(loadProgress(int)),this, SLOT(progressLoading(int)));
        }
        return true;
    } else if(it.getMimeType() == DEFAULT_HTML_MIME_TYPE &&
              it.hasLabel(FILE_CATALOG_PLUGIN_STR)){
        if(!justTesting){
            QString urlStr = QString("file:/") + it.getPath();
            qDebug() << "loading urlStr: " << urlStr;
            QFile f(it.getPath());
            if(f.open(QIODevice::ReadOnly)){
                QString html = f.readAll();
                setHtml(html);
                setVisible(true);
                readyToShow();
            }
        }
        return true;
    } else if(it.getMimeType() == DEFAULT_TEXT_MIME_TYPE &&
              it.hasLabel(FILE_CATALOG_PLUGIN_STR)){
        if(!justTesting){
            QString pathStr = it.getPath();
            qDebug() << "loading pathStr: " << pathStr;
            QString h = htmlizeFile(pathStr);
            //qDebug() << "html str: " << h;
            m_loadType = CUSTOM_HTML;
            setHtml(h);
            readyToShow();
        }
        return true;

    } else if(!it.getPreviewHtml().isEmpty()){
        if(!justTesting){
            m_loadType = CUSTOM_HTML;
            QString preview = it.getPreviewHtml();
            qDebug() << "PreviewPane::setItem preview: " << preview;
            setHtml(preview);
            readyToShow();
        }
        return true;
    } else if(it.hasLabel(LONG_TEXT_KEY_STR) && !it.getLongText().isEmpty()){
        if(!justTesting){
            qDebug() << "loading long text item: " << it.getPath();
            QString h = htmlizeStringLines(it.getLongText());
            //qDebug() << "html str: " << h;
            m_loadType = CUSTOM_HTML;
            setHtml(h);
            readyToShow();
        }
        return true;
    } else if(it.hasLabel(TEMPORARY_LONG_HTML_KEY_STR)){
        if(!justTesting){
            qDebug() << "loading temp long item: " << it.getPath();
            QString h = (it.getTemporaryLongHtml());
            //qDebug() << "html str: " << h;
            m_loadType = CUSTOM_HTML;
            this->setHtml(h);
            readyToShow();
        }
        return true;
    } else {
        return false;
    }
}

void PreviewPane::startGettingUrlHtml(QUrl url){
    Q_ASSERT(!m_reply);
    QNetworkAccessManager netAccessMan;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", ONELINE_AGENT_STR);
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "Keep-Alive");
    m_reply = netAccessMan.get(request);
    if(!m_reply){
        qDebug() << "empty reply error";
        setHtml(htmlize(URL_LOAD_ERROR));
        return;
    } else if (m_reply->error()){
        qDebug() << "Start Error:" << m_reply->errorString();
        qDebug() << "Error #:" << m_reply->error();
        setHtml(htmlize(URL_LOAD_ERROR));
    }

    QEventLoop loop;
    connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    loop.exec();
    urlLoaded();

    //connect(m_reply, SIGNAL(finished()), this, SLOT(urlLoaded()));

}

void PreviewPane::urlLoaded(){
    Q_ASSERT(m_reply);
    qDebug() << "reply url" <<  m_reply->url();
    if (m_reply->error() != QNetworkReply::NoError) {
        setHtml(htmlize(URL_LOAD_ERROR));
        return;
    }
    QVariant attr =m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(!attr.canConvert(QVariant::Int)){
        qDebug() << ("no http return code");
        setHtml(htmlize(URL_LOAD_ERROR));
        return;
    } else if(attr.toInt()!=200){
        qDebug() << "return code:" << attr.toInt();
        if(attr.toInt()==301){
            setHtml(htmlize(URL_REDIRECT_MSG));
            QUrl newURL = m_reply->header(QNetworkRequest::LocationHeader).toUrl();
            delete (m_reply);
            m_reply = 0;
            startGettingUrlHtml(newURL);
            return;
        }
        setHtml(htmlize(URL_LOAD_ERROR));
        return;
    }

    QByteArray rawBytes = m_reply->readAll();
    setContent(rawBytes,"",m_reply->url());
    //setReaderScript();
    delete ( m_reply);
    m_reply = 0;
}



void PreviewPane::show(){
    this->setVisible(true);
    if(!m_item.getCustomPluginVariant(PREVIEW_IMAGE_KEY).isNull()){
        QVariant v = m_item.getCustomPluginVariant(PREVIEW_IMAGE_KEY);
        if (v.canConvert<QPixmap>()){
            QPixmap pm = v.value<QPixmap>();
            m_imageLabel.setPixmap(pm);
            m_imageLabel.setVisible(true);
            m_imageLabel.setGeometry(this->geometry());
        }
        Q_ASSERT(false);
    } else {
        m_imageLabel.setVisible(false);
        QWebView::show();
        raise();
    }
    update();
}

void PreviewPane::debugOut(QString msg) {
    qDebug() << "javascript out msg:" << msg;
}

//QString selectJS="window.find(\"%1\")";
//    page()->mainFrame()->evaluateJavaScript(script);


void PreviewPane::searchForString(QString str){
    findText(str);
//    QString selectJS;
//    if(!str.isEmpty()){
//        selectJS="window.find(\"%1\")";
//        selectJS.arg(str);
//    } else {
//        selectJS="window.getSelection().empty()";
//    }
//    page()->mainFrame()->evaluateJavaScript(selectJS);
}

void PreviewPane::setReaderScript(bool setOnload){
    if(m_loadType != UNKNOWN_HTML){ return;}
    if(m_item.getPreviewCSSPath().isEmpty()){ return;}
    QString preamble = QString("ccsPath='") + m_item.getPreviewCSSPath()+"';\n";
    if(m_item.getJavascriptFilterFilePath().isEmpty()){return;}
    QString js_path = m_item.getJavascriptFilterFilePath();
    qDebug() << "trying to open javascript file:" << js_path;
    QFile f(js_path);
    if(!f.open(QIODevice::ReadOnly)){ return;}
    QString script = f.readAll();
    script = preamble + script;
    if(script.isEmpty()){ return;}
    qDebug() << "evaluating: " << js_path << "setOnload" << setOnload;
    //qDebug() << "js body: " << script ;

    //Obviously, none of our code needs to be "cross-browser"
    if(setOnload){
        script.append("\n if((typeof onelineReadHook=='undefined') "
                      "&& oneLineListen && document.addEventListener) {"
            "document.addEventListener(\"DOMContentLoaded\", reader.init, false);"
            "onelineReadHook = true;"
        "}");
    } else {
        script.append("\n reader.init();");
    }
    page()->mainFrame()->evaluateJavaScript(script);

#ifdef TEST_WEBPREVIEW
    if(!mp_inspectorPage){ mp_inspectorPage= new QWebInspector;}
    settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
    mp_inspectorPage->setPage(page());
    connect(page(), SIGNAL(webInspectorTriggered(QWebElement)),
            mp_inspectorPage, SLOT(show()));
    mp_inspectorPage->setVisible(true);
    mp_inspectorPage->raise();
#endif

}

void PreviewPane::finishLoading(bool success){
    if(!success){ return;}
    //show();
    setReaderScript();
    readyToShow();

}

void PreviewPane::readyToShow(){

    QMutexLocker locker(&gMainWidget->m_itemChoiceList->m_previewMutex);
    setVisible(true);
    if(gMainWidget && gMainWidget->m_itemChoiceList &&
       gMainWidget->m_itemChoiceList->showingSidePreview()){
        gMainWidget->m_itemChoiceList->m_previewFrame->show();
        gMainWidget->m_itemChoiceList->previewLoaded();
    }

}

void PreviewPane::loadingStart(){
    hide();
    if(gMainWidget
            &&  gMainWidget->m_itemChoiceList
            &&  gMainWidget->m_itemChoiceList->m_previewFrame){
        gMainWidget->m_itemChoiceList->m_previewFrame->hide();
    }
}

void PreviewPane::progressLoading(int /*level*/){
    //if(level>10 && level <15){ setReaderScript(true); }
}

