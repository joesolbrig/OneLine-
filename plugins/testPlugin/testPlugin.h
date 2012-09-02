/*
Oneline: Multibrower and Application Launcher
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

#define TEST_PLUGIN_NAME "TestPlugin"

#include "input_list.h"
#include "plugin_interface.h"


class testpluginPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)


public:
    uint TEST_HASH;
private:
    QString libPath;

public:
    testpluginPlugin() {
        TEST_HASH = qHash(QString(TEST_PLUGIN_NAME));
	}
    ~testpluginPlugin() {}
	int msg(int msgId, void* wParam = NULL, void* lParam = NULL); 

        void getLabels(InputList* id);
	void getID(uint*);
	void getName(QString*);
        void getResults(InputList* id, QList<CatItem>* results);
	void getCatalog(QList<CatItem>* items);
    int launchItem(InputList* id, QList<CatItem>*);
	void doDialog(QWidget* parent, QWidget**);
	void endDialog(bool accept);
    void AfterLoad(QList<CatItem>* results);

	void init();
	QString getIcon();
    void setPath(QString * path);
    void extendCatalog(SearchInfo* info, QList<CatItem>* r);
};

extern testpluginPlugin* gmypluginInstance;
