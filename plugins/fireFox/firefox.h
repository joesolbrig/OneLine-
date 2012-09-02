/*
oneline: Multibrowser and App Launcher
Copyright (C) 2012 Hans Solbrig

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


//These
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlDatabase>

#include "item.h"
#include "itemarglist.h"
#include "plugin_interface.h"

class FirefoxPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)


public:
	uint HASH_FIREFOX_PLUGIN;
private:

    QString templatePlacePath;
public:
    QString libPath;
    QSqlDatabase m_db;

    FirefoxPlugin() {
        HASH_FIREFOX_PLUGIN = FIREFOX_PLUGIN_ID;
	}
    ~FirefoxPlugin() {}
	int msg(int msgId, void* wParam = NULL, void* lParam = NULL); 

    //Plugin Interface Members...
    void getLabels(InputList* id);
    void getID(uint*);

    void getName(QString*);
    void getResults(InputList* id, QList<CatItem>* results);
    void getCatalog(QList<CatItem>* items);
    void extendCatalog(SearchInfo* existingItems, QList<CatItem>* outItems);


    //bool modifyItem(CatItem* id , UserEvent::LoadType lt);
    void launchItem(InputList* id, CatItem*);
    void doDialog(QWidget* parent, QWidget**);
    void endDialog(bool accept);
    void init();
    void setPath(QString *path);

    //Helpers
    void checkRefreshTempFile();
    QString getFirefoxPath();
    void createIcons(time_t time_from=0);
    void itemsFromQuery(QList<CatItem>* items, QString queryStr);

};

extern FirefoxPlugin* gmypluginInstance;
