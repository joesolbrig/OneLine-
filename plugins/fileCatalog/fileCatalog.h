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


#include "directory.h"
#include "plugin_interface.h"
#include "platform_base.h"
#include "input_list.h"

class FilecatalogPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
    static uint HASH_FILE_CATALOG;

private:
    QString libPath;
public:
    FilecatalogPlugin() {
        HASH_FILE_CATALOG = (qHash(QString(FILE_CATALOG_PLUGIN_STR)));
    }
    ~FilecatalogPlugin() {}

    int msg(int msgId, void* wParam = NULL, void* lParam = NULL);

    void setPath(QString * path);
    void getLabels(InputList* id);
    void getID(uint*);
    void getName(QString*);
    void getResults(InputList* id, QList<CatItem>* results);
    void getCatalog(QList<CatItem>* items);
    void extendCatalog(SearchInfo* existingItems, QList<CatItem>* outItems);
    bool modifyItem(CatItem* id, UserEvent::LoadType lt);
    void launchItem(InputList* il, QList<CatItem>* output);
    void doDialog(QWidget* parent, QWidget**);
    void endDialog(bool accept);
    void init();
    QString getIcon();

protected:
    void indexDirectory(QSet<QString> &extendedTypes, CatItem& parentItem,
        QList<CatItem>* output, UserEvent::LoadType lt=UserEvent::IGNORE,
        bool addChild=false, int overrideMaxPasses=-1);
    void alterItem(CatItem* item);
    CatItem createFileItem(QSet<QString> &extendedTypes, QFileInfo fileInfo, CatItem& parent, int depth, UserEvent::LoadType lt=UserEvent::IGNORE);
    CatItem parseOnelineFile(QFileInfo fileInfo);
    QList<Directory> getInitialDirs();



};

extern FilecatalogPlugin* gFileCatalogPluginInstance;
