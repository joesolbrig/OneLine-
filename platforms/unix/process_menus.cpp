/*
 * Copyright (C) 2010 Hans Joseoph Solbrig,
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
/* This file exists for the parseXdgMenus function, which return a list of CatItem's which correspond to
the menus, menu-items and submenu defined by the Xdg menu standard created by freedesktop.org.
This calls a **local snapshot** of gnome's libmenu package because the libmenu code base loudly
proclaims that it can't be used as stable API/ABI.. */

#define GMENU_I_KNOW_THIS_IS_UNSTABLE //As we see...
//(but how unstable is this IF it drives gnome panels??)

//OneLine c++ stuff
#include "item.h"

//Gmenu c stuff..
#include "gmenu-tree.h"
#include <glib/gi18n.h>
#include <string.h>

#include "process_menu.h"

//A couple functions to walk the subdirectory

CatItem gnomeMenuBaseItem(){
    CatItem gm(BASE_GNOME_MENU_PATH, GNOME_MENUS_NAME);
    gm.setItemType(CatItem::ORGANIZING_TYPE);
    gm.setSourceWeight(MAX_MAX_EXTERNAL_WEIGHT,
                       CatItem::createTypeParent(CatItem::LOCAL));
//    CatItem typeParent = CatItem::createTypeParent(CatItem::LOCAL);
//    gm.addParent(typeParent);
    Q_ASSERT(gm.isASource());
    return gm;
}


static CatItem process_entry(GMenuTreeEntry *entry, int impliedWeight)
{
  char* utf8_path = g_filename_to_utf8 (gmenu_tree_entry_get_desktop_file_path (entry),
				  -1, NULL, NULL, NULL);
  
  //"full_name" first then fallback to "name" - see http://osdir.com/ml/svn-commits-list/2009-09/msg01931.html
  const char *displayChars;
  displayChars = desktop_entry_get_full_name (entry->desktop_entry);
  if (!displayChars || displayChars[0] == '\0')
  displayChars = desktop_entry_get_name (entry->desktop_entry);

  CatItem childItem;
  const char* execChars = desktop_entry_get_exec(entry->desktop_entry);

  QString name((displayChars));
  QString item_path(utf8_path);
  if(execChars && execChars[0]!=0){
      QList<QString> execParts = QString(execChars).split(' ');
      if(execParts[0].length()>0){
          QString itemPath = addPrefix(SHELL_VERB_PREFIX,execParts[0]);
          childItem.setPath(itemPath);
          childItem.setCustomCommandLine(execChars);
      }
  }

  if(childItem.isEmpty())
    { return CatItem(); }

  childItem.setCustomPluginInfo(DESKTOP_ITEM_LABEL, item_path);
  childItem.setName(name);

  CatItem parent = gnomeMenuBaseItem();
  childItem.setExternalWeight(impliedWeight, parent);
  Q_ASSERT(childItem.hasParent(parent));
  Q_ASSERT(childItem.getTypeParent(CatItem::ORGANIZING_TYPE).isASource());
  childItem.setItemType(CatItem::VERB);

  childItem.setTagLevel(CatItem::ATOMIC_ELEMENT);
  if(desktop_entry_get_icon(entry->desktop_entry)
      && desktop_entry_get_icon(entry->desktop_entry)[0]){
      childItem.setIcon(desktop_entry_get_icon(entry->desktop_entry));
  }

  const char* descriptionChars = desktop_entry_get_comment(entry->desktop_entry);
  if(!descriptionChars || descriptionChars[0] == '\0'){
      descriptionChars = execChars;
  }
  QString description(descriptionChars);
  childItem.setDescription(description);

  Q_ASSERT(childItem.getUseDescription());

//  g_free (utf8_file_id);
  g_free (utf8_path);
  return childItem;
}

static CatItem walk_directory(GMenuTreeDirectory *directory, int previousWeight){
  const char* dirID = gmenu_tree_directory_get_menu_id(directory);
  const char* dirName = gmenu_tree_directory_get_name(directory);
  CatItem dirItem(addPrefix(QString(DESKTOPDIR_SOURCE_PREFIX), dirID), dirName);

  const char* dirComment = gmenu_tree_directory_get_comment(directory);
  if(!dirComment || dirComment[0]==0){
      dirItem.setDescription(STANDARD_GNOMEDIR_DESCRIPTION);
  } else {
      dirItem.setDescription(dirComment);
  }
  if(gmenu_tree_directory_get_icon(directory)){
      dirItem.setIcon(gmenu_tree_directory_get_icon(directory));
  }

  //CatItem gm(BASE_GNOME_MENU_PATH);
  //give directories a little more weight
  dirItem.setExternalWeight(previousWeight, gnomeMenuBaseItem());
  dirItem.setTagLevel(CatItem::POSSIBLE_TAG);
  dirItem.setSortingType(CatItem::VERB);
  dirItem.setIsAction(false);
  Q_ASSERT(!dirItem.getIsAction());
  //This is atomic meaning it can't be extended or have things added to it... currently
  dirItem.setUseDescription(true);

  GSList* items = gmenu_tree_directory_get_contents (directory);
  GSList* tmp = items;

  CatItem itemChild;
  CatItem dirChild;

  const int dirWeight = previousWeight/APP_FOLDER_WEIGHT_DECREASE_FACTOR;
  while (tmp != NULL) {
    GMenuTreeItem *item = (GMenuTreeItem *)tmp->data;
    switch (gmenu_tree_item_get_type (item)) {
        case GMENU_TREE_ITEM_ENTRY:
          itemChild = process_entry(GMENU_TREE_ENTRY (item), dirWeight);
          if(!itemChild .isEmpty()){ dirItem.addChild(itemChild); }
          break;
        case GMENU_TREE_ITEM_DIRECTORY:
          dirChild = walk_directory (GMENU_TREE_DIRECTORY (item), dirWeight);
          if(!dirChild.isEmpty()){ dirItem.addChild(dirChild); }
          break;
        case GMENU_TREE_ITEM_HEADER:
        case GMENU_TREE_ITEM_SEPARATOR:
          break;
        case GMENU_TREE_ITEM_ALIAS: {
            GMenuTreeItem *aliased_item = gmenu_tree_alias_get_item (GMENU_TREE_ALIAS (item));
            if (gmenu_tree_item_get_type (aliased_item) == GMENU_TREE_ITEM_ENTRY)
              itemChild  = process_entry(GMENU_TREE_ENTRY (aliased_item),dirWeight);
              if(!itemChild.isEmpty()){ dirItem.addChild(itemChild ); }
          }
          break;
        default: g_assert_not_reached ();
          break;
    }
    gmenu_tree_item_unref (tmp->data);
    tmp = tmp->next;
  }
  g_slist_free (items);
  return dirItem;
}

QList<CatItem> parseXdgMenus () {

  bindtextdomain ("gnome-menus", "/usr/local/share/locale");
  bind_textdomain_codeset ("gnome-menus", "UTF-8");
  textdomain ("gnome-menus");

  GMenuTree* tree = gmenu_tree_lookup ("applications.menu", GMENU_TREE_FLAGS_NONE);
  g_assert (tree != NULL);

  GMenuTreeDirectory* root = gmenu_tree_get_root_directory (tree);
  Q_CHECK_PTR(root);
  CatItem dirParent = walk_directory(root, MAX_EXTERNAL_WEIGHT);
  gmenu_tree_item_unref (root);
  gmenu_tree_unref (tree);

  QList<CatItem> childs = dirParent.getChildren();

  for(int i=0; i< childs.count();i++){
      childs[i].setPinned("");
  }
  return childs;
}
