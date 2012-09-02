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

#include "item.h"
#include "plugin_interface.h"


class TagsPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)


public:
    //Ought to be an enum somehow but this works..
    uint HASH_TAG_PLUGIN;
    uint TAG_TARGET_ITEM;
    uint TAG_NAME_ITEM;
    uint TAG_ACTION;
    uint TAG_DEFAULT_TAG_ACTION;
    uint TAG_COMBINED_ACTION;

private:
    qint64 tagActionId;

public:
    QString libPath;
    TagsPlugin() {
        HASH_TAG_PLUGIN = qHash(QString(TAG_LABEL));
        TAG_TARGET_ITEM = qHash(QString(TAGGED_ITEM_LABEL));
        TAG_NAME_ITEM = qHash(QString(TAG_ACTION_ITEM_LABEL));
        TAG_ACTION = qHash(QString(TAG_ACTION_LABEL));
        TAG_DEFAULT_TAG_ACTION = qHash(QString(TAG_DEFAULT_TAG_ACTION_LABEL));


        //TAG_COMBINED_ACTION = qHash(QString("Tag plugin combined action item"));
	}
	~TagsPlugin() {}
	int msg(int msgId, void* wParam = NULL, void* lParam = NULL); 
	
	//Pluggin Interface...

    void setPath(QString * path);
    void getLabels(InputList* id);
	void getID(uint*);
	void getName(QString*);
        void getResults(InputList* id, QList<CatItem>* results);
	void getCatalog(QList<CatItem>* items);
    void itemLoaded(CatItem *id);
    //int launchItem(InputList* id, QList<CatItem>* results);
	void doDialog(QWidget* parent, QWidget**);
	void endDialog(bool accept);
	void init();
    void afterLoad(QList<CatItem>* items);

	QString getIcon();
};

extern TagsPlugin* gmypluginInstance;
