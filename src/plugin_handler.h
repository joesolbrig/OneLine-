#ifndef PLUGIN_HANDLER_h
#define PLUGIN_HANDLER_h
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


#include <QPluginLoader>
#include <QHash>
//#include <QLayout>
#include "input_list.h"
#include "plugin_interface.h"
//#include "item.h"


class PluginInterface;
struct SearchInfo;

struct ExtendConfigInfo{

};

struct PluginInfo {
        int id;
	QString name;
        QString internalName;
	QString path;
	PluginInterface* obj;
	bool loaded;

        int baseLoadInterval;
        int workingloadInterval;
        int lastLoadTime;

        int baseExtendInterval;
        int workingExtendInterval;
        int lastExtendTime;

	~PluginInfo() {
        //We'll unload plugins if they start being a pain...
        //if(name!="Plugin Loader"){
        //    QPluginLoader loader(path);
        //    loader.unload();
        //}
	}
};

class InputData;
class CatItem;

class PluginHandler {
    private:
        QHash<QString, PluginInfo> plugins;

        void SetPluginForChildren(QString pluginName, QList<CatItem>* items);

        int adjustForSpeed(int baseInt, int curInt, int dur){
            int res = ((INTERVAL_ADJ_PROP - 1) *curInt + dur*EXTENDING_PROP)/INTERVAL_ADJ_PROP;
            res = MIN(res, baseInt*EXTENDING_PROP);
            res = MIN(res, curInt*2);
            return res;
        }

    public:

        PluginHandler();
        ~PluginHandler();

        QList<CatItem> loadPlugins(QHash<QString, QList<QString> > dirs, QSet<QString> pluginNames=QSet<QString>());
        void LoadPluginFromFile(QDir pluginsDir, QString fileName);
        void showTheApp();
        void hideTheApp();
        void getLabels(InputList* id);
        void getResults(InputList*, QList<CatItem>* results);
        void getCatalogs(QList<CatItem>* outItems);
        bool loadCatalogs(QList<CatItem>* outItems);
        void getSourceIds(qint32 id, QSet<qint32>* sourceIdList);
        void doACatalogueExtension(SearchInfo* search_info, QList<CatItem>* outItems);
        void doTestCatalogueExtension(QString plugin, SearchInfo* search_info, QList<CatItem>* outItems);
        void extendCatalogs(SearchInfo* existingItems, QList<CatItem>* outItems);
        void backgroundSearch(SearchInfo* search_info, QList<CatItem>* outItems);
        void stopExtending( SearchInfo* existingItems=0);
        void saveCatalogs(QList<CatItem>* items);
        bool itemLoading(CatItem* outItems, UserEvent::LoadType lt);
        void itemsLoading(SearchInfo* items , QList<CatItem>* res);
        void getCustomFieldInfo(CustomFieldInfo* cfi);
        int execute(InputList*, QList<CatItem>*,QString* errString);
        void preExecute(InputList*);
        void userItemsCreated(QList<CatItem>* inList);
        QWidget* doDialog(QWidget* parent, uint id);
        void endDialog(uint id, bool accept);
        QHash<QString, PluginInfo> & getPlugins() { return plugins; }

    protected:

        void saveCreatedItem(CatItem item);


};


#endif
