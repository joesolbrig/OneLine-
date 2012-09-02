
//Why do the authors demand this?
#define GMENU_I_KNOW_THIS_IS_UNSTABLE


#include "menu-layout.h"
#include "menu-monitor.h"
#include "menu-util.h"
#include "canonicalize.h"

#include "gmenu-tree.h"

#include "item.h"

//All this fricken bullshit from gmenu...

QList<CatItem> parseXdgMenus ();

typedef enum
{
  GMENU_TREE_ABSOLUTE = 0,
  GMENU_TREE_BASENAME = 1
} GMenuTreeType;

struct GMenuTree
{
  GMenuTreeType type;
  guint         refcount;

  char *basename;
  char *absolute_path;
  char *canonical_path;

  GMenuTreeFlags flags;
  GMenuTreeSortKey sort_key;

  GSList *menu_file_monitors;

  MenuLayoutNode *layout;
  GMenuTreeDirectory *root;

  GSList *monitors;

  gpointer       user_data;
  GDestroyNotify dnotify;

  guint canonical : 1;
};

typedef struct
{
  GMenuTreeChangedFunc callback;
  gpointer             user_data;
} GMenuTreeMonitor;

struct GMenuTreeItem
{
  GMenuTreeItemType type;

  GMenuTreeDirectory *parent;

  gpointer       user_data;
  GDestroyNotify dnotify;

  guint refcount;
};

struct GMenuTreeDirectory
{
  GMenuTreeItem item;

  DesktopEntry *directory_entry;
  char         *name;

  GSList *entries;
  GSList *subdirs;

  MenuLayoutValues  default_layout_values;
  GSList           *default_layout_info;
  GSList           *layout_info;
  GSList           *contents;

  guint only_unallocated : 1;
  guint is_root : 1;
  guint is_nodisplay : 1;
  guint layout_pending_separator : 1;
  guint preprocessed : 1;

  /* 16 bits should be more than enough; G_MAXUINT16 means no inline header */
  guint will_inline_header : 16;
};

typedef struct
{
  GMenuTreeDirectory directory;

  GMenuTree *tree;
} GMenuTreeDirectoryRoot;

struct GMenuTreeEntry
{
  GMenuTreeItem item;

  DesktopEntry *desktop_entry;
  char         *desktop_file_id;

  guint is_excluded : 1;
  guint is_nodisplay : 1;
};

struct GMenuTreeSeparator
{
  GMenuTreeItem item;
};

struct GMenuTreeHeader
{
  GMenuTreeItem item;

  GMenuTreeDirectory *directory;
};

struct GMenuTreeAlias
{
  GMenuTreeItem item;

  GMenuTreeDirectory *directory;
  GMenuTreeItem      *aliased_item;
};
