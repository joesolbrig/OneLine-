#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include "item.h"


//struct SourceSpec {
//
//    SourceSpec();
//    //Decoding constructor
//    SourceSpec(QString code)=0;
//    SourceSpec(QSettings* set)=0;
//    QString toString()=0;
//
//};

struct Directory {
    bool indexDirs;
    bool indexExe;
    QString name;

    QString label;
    int depth;
    int baseWeight;
    bool hidden;
    QStringList types;

    Directory():indexDirs(false), indexExe(false), depth(MAX_DIR_SEARCH_DEPTH), hidden(false) {
        types.append("*.*");
    }
    Directory(QString n, QStringList t, bool indexDirs, bool indexExes, int index_depth)
     : indexDirs(indexDirs), indexExe(indexExes), name(n), depth(index_depth), hidden(false), types(t) {}

    Directory(QString path, Directory parent){
        *this = parent;
        name = path;
        depth--;
    }

    QString toString(){
        QString s;
        QTextStream txs(&s);
        txs << indexDirs << TYPE_LIST_SEP
            << indexExe << TYPE_LIST_SEP
            << name << TYPE_LIST_SEP

            << label <<TYPE_LIST_SEP
            << depth <<TYPE_LIST_SEP
            << hidden <<TYPE_LIST_SEP
            << types.join(TYPE_ITEM_SEP) << TYPE_LIST_SEP
            ;
        return txs.readAll();
    }

    Directory(const Directory& d){
        indexDirs = d.indexDirs;
        indexExe = d.indexExe;
        name = d.name;

        label = d.label;
        depth = d.depth;
        baseWeight = d.baseWeight;
        hidden = d.hidden;
        types = d.types;
    }

    Directory(QSettings* settings){
        indexDirs = settings->value("indexDirs", false).toBool();
        indexExe = settings->value("indexExes", false).toBool();
        name = settings->value("name").toString();

        label = settings->value("labels").toString();
        types = settings->value("types").toStringList();
        depth = settings->value("depth", 3).toInt();
        baseWeight = settings->value("baseWeight", 0).toInt();
    }

    Directory(QString toDecode){
        QStringList items = toDecode.split(TYPE_LIST_SEP);
        //Q_ASSERT(types.count()>3);
        //Q_ASSERT(types.pop_front().toInt()==0 || types.pop_front().toInt()==1);
        indexDirs = (bool)items.first().toInt();
        items.pop_front();
        indexExe = (bool)items.first().toInt();
        items.pop_front();
        name = items.first();
        items.pop_front();
        label = items.first();
        items.pop_front();
        depth  = items.first().toInt();
        items.pop_front();
        types = items.first().split(TYPE_ITEM_SEP);
    }



};


#endif // DIRECTORY_H
