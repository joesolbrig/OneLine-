#ifndef RECURSIVE_JSON_H
#define RECURSIVE_JSON_H

#include "feed_reader.h"

//This iteratively downloads a tree
//from multiple parts.

class RecursiveStream : public AbstractReader {
    CatItem m_arg;
    XSLTFeedReader* m_xsltDownloader;
    JsonReader* m_jsonDownloader;

    QSet<QString> m_extendedURLS;
    QObject* m_parent;

public:
    RecursiveStream(QObject* parent, CatItem it, CatItem arg);
    virtual void run();
    bool  extendList(QList<CatItem>& it, int recur);
    QList<CatItem>  extendChildren(CatItem& it, int recur, bool* hasExtendedPtr=0);
    QList<CatItem> extendItem(CatItem& it, CatItem arg=CatItem(), bool* hasExtendedPtr=0);
    virtual QByteArray processRawStream(QByteArray ){return "";};
    virtual void parseTransformedStream(QByteArray ) {};
    bool relevantToItem();
};

#endif // RECURSIVE_JSON_H
