/*
Oneline: Multibrowser
Copyright (C) 2012  Hans Joseph Solbrig

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#pragma once

#include <QtNetwork>

#include <QUrl>

#include <QtXmlPatterns>
#include <QXmlQuery>
#include <QXmlSimpleReader>
#include "item.h"

#include "plugin_interface.h"
#include "feed_reader.h"

class AbstractReader;

class TwitterReader;

extern QSettings** ggSettings;

class StreamPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:
    uint HASH_streamPlugin;

private:
    QList<CatItem> m_filters;

    QString keys;
    QString tags;
    QString contacts;
    qint64 timeCutoff;
    QMutex itemMutex;

    TwitterReader* mp_twitterReader;

    QList<CatItem> m_pluginGenerators;
    //QList<AbstractReader*> m_feedDownloaders;


public:
    StreamPlugin(QObject* parent=0): QObject(parent), m_filters()  {
        HASH_streamPlugin = qHash(QString(STREAMPLUGIN_NAME));
	}
    ~StreamPlugin() {}
	int msg(int msgId, void* wParam = NULL, void* lParam = NULL); 

    //implements plugin interface
    void getLabels(InputList* id);
    void getID(uint*);
    void getName(QString*);
    void getResults(InputList* id, QList<CatItem>* results);
    void getCatalog(QList<CatItem>* items);
    bool extendCatalog(SearchInfo* info, QList<CatItem>* results);
    bool itemLoading(CatItem* it, UserEvent::LoadType loadType);
    void stopExtendingCatalog();
    bool getCustomFieldInfo(CustomFieldInfo* cfi);

    void launchItem(InputList* id, CatItem*);
    void doDialog(QWidget* parent, QWidget**);
    void endDialog(bool accept);
    void init(QSettings* set=0);
    QString getIcon();
    QList<AbstractReader*> createStreamReader(CatItem it, UserEvent::LoadType);
    QList<AbstractReader*> createIconReader(CatItem it);

    bool getFeedReaders(SearchInfo* info, QList<CatItem>* r);
    QList<CatItem> runFeedReaders(QList<AbstractReader*> feedDownloaders);
    QList<CatItem> launchFeedReaders(QList<AbstractReader*>& feedDownloaders);


private:

signals:
    void end_current_downloads();

};




extern StreamPlugin* gmypluginInstance;
