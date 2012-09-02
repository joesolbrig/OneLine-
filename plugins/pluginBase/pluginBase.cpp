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

#include <QtGui>
#include <QUrl>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>

#ifdef Q_WS_WIN
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#endif

#include "pluginBase.h"

PluginBase* gmypluginInstance = NULL;

int PluginBase::msg(int msgId, void* wParam, void* lParam)
{
	bool handled = true;
	switch (msgId) {		
		case MSG_INIT: init(); break;
		case MSG_GET_LABELS: getLabels((QList<InputData>*) wParam);
			break;
		case MSG_GET_ID: getID((uint*) wParam); break;
		case MSG_GET_NAME: getName((QString*) wParam); break;
		case MSG_GET_RESULTS: getResults((QList<InputData>*) wParam, (QList<CatItem>*) lParam);
			break;
		case MSG_GET_CATALOG: getCatalog((QList<CatItem>*) wParam); break;
		case MSG_LAUNCH_ITEM: launchItem((QList<InputData>*) wParam, (CatItem*) lParam);
			break;
		case MSG_HAS_DIALOG: handled = hasDialog(); break;
		case MSG_DO_DIALOG: doDialog((QWidget*) wParam, (QWidget**) lParam);
			break;
		case MSG_END_DIALOG:  endDialog((bool) wParam); break;
		case MSG_AFTER_LOAD: afterLoad((QList<CatItem>*) wParam);
			break;
		case MSG_LOAD_ITEMS: load((QList<CatItem>*) wParam));
			break;
		case MSG_SAVE_ITEMS:
			save((QList<CatItem>*) wParam));
			break;
		case MSG_EXTEND_CATALOG:
			handled = extendCatalog((SearchInfo*) wParam, (QList<CatItem>*) lParam);
			break;
		case MSG_VALIDATE_ITEM: handled = validateItem((catItem*) wParam, (bool) lParam);
			break;

		default:
			handled = false;
			break;
	}
	return handled;
}

Q_EXPORT_PLUGIN2(pluginBase, PluginBasePlugin) 
