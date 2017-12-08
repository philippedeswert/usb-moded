/*
 
  Copyright (C) 2010 Nokia Corporation. All rights reserved.
  Copyright (C) 2013-2016 Jolla Ltd.

  author: Philippe De Swert <philippe.de-swert@nokia.com>
  author: Philippe De Swert <philippe.deswert@jollamobile.com>
  author: Simo Piiroinen <simo.piiroinen@jollamobile.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the Lesser GNU General Public License 
  version 2 as published by the Free Software Foundation. 

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
 
  You should have received a copy of the Lesser GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#define CONF_DIR_PATH		"/etc/usb-moded/run"
#define CONF_DIR_DIAG_PATH	"/etc/usb-moded/run-diag"

#define APP_INFO_ENTRY		"info"
#define APP_INFO_MODE_KEY	"mode"
#define APP_INFO_NAME_KEY	"name"
#define APP_INFO_LAUNCH_KEY	"launch"
#define APP_INFO_SYSTEMD_KEY	"systemd"
#define APP_INFO_POST		"post"

typedef enum {
  /** Application is not relevant for the current mode */
  APP_STATE_DONTCARE = 0,
  /** Application should be started */
  APP_STATE_INACTIVE = 1,
  /** Application should be stopped when exiting the mode  */
  APP_STATE_ACTIVE   = 2,
} app_state_t;

/** 
 * keep all the needed info together for launching an app 
 */
typedef struct list_elem
{
  /*@{*/
  char *name;		/* name of the app to launch */
  char *mode; 		/* mode in which to launch the app */
  char *launch;		/* dbus launch command/address */ 
  app_state_t state;	/* marker to check if the app has started sucessfully */
  int systemd;		/* marker to know if we start it with systemd or not */
  int post;		/* marker to indicate when to start the app */
  /*@}*/
}list_elem;

void readlist(int diag);
int activate_sync(const char *mode);
int activate_sync_post(const char *mode);
int mark_active(const gchar *name, int post);
int appsync_stop(gboolean force);
void free_appsync_list(void);
void usb_moded_appsync_cleanup(void);
