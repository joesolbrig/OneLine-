

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnome/gnome-desktop-item.h>
#include <gio/gio.h>

#include "gnomeSignalReceiver.h"

#include <QtGui/QApplication>
//#include "messageiconwidget.h"

GnomeSignalReceiver* g_gsr=0;


static void
active_window_changed_callback    (WnckScreen    *screen,
                                   WnckWindow    *,
                                   gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();

}

static void
active_workspace_changed_callback (WnckScreen    *screen,
                                   WnckWorkspace * ,
                                   gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();

}

static void
window_stacking_changed_callback  (WnckScreen    *screen,
                                   gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}




static void
workspace_created_callback        (WnckScreen    *screen,
                                   WnckWorkspace * ,
                                   gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}

static void
workspace_destroyed_callback      (WnckScreen    *screen,
                                   WnckWorkspace * ,
                                   gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}

static void
application_opened_callback (WnckScreen      *screen,
                             WnckApplication * )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}

static void
application_closed_callback (WnckScreen      *screen,
                             WnckApplication * )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}

static void
showing_desktop_changed_callback (WnckScreen *screen,
                                  gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}

static void
window_name_changed_callback (WnckWindow *,
                              gpointer )
{
    //g_gsr->setWindow(window);
    g_gsr->scanActiveWindows();

}


static void
window_workspace_changed_callback (WnckWindow    *,
                                   gpointer )
{
    g_gsr->scanActiveWindows();

}

static void
window_icon_changed_callback (WnckWindow    *,
                              gpointer )
{
    g_gsr->scanActiveWindows();

}

static void
window_geometry_changed_callback  (WnckWindow      *,
                                   gpointer )
{
    g_gsr->scanActiveWindows();
}


//Looking at the Avant Window source code, we can test all the windows states from here...
static void
window_state_changed_callback (WnckWindow     *,
                               WnckWindowState ,
                               WnckWindowState ,
                               gpointer )
{
    g_gsr->scanActiveWindows();

}


static void
window_opened_callback            (WnckScreen* ,
                                   WnckWindow* window,
                                   gpointer )
{
  g_print ("Window '%s' opened (pid = %d session_id = %s)\n",
           wnck_window_get_name (window),
           wnck_window_get_pid (window),
           wnck_window_get_session_id (window) ?
           wnck_window_get_session_id (window) : "none");

  g_signal_connect (G_OBJECT (window), "name_changed",
                    G_CALLBACK (window_name_changed_callback),
                    NULL);
  g_signal_connect (G_OBJECT (window), "state_changed",
                    G_CALLBACK (window_state_changed_callback),
                    NULL);
  g_signal_connect (G_OBJECT (window), "workspace_changed",
                    G_CALLBACK (window_workspace_changed_callback),
                    NULL);
  g_signal_connect (G_OBJECT (window), "icon_changed",
                    G_CALLBACK (window_icon_changed_callback),
                    NULL);
  g_signal_connect (G_OBJECT (window), "geometry_changed",
                    G_CALLBACK (window_geometry_changed_callback),
                    NULL);

}

static void
window_closed_callback            (WnckScreen    *screen,
                                   WnckWindow    *,
                                   gpointer )
{
    g_gsr->setScreen(screen);
    g_gsr->scanActiveWindows();
}


//Hooks everything to a global signal receiver who's 
int hookWindowChanges( )
{
    //Gtk takes a whole forrest of init's but they should be done before this...
    //g_thread_init(NULL);
    //gdk_threads_init();
    //gtk_init(&argc, &argv);
    //gnome_vfs_init();//This is suppose to be defunct but something demands it

    g_gsr = new GnomeSignalReceiver();
    
    WnckScreen *screen = wnck_screen_get (0);
	
    //Save the output here if we ever want to disconnect - we may not...
    g_signal_connect (G_OBJECT (screen), "active_window_changed",
                    G_CALLBACK (active_window_changed_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "active_workspace_changed",
                    G_CALLBACK (active_workspace_changed_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "window_stacking_changed",
                    G_CALLBACK (window_stacking_changed_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "window_opened",
                    G_CALLBACK (window_opened_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "window_closed",
                    G_CALLBACK (window_closed_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "workspace_created",
                    G_CALLBACK (workspace_created_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "workspace_destroyed",
                    G_CALLBACK (workspace_destroyed_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "application_opened",
                    G_CALLBACK (application_opened_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "application_closed",
                    G_CALLBACK (application_closed_callback), NULL);
    g_signal_connect (G_OBJECT (screen), "showing_desktop_changed",
                    G_CALLBACK (showing_desktop_changed_callback), NULL);
    return 1;

}














