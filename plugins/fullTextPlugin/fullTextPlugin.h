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
#ifndef FULL_TEXT_PLUGIN_H
#define FULL_TEXT_PLUGIN_H
#pragma once

#include "directory.h"
#include "plugin_interface.h"
#include "platform_base.h"

#include "recoll_interface.h"
#include "input_list.h"

const int MAX_FILE_SIZE=1000000;
const int FILE_PATH_XAPIAN_ID = 1;
const int SECTION_DISPLAY_PADDING = 10;

extern const int HASH_FILE_CATALOG;

const int MAX_SEARCH_ITEMS = 20;

//class DBInitor : public QMutexLocker {
//    DBInitor(QMutex& mut) : QMutexLocker(mut){
//        if(info->m_subthread){
//            recoll_threadinit();
//        }
//        if(!m_recoll_interface->openDB(true)){
//            return;
//        }
//    }
//
//    ~DBInitor(){
//        if(info->m_subthread){
//            m_recoll_interface->closeDB();
//        }
//    }
//
//};


class FullTextPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
    uint HASH_FULLTEXT_CATALOG;
    Recoll_Interface* m_recoll_interface;
    const QString pluginName;
private:
    QString libPath;
    PlatformBase * platform;
    QMutex m_fullTextMutex;
public:
    FullTextPlugin(): HASH_FULLTEXT_CATALOG(qHash(QString(FULL_TEXT_PLUGIN_NAME))),
        pluginName("fulltext plugin") {
    }
    ~FullTextPlugin();

    int msg(int msgId, void* wParam = NULL, void* lParam = NULL);

    void setPath(QString * path);
    void getLabels(InputList* id);
    void getID(uint*);
    void getName(QString*);
    void search(SearchInfo* searchInfo, QList<CatItem>* results);
    void getCatalog(QList<CatItem>* items);
    //bool modifyItem(CatItem* id);
    void itemsLoaded(SearchInfo* inf, QList<CatItem>* res);
    bool itemLoaded(CatItem* itemPtr, UserEvent::LoadType lt);
    void indexAFileItem(CatItem& id, QFileInfo fi, QList<CatItem>* r, bool compound=true);
    QString cachePath();
    void launchItem(InputList*, QList<CatItem>* output);
    //int ItemsLoaded(CatItem *id);
    void extendCatalog(SearchInfo* info, QList<CatItem>* r);

    void doDialog(QWidget* parent, QWidget**);
    void endDialog(bool accept);
    void init();
    QString getIcon();
//    void indexDirectory(QList<CatItem>* items, QString dir,
//        QStringList filters, bool fdirs, bool fbin, int depth);
    CatItem CreateFullTextItem(RecollQueryItem it, QList<CatItem>* items);
    CatItem createFileItem(QString path, bool isDir=false);
//    CatItem createKeywordItem(QString keyword, QList<CatItem>* items);
};

extern FullTextPlugin* gFullTextPluginInstance;
#endif
