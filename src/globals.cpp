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

#include "globals.h"
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QTextStream>

class MainUserWindow;
MainUserWindow* gMainWidget=0;
MarginWindow* gMarginWidget=0;
class PlatformBase;
PlatformBase* gPlatform;

QSettings* gSettings;
QSettings* gStyleSettings;
QString gSearchTxt;
QString gNativeSep = QDir::toNativeSeparators("/");

QHash<QString, QList<QString> >* gDirs=0;


void log(QString str) 
{
	QFile file("log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	QTextStream os(&file);
	os <<"[";
	os << QDateTime::currentDateTime().toString("hh:mm:ss");
	os << "] " << str << "\n";
}


