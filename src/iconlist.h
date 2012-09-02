#ifndef ICONLIST_H
#define ICONLIST_H

#include <QIcon>
#include "list_item.h"




class IconList
{
    struct IconEntry{
        QString suffix;
        QString defaultMimeType;
        QIcon icon;
    };

    QHash <QString, IconEntry> m_bySuffix;
    QHash <QString, QIcon> m_byMimeType;
    QHash <QString, QIcon> m_byPath;


public:
    IconList();

    void addIcon(ListItem& li);

};

#endif // ICONLIST_H
