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

#ifndef ONELINE_GLOBALS_H
#define ONELINE_GLOBALS_H



//#ifndef COMMANDLINE
//#include <QWidget>
#include <QSettings>
//#endif

#define USE_MARGIN_WINDOW

#define ONELINE_VERSION 3

#define LABEL_FILE 0

#define GNOME_PLATFORM 1

#include <QDateTime>

class GnomeSignalReceiver;
extern GnomeSignalReceiver* g_gsr;

class PlatformBase;
class MarginWindow;
class MainUserWindow;
extern MainUserWindow* gMainWidget;
extern MarginWindow* gMarginWidget;
extern QSettings* gSettings;
extern QSettings* gStyleSettings;
extern PlatformBase* gPlatform;
extern QString gNativeSep;

#ifndef G_DIRS
#define G_DIRS
extern QHash<QString, QList<QString> >* gDirs;
#endif

extern QString gSearchTxt;

void log(QString str);




#endif
