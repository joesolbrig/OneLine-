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

#include <QString>

//#define PLUGIN_NAME "basePlugin"


#include "plugin_interface.h"


class PluginBase : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
    uint HASH_PluginBase;
	QString PLUGIN_NAME;
private:

public:
	PluginBase() {
		HASH_PluginBase = qHash(QString(PLUGIN_NAME));
	}
	~PluginBase() {}
	int msg(int msgId, void* wParam = NULL, void* lParam = NULL); 
	
    virtual void init(){}
    virtual void getLabels(InputList* id){ }
    virtual int getID(uint*){return HASH_PluginBase;}
	virtual void getName(QString*){};
    virtual void getResults(InputList*, QList<CatItem>* ){}
    virtual void getCatalog(QList<CatItem>* ){}
        virtual void launchItem(InputList*, CatItem*){}
    virtual QString getIcon(){return "";}
	virtual bool hasDialog() {return false;}
    virtual void doDialog(QWidget* , QWidget**){}
    virtual void endDialog(bool ){}
    virtual void afterLoad(QList<CatItem>* ){}
    virtual void load(QList<CatItem>* ){}
    virtual void save(QList<CatItem>* ){}
    virtual void modifyItem(CatItem*  ){}
	
};

extern PluginBase* gPluginBaseInstance;
