//Stuff for parsing the XDG menu item standard...

#define MENU_XDG_ELEM "Menu"
#define MOVE_XDG_ELEM "Move"
#define MERGE_XDG_ELEM "MergeFile"
#define NAME_XDG_ELEM "Name"
#define DefaultDirs_XDG_ELEM "DefaultDirectory"


struct XdgFixup{
    CatItem& parent;
    CatItem child;
    QList<QString> newPath;
};

class XdgMenuConstructor {

    QList<XdgFixup> fixupList;
    QList<QDir> dirPath;
    QHash<QString, CatItem> availableItems;


protected:
    //In this parsing process,
    //we use CatItems as containers for
    //The menu tree. We assume they won't yet have any
    //parents beside what we add.


    //I need a base function which allocates the base item,
    //and does fixup in place
    QList<CatItem> createXDG_MenuCategories(){
        CatItem i;//where everything will live here but then it will be

    }


    void requireOrError(bool condition, QString message){
        if(condition){
            qDebug() << message;
        }
    }

    QList<CatItem> categoryFromXDGMenuFile(QFile f, CatItem& i){
        QDomDocument doc("mydocument");
        doc.setContent(&f);
        return categoryFromXDG_DOM_Item(doc, i);
    }

    QList<CatItem> categoryFromXDG_DOM_Item(QDomNode d, CatItem& res){
        QDomNodeList docElems = d.elementsByTagName(QString("Menu"));
        for(int i=0;i<docElems.count();i++){
            res.append(processSubmenu(docElems[i]));
        }
    }

    //
    CatItem processSubmenu(QDomNode menuNode, CatItem& res){
        QDomNode d= menuNode.firstChild();
        while(!d.isNull()){
            if(d.isElement()){
                QDomElement e = d.toElement();
                if(e.tagName()==MOVE_XDG_ELEM){
                    QDomElement ee = e.firstChild();
                    XdgFixup p;

                    for(int j=0;!ee.isNull();j++){
                        if(ee.tagName() == OLD_XDG_ELEM){
                            CatItem i(ee.toCharacterData());
                            p.child = i;
                        } else if(ee.tagName() == NEW_XDG_ELEM){
                            p.newPath = ee.toCharacterData().toText();
                        }

                        ee = ee.nextSibling();
                    }
                    p.parent = res;
                } else if(e.tagName()==MENU_XDG_ELEM){
                    res.addChild(processSubmenu(d));
                } else if(e.tagName()==MERGE_XDG_ELEM){
                    QList<CatItem> c = categoryFromXDGMenuFile(d.toCharacterData());
                    for(int i=0;i<c.count();i++){
                        res.addChild(c[i]);
                    }
                } else if(e.tagName()==NAME_XDG_ELEM){
                    requireOrError(res.shortName.length()==0, "duplicate name");
                    res.shortName = d.toCharacterData();
                } else if(e.tagName()==DefaultDirs_XDG_ELEM){
                    processXDG_DirectoryFile(d.toCharacterData());
                } else if(e.tagName()=="Directory"){
                    //Just the info to the global list...
                    if(!d.toCharacterData().isNull()){
                        processXDG_DirectoryFile( d.toCharacterData().data());
                    }
                } else if(e.tagName()=="Include"){
                    processXDG_DirectoryFile(dirPath[0].absolutePath());
                } else if(e.tagName()=="DefaultLayout"){

                    //ignore optional element...

                } else if(e.tagName()=="Menuname"){
                    //ignore optional element...
                }

            }

       }

    }

    void processXDG_DirectoryFile(QString dirName){


    }

    QFile getXdgFile(QString fileName){
        for(int i=0;i<dirPath;i++){
            if(dirPath[i].exists(fileName)){
                QFile f(dirPath[i].filePath(fileName));
                return f;
            }

        }
        QFile f("");
        return f;
    }
    pair<QString> moveElem(QDomNode d){
        QDomNode n = d.firstChild();
        pair<QString> res;
        while(!n.isNull()){
            if(n.isElement()){
                if( n.toElement().tagName()==OLD_XDGELEM){
                    res.first = n.toCharacterData();
                } else if (n.toElement().tagName()==NEW_XDGELEM){
                    res.second = n.toCharacterData();
                }
            }
        }
        return res;
    }

    void doFixedUps(){
        for(int i=0;i < fixupList.count();i++){
            CatItem oldChild = fixupList[i].parent.removeChild(fixupList[i].child);
            fixupList[i].parent.recursivelyAddChildren(oldChild, fixupList[i].newPath);
        }

    }


    CatItem categoryFromXDGDesktopFile(QFile f);
    void fixupMenus();
public:
    XdgMenuConstructor();
    QList<CatItem> createCategories();

};



