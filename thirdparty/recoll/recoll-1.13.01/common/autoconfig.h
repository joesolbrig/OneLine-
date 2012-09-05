/* common/autoconfig.h.  Generated from autoconfig.h.in by configure.  */
/* common/autoconfig.h.in.  Generated from configure.ac by autoheader.  */

/* Compile the aspell interface */
#define RCL_USE_ASPELL 1

/* Path to the aspell api include file */
#define ASPELL_INCLUDE "/usr/include/aspell.h"

/* Path to the aspell program */
#define ASPELL_PROG "/usr/bin/aspell"

/* Path to the file program */
#define FILE_PROG "/usr/bin/file"

/* Enable using the system's 'file' command to id mime if we fail internally */
#define USE_SYSTEM_FILE_COMMAND 1

/* Real time monitoring option */
#define RCL_MONITOR 1

/* Compile the fam interface */
/* #undef RCL_USE_FAM */

/* Path to the fam api include file */
/* #undef FAM_INCLUDE */

/* Compile the inotify interface */
#define RCL_USE_INOTIFY 1

/* iconv parameter 2 is const char** */
/* #undef RCL_ICONV_INBUF_CONST */

/* putenv parameter is const */
/* #undef PUTENV_ARG_CONST */

/* mkdtemp availability */
#define HAVE_MKDTEMP 1

#define HAVE_SYS_MOUNT_H 1
#define HAVE_SYS_STATFS_H 1
#define HAVE_SYS_STATVFS_H 1
#define HAVE_SYS_VFS_H 1

/* Use file extended attributes */
/* #undef RCL_USE_XATTR */

/* Split camelCase words */
/* #undef RCL_SPLIT_CAMELCASE */

/* No X11 */
/* #undef WITHOUT_X11 */
