#ifndef FB_REST_H
#define FB_REST_H
#include "feed_reader.h"

//Use Graph API instead
class FacebookREST_FeedReader : public XSLTFeedReader {
    Q_OBJECT

    FBRequest* m_fbrequest;
    FBSession* m_FBSession;
    FBUID m_FBUID;

public:
    FacebookREST_FeedReader(QObject* parent, RemoteItemSource filt);
    FacebookREST_FeedReader(QObject* parent, CatItem it);

    void run();
    void parseFBResult(const QVariant& aContainer);
    void requestFailedWithFacebookError ( const FBError& aError );

signals:
    void downloadDone();
public slots:

    void sessionDidLogin(FBUID);
    //void requestDidLoad(const QVariant&);
    void sessionDidLogout();

};

#endif // FB_REST_H
