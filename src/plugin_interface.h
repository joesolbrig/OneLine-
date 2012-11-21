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

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H


#include <QObject>
#include <QSettings>

#include "platform_util.h"
#include "item.h"
//#include "input_list.h"
#include "itemWSHash.h"
#include "itemarglist.h"

#ifdef Q_WS_WIN
#define VC_EXTRALEAN
#ifndef WINVER
#define WINVER 0x05100
#define _WIN32_WINNT 0x0510	
#define _WIN32_WINDOWS 0x0510 
#define _WIN32_IE 0x0600
#endif

#include <windows.h>
#include <shlobj.h>
#endif

#ifndef UNICODE
#define UNICODE
#endif




#define DESKTOP_WINDOWS 0
#define DESKTOP_GNOME 1
#define DESKTOP_KDE 2

/**
	\brief This message asks the Plugin for its ID Number.

         Application needs an unsigned int identification value for each loaded plugin.  You supply your own here.
	Typically, this is the result of hashing a string, as shown in the example below.
	\param wParam (uint*) That stores the resulting ID number.
	\warning Because we're hashing strings to integers.. it is theoretically possible that two plugin names will collide to the same plugin id.  

	\verbatim
	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_GET_ID:
				*wParam = qHash(QString("Weby_Plugin"))
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_GET_ID 0


/**
	\brief This message asks the plugin if it would like to apply a label to the current search query.
	
	 It is sometimes useful to label user queries with plugin-defined tags.  For instance, the weby plugin
		will tag input that contains "www" or ".com" or ".net" with the hash value of the string "HASH_WEBSITE".  Then,
		other plugins that see the query can know that the current search is for a website.\n\n
		The InputData class stores the current user's query.  It is in a List structure because each time "tab" is pressed by the user
		a new InputData is formed and appended to the list.  In other words, if the user typed "google <tab> this is my query" then
		wParam would represent a list of 2 InputData classes, with the first representing "google", and the second, "this is my query". Each InputData
		can be tagged individually.

	\warning This is called each time the user changes a character in his or her query, so make sure it's fast.

	\param wParam QList<InputData>* id
	\param lParam NULL

	\verbatim
	void WebyPlugin::getLabels(QList<InputData>* id)
	{
		if (id->count() > 1)
			return;

		// Apply a "website" label if we think it's a website
        QString & text = id->last().getUserKeys();

		if (text.contains("http://", Qt::CaseInsensitive))
			id->last().setLabel(HASH_WEBSITE);
		else if (text.contains("https://", Qt::CaseInsensitive)) 
			id->last().setLabel(HASH_WEBSITE);
		else if (text.contains(".com", Qt::CaseInsensitive)) 
			id->last().setLabel(HASH_WEBSITE);
		else if (text.contains(".net", Qt::CaseInsensitive))
			id->last().setLabel(HASH_WEBSITE);
		else if (text.contains(".org", Qt::CaseInsensitive))
			id->last().setLabel(HASH_WEBSITE);
		else if (text.contains("www.", Qt::CaseInsensitive))
			id->last().setLabel(HASH_WEBSITE);
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_GET_LABELS:
				getLabels((QList<InputData>*) wParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_GET_LABELS 1



/**
	\brief Asks the plugin for any results to a query

	 If your plugin returns catalog results on the fly to a query 
		(e.g. a website query for weby or a calculator result), then this is the place to do so.
		The existing results are stored in the list of CatItem's (short for Catalog Items) passed in as
		lParam and you can append your own results to it.

	\param wParam (QList<InputData>*): The user's query
	\param lParam (QList<CatItem>*): The list of existing results for the query, you can append your own

	\verbatim
        void WebyPlugin::getResults(QList<InputData>* id, QList<CatItem>* results)
	{
		// Is this query a website?  If so create a new result for the website
		if (id->last().hasLabel(HASH_WEBSITE)) {
            QString & text = id->last().getUserKeys();
			// This is a website, create an entry for it
			results->push_front(CatItem(text + ".weby", text, HASH_WEBY, getIcon()));
		}
		
		// Is this query of form:  website <tab> search_term?
		if (id->count() > 1 && id->first().getTopResult().id == HASH_WEBY) {
            QString & text = id->last().getUserKeys();
			// This is user search text, create an entry for it
			results->push_front(CatItem(text + ".weby", text, HASH_WEBY, getIcon()));
		}
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_GET_RESULTS:
				getResults((QList<InputData>*) wParam, (QList<CatItem>*) lParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_GET_RESULTS 2

/**
	\brief Asks the plugin for a static or semi-static catalog to be added to the primary catalog
         Some plugins will add permanent entries to application's primary catalog (until the catalog is rebuilt).
	For instance, weby adds firefox bookmarks into the primary catalog.  This is the function in which that is done.
	\param wParam (QList<CatItem>*): The catalog that you append your new entries to (these will be copied over to the primary catalog)
	\verbatim
    void WebyPlugin::getCatalog(QList<CatItem>* items)
	{
		// Add the default websites to the catalog "google/amazon/etc.."
		foreach(WebySite site, sites) {
			items->push_back(CatItem(site.name + ".weby", site.name, HASH_WEBY, getIcon()));
		}

		// If we're supposed to index firefox, then do that there
		if ((*settings)->value("weby/firefox", true).toBool()) {
			QString path = getFirefoxPath();
			indexFirefox(path, items);
		}
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_GET_CATALOG:
				getCatalog((QList<CatItem>*) wParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/

#define MSG_GET_CATALOG 3


/**
    \brief This message instructs the plugin that one of its own catalog items
    \brief was selected by the user and should now be executed

    If the plugin adds items to the catalog via MSG_GET_RESULTS or MSG_GET_CATALOG and one of those
    items is selected by the user then it is up to the plugin to execute it when the user presses
    "enter".  This is where the plugin performs the action.
    The plugin can perform application-specific actions:
        1) Tell application to stay visible and display the options menu
        2) Tell application to exit permanently
        3) Tell application to refresh the catalog and become invisible
        4) Tell application to add a specific "result set" of items
        5) Tell application to display a single item in "large format"
    \param wParam (InputList*): The user's query
    \param lParam QList<CatItem>* output: [Optional] results for action

	\verbatim
    void Plugin::launchItem(InputList* id, QList<CatItem>* output)
	{
    .........
	}

    int Plugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_LAUNCH_ITEM:
                launchItem(InputList*) wParam, (CatItem*) lParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_LAUNCH_ITEM 4


/**
	\brief This message informs the plugin that it's being loaded.  This is a good time to do any initialization work.

	\param wParam NULL
	\param lParam NULL

	\verbatim
	void calcyPlugin::init()
	{
	    // Do some initialization here if necessary..
	}

	int CalcyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_INIT:
				init();
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/


#define MSG_INIT 5

/**
	\brief This message asks the plugin if it has a dialog to display in the options menu.

	 The result of the request is returned via the result of the msg function (handled).

	\param wParam NULL
	\param lParam NULL

	\verbatim
	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
		case MSG_HAS_DIALOG:
			handled = true;
			break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_HAS_DIALOG 6

/**
	\brief This message tells the plugin that it's time to show its user interface

	\param wParam (QWidget*): The parent of the dialog to create
        \param lParam (QWidget**): Your plugin's new dialog is returned here for application to keep tabs on it

	\verbatim
	void WebyPlugin::doDialog(QWidget* parent, QWidget** newDlg) {
		// gui is a private member variable of class WebyPlugin
		if (gui != NULL) return;

		// class Gui is weby's user interface class
		gui = new Gui(parent);
		*newDlg = gui;
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_DO_DIALOG:
				doDialog((QWidget*) wParam, (QWidget**) lParam);
				break;
		}
		
		return handled;
	}
	\endverbatim
*/

#define MSG_DO_DIALOG 7


/**
	\brief This message asks the plugin for its string name

	\param wParam (QString*) The destination for the name of your plugin
	\param lParam NULL

	\verbatim
	void WebyPlugin::getName(QString* str)
	{
		*str = "Weby";
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_GET_NAME:
				getName((QString*) wParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_GET_NAME 8

/**
	\brief This message informs the plugin that it should close its dialog

	\param wParam (bool): Whether the plugin should accept changes made by the user while the dialog was open
	\param lParam NULL

	\verbatim
	void WebyPlugin::endDialog(bool accept) {
		if (accept) {
			gui->writeOptions();
			init();
		}
		if (gui != NULL) 
			delete gui;
		
		gui = NULL;
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_END_DIALOG:
				endDialog((bool) wParam);
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_END_DIALOG 9


/**
        \brief This message informs the plugin that application is now visible on the screen.

	\param wParam NULL
	\param lParam NULL
*/
#define MSG_APPLICATION_SHOWING 10

/**
        \brief This message informs the plugin that application is no longer visible on the screen.

	\param wParam NULL
	\param lParam NULL
*/
#define MSG_APPLICATION_HIDING 11


/**
	\brief Tells the plugin to parse items which have loaded from external storage
	
	This is called after items have been loaded. Plugins should use this method 
	to process items which have been saved using the persistString member. 

	\param wParam (QList<CatItem>*): The list of loaded items, 

	\verbatim
	void TagsPlugin::afterLoad(QList<CatItem>* results)
	{
        foreach(results.items){
            i = results.at();
            if(i.id == TAGS_HASH){
                Tag t(i.persistString);
                Tag_hash.setItem(t.key, t);
                results.at() = t.asItem();
            }
        }
	}

	\endverbatim
*/

#define MSG_AFTER_LOAD 14

/**
	\brief Tells the plugin to load items from external storage
	
	This is called to load items. 

	\param wParam (QList<CatItem>*): The list of  items, 

	\verbatim
	void TagsPlugin::Load(QList<CatItem>* results)
	{
        foreach(results.items){
            i = results.at();
            if(i.id == TAGS_HASH){
                Tag t(i.persistString);
                Tag_hash.setItem(t.key, t);
                results.at() = t.asItem();
            }
        }
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_HISTORY_LOADED:
				getResults((QList<InputData>*) wParam, (QList<CatItem>*) lParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_LOAD_ITEMS 15

/**
	\brief Tells the plugin to save items to storage
	
	This is called to save items to any persistant store...

	\param wParam (QList<CatItem>*): The list of  items, 

	\verbatim
	void TagsPlugin::Save(QList<CatItem>* results)
	{
        foreach(results.items){
            i = results.at();
            if(i.id == TAGS_HASH){
                Tag t(i.persistString);
                Tag_hash.setItem(t.key, t);
                results.at() = t.asItem();
            }
        }
	}

	int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_HISTORY_LOADED:
				getResults((QList<InputData>*) wParam, (QList<CatItem>*) lParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_SAVE_ITEMS 17

/**
	\brief This message asks the plugin to validate and fill-in extra information to the current search query.

	\warning This is called on short intervals when an item become visible so it should be reasonably fast

	\param wParam catItem* 
	\param lParam NULL

	\verbatim
    bool FileCatalogPlugin::validateItem(catItem* item, bool isValid)
	{
        //Make sure the file exists 
        ....
        
        //If it's directory, give both listing and 
        //parent...
	}

	int FileCatalogPlugin::msg(int msgId, void* wParam, void* lParam)
	{
		bool handled = false;
		switch (msgId)
		{
			case MSG_GET_LABELS:
				getLabels((QList<InputData>*) wParam);
				handled = true;
				break;
		}
		
		return handled;
	}
	\endverbatim
*/
#define MSG_VALIDATE_ITEM 18


/**
   \brief This message is sent to plugin so it can check if item it added to the database is still valid when it being loaded

   \param wParam QString path
*/
#define MSG_PATH 19


/**
    \brief This message telll the plugin that a catalog items, not necessarily it's own, was selected by the user
    and will be executed

    This allows the plugin to do further bookkeeping - IE, tags can be modified, etc.

    \param wParam (QList<InputData>*): The user's query
    \param lParam (CatItem*): The user selected catalog item

    \verbatim
    void WebyPlugin::ItemLaunched(QList<InputData>* id)
    {

    }

    \endverbatim
*/
#define MSG_ITEM_PREVIEW 32

/**
    \brief Sent to item so it can generate html preview information.
    \param wParam (QList<CatItem>*): existingItems and user keystrokes etc, lParam (QList<CatItem>*) items that are added
    \verbatim
    void FilePlugin::extendCatalog(SearchInfo* existingItems, QList<CatItem>* furtherItems )
    {
        foreach(CatItem item, existingItems) {
            //index subdirectories...
        }
        ....

    }

    \endverbatim*/



#define MSG_ITEM_LAUNCHED 20

/**
    \brief Offers the plugin to extend a list of items. These items are those previously marked
    for extension as well as those which have been "touched" by the user. Thus the file plugin
    will be able to progressively scan directories. Further, it will scan any directories which
    the user has been to.
    \param wParam (QList<CatItem>*): existingItems and user keystrokes etc, lParam (QList<CatItem>*) items that are added
    \verbatim
    void FilePlugin::extendCatalog(SearchInfo* existingItems, QList<CatItem>* furtherItems )
    {
        foreach(CatItem item, existingItems) {
            //index subdirectories...
        }
        ....

    }

    \endverbatim
*/

#define MSG_EXTEND_CATALOG 22

/**
    \brief Tell the plugin to stop extending a list of items. The thread doesn't have to exit
    immediately but any further activity will be ignoreed.
    \param  QList<CatItem>* furtherItems, QList<CatItem>* outItems ; same as extend msg info...

    void FilePlugin::StopExtendCatalog( QList<CatItem>* furtherItems, QList<CatItem>* outItems )
    {
        foreach(CatItem item, existingItems) {
            //index subdirectories...
        }
        ....

    }

    \endverbatim
*/

#define MSG_STOP_EXTENDING_CATALOG 23

/**
    \brief for each url hash, the plugin gives a source id to be used in a seperate thread for extending items...
    \param wParam (qint32): url hash, lParam (QSet<qint32>*) source id's to be returned
    \verbatim

    void Xsltplugin::getSourceIds(qint32 id, QSet<qint32> sourceIdList){

        if(idsByUlr.contains(id)){
            for(int i=0;i < idsByUlr[id].count();i++){
                sourceIdList.insert(idsByUlr[id][i]);
            }
        }
    }

    \endverbatim
*/

#define MSG_GET_SOURCE_ID 24

/**
    \brief for a given item, if it has children but they haven't been filled-in, this retrieves them
    \param wParam (qint32): CatItem* item, lParam (QList<CatItem>*) list of children
    \verbatim

    //This allows us to add items past our normal depth.
    //These can be new OR existing CatItems...
    void fileCataloguePlugin::getChildren(CatItem* id, (QList<CatItem>*) children){
        .....

    }

    \endverbatim
*/
#define MSG_GET_CHILDREN 24

/**
    \brief for a given item, add any last-minute information or even remove it
    \param wParam (qint32): CatItem* item
    \verbatim

    //This allows us to add items past our normal depth.
    //These can be new OR existing CatItems...
    void fileCataloguePlugin::modifyItem(CatItem* id ){
        .....

    }

    \endverbatim
*/
#define MSG_ITEM_LOADING 26
//Same except all at once
#define MSG_ITEMS_LOADING 27

/**
    \brief lets the plugin save items as they are created,
    \param wParam (qint32): CatItem* item
    \verbatim


    \endverbatim
*/
#define MSG_USER_CREATED_ITEM 28

/**
    \brief lets the plugin know a custom field is being load and lets it add other info (URL esp),
    \param wParam (qint32): CustomFieldInfo*
    \verbatim


    \endverbatim
*/

#define MSG_CUSTOM_FIELD_EDITING 29


/**
    \brief Search for item in the background...
    \param wParam SearchInfo* search data, lParam (QList<CatItem>*) items that are added
    \verbatim
    void FilePlugin::backgroundSearch(SearchInfo* existingItems, QList<CatItem>* furtherItems )
    {
        ....

    }

    \endverbatim
*/

#define MSG_BACKGROUND_SEARCH 30

/**
   \brief This message asks the plugin to load any of its own plugins and to return them.  This is for language binding plugins such as for python plugins.

   \param wParam QList<PluginInfo>*: The plugins controlled by the plugin
   \param lParam NULL
*/

#define MSG_LOAD_PLUGINS 100

/**
   \brief This message asks the plugin to unload a plugin.

   \param wParam uint: The plugin ID
   \param lParam NULL
*/
#define MSG_UNLOAD_PLUGIN 101


#define MSG_NO_ACTION 499
#define MSG_CONTROL_OPTIONS 500
//#define MSG_CONTROL_EXIT 501
#define MSG_CONTROL_REBUILD 502
#define MSG_TAKE_OUTPUT 503
#define MSG_EXECUTE_RESULT 504
#define MSG_SHOW_ITEMS 505
#define MSG_SHOW_ERR 506

//A kludge to call QThread::sleep
extern bool gThrottleUpdates;

class MySleeper :public QThread {
    QThread* ptr_thread;
public:

    MySleeper(){}
    MySleeper(QThread* t) { ptr_thread = t;}
    static void sleepNow(){
        if(gThrottleUpdates>0){
            MySleeper s(QThread::currentThread());
            s.ptr_thread->msleep(gThrottleUpdates);
        }
    }
    void mysleep(long i){
        QThread::sleep(i);
    }

};

#define THREAD_SLEEP MySleeper::sleepNow()

class Catalog;

/**
	\brief This is the class that a plugin must implement.

        The PluginInterface is the interface between application and your plugin.
        application communicates with your plugin through the msg function.  To store
        any settings in application's permanent configuration file, please
	refer to the settings member variable.
*/
class PluginInterface
{
public:
	virtual ~PluginInterface() {};


	/**
		\brief This is how messages are passed to the plugin

		Here you handle the subset of messages that your plugin requires.  At minimum, your plugin must
			handle MSG_GET_ID and MSG_GET_NAME.  For unhandled messages, return 0.  For handled messages, return 1.
		\param msgId The type of the message, such as show dialog, initialize, or get results 
		\param wParam If this message type requires any parameters, this will be the first and casted to void*
		\param lParam If this message requires two parameters, this is the second, casted to void*
		\return 1 means the message was handled, 0 means it was ignored

		\verbatim
		int WebyPlugin::msg(int msgId, void* wParam, void* lParam)
		{
			bool handled = false;
			switch (msgId)
			{		
				case MSG_INIT:
					init();
					handled = true;
					break;
				case MSG_GET_LABELS:
					getLabels((QList<InputData>*) wParam);
					handled = true;
					break;
				case MSG_GET_ID:
					getID((uint*) wParam);
					handled = true;
					break;
				case MSG_GET_NAME:
					getName((QString*) wParam);
					handled = true;
					break;
				case MSG_GET_RESULTS:
					getResults((QList<InputData>*) wParam, (QList<CatItem>*) lParam);
					handled = true;
					break;
				case MSG_GET_CATALOG:
					getCatalog((QList<CatItem>*) wParam);
					handled = true;
					break;
				case MSG_LAUNCH_ITEM:
					launchItem((QList<InputData>*) wParam, (CatItem*) lParam);
					handled = true;
					break;
				case MSG_HAS_DIALOG:
					handled = true;
					break;
				case MSG_DO_DIALOG:
					doDialog((QWidget*) wParam, (QWidget**) lParam);
					break;
				case MSG_END_DIALOG:
					endDialog((bool) wParam);
					break;

				default:
					break;
			}
				
			return handled;
		}
		\endverbatim

	*/
	virtual int msg(int msgId, void* wParam = NULL, void* lParam = NULL) = 0;

    //This allows every item to identified as uniquely as possible by its path
    // ~/Documents == /home/bob/Documents/
    virtual QString uniquePath(QString url){return url;}


	/** The setting manager in which you store your persistent data.  More data can be found in QT's documentation for QSettings

	Following is an example of using the settings variable:

	\verbatim
	QSettings* set = *settings;

	// Write a value to the config file
	set->setValue("weby/version", 1.0);

	// Read a value from the config value
	double value = set->value("weby/version", 0.0);
	\endverbatim
	*/
    QSettings** settings;
    //Catalog* theCatalog;
    PlatformBase* thePlatform;
    QString itemPath;
    QMutex* gMutexPtr;
    virtual void getName(QString*)=0;
    //static QString ms_homeDir;
    static QChar* ms_homeDirStr;
    static int ms_homeDirLen;

public:

    void addHomeDir(QString home){
        QChar* str = home.data();
        ms_homeDirLen = home.length();
        ms_homeDirStr = new QChar[ms_homeDirLen];
        memcpy(ms_homeDirStr,str,ms_homeDirLen*sizeof(QChar));
        Q_ASSERT(getHomeDir() == home);
    }

    static QString getHomeDir(){
        return QString(ms_homeDirStr, ms_homeDirLen);

    }

    CatItem getPluginRep(){
        QString pluginPath;
        getName(&pluginPath);
        pluginPath = addPrefix(PLUGIN_PATH_PREFIX,pluginPath);
        CatItem me(pluginPath);
        return me;
    }

};


/**
	\brief A convienience run function

	 This function will run the program along with arguments and is platform independent.
        \param command line to run
	\param args The arguments to the command
	*/
void runProgram(QString file);
int getDesktop();


//This has changed, should I declare another verision?
 Q_DECLARE_INTERFACE(PluginInterface, "net.oneline.PlatformBase/1.0")


#endif
