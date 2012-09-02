
#include "list_item.h"
#include "iconlist.h"





IconList::IconList()
{

}

void IconList::addIcon(ListItem& item){

        QIcon icon;

        //If we're asigned an icon already
        if()

        if(item.hasEmbeddedIcon()){
            icon = li.getActualIcon();
        }
        if (icon.isNull() && !item.getIcon().isEmpty()) {
            QFileInfo f(item.getIcon());
            if (f.exists()) {
                  icon = IconFromFile(item.getIcon());
            } else if(QIcon::hasThemeIcon(item.getIcon())){
                icon = makeStandardIcon(item.getIcon());
            }
        }

        if(icon.isNull() && item.getIsVerbArg()){
            icon = makeStandardIcon(UI_TEXT_PLACEHOLDER_ICON);
        }
        if (icon.isNull() && QFile::exists(item.getPath()) && QFileInfo(item.getPath()).isDir()) {
            icon = makeStandardIcon(FOLDER_ICON_NAME );
        }

        //First try to get gnome's idea of the icon...
        // (This can fail fail, frustratingly...)
        // From file
        if (icon.isNull() && QFile::exists(item.getPath())) { // && ((int)item.getItemType() >= (int)CatItem::LOCAL_DATA_DOCUMENT)
            qDebug() << "trying to get icon from file path : " << item.getPath();
            icon = gPlatform->icon(QDir::toNativeSeparators(item.getPath()));
        }

        // From mime-type
        if (icon.isNull() && !item.getMimeType().isEmpty()
                && ((int)item.getItemType() >= (int)CatItem::LOCAL_DATA_DOCUMENT)){
            qDebug() << "trying to get icon from Mime info: " << item.getMimeType();
            icon = gPlatform->iconFromMime(item.getMimeType());
        }

        //Otherwise settle on the icon of the associated application..
        if(icon.isNull()){
            CatItem actionItem = item.getActionParent();
            qDebug() << "trying to get icon from parent: " << actionItem.getPath();
            if(actionItem.isEmpty()){
                actionItem = gPlatform->alterItem(&item,true);
            }
            if(item.getItemType() != CatItem::ACTION_TYPE &&
               actionItem.getItemType() == CatItem::ACTION_TYPE){
               icon = getIcon(actionItem);
            }
        }

        if(icon.isNull()){
            if(item.getItemType() == CatItem::ACTION_TYPE ){
               gPlatform->alterItem(&item,true);
               ListItem li(item);
               icon = li.getActualIcon();
            }
        }



        if (icon.isNull() && !item.hasLabel(FILE_CATALOG_PLUGIN_STR) ) {
            qDebug() << "generic non-file doc : " << item.getPath();
            icon = makeStandardIcon(GENERIC_DOCUMENT_ICON_NAME);
        }

        if (icon.isNull() && item.hasLabel(FILE_CATALOG_PLUGIN_STR) && !QFile::exists(item.getPath())) {
            qDebug() << "use missing file icon for: " << item.getPath();
            icon = makeStandardIcon(FILE_MISSING_ICON_NAME);
        }

        if (icon.isNull() ) {
            icon = makeStandardIcon(GENERIC_DOCUMENT_ICON_NAME);
        }

        return icon;















    QIcon& ic = (li.getActualIcon());

    if(!ic.isNull()){

        if(li.iconIsByPath()){
            if(!m_byPath.contains(li.getPath())){



            }

        }

    }


}

void IconList::storeIcon(ListItem& li){
    QIcon& ic = (li.getActualIcon());

    if(!ic.isNull()){

        if(li.iconIsByPath()){
            if(!m_byPath.contains(li.getPath())){



            }

        }

    }


}
