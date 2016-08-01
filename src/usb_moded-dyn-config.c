/**
  @file usb_moded-dyn-mode.c

  Copyright (C) 2011 Nokia Corporation. All rights reserved.
  Copyright (C) 2013 Jolla. All rights reserved.

  @author: Philippe De Swert <philippe.de-swert@nokia.com>
  @author: Philippe De Swert <philippe.deswert@jollamobile.com>

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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "usb_moded-dyn-config.h"
#include "usb_moded-log.h"

static struct mode_list_elem *read_mode_file(const gchar *filename);

void android_mode_data_free(android_mode_data *data)
{
  free(data->sysfs_path);
  free(data->sysfs_value);
  free(data->sysfs_reset_value);
  free(data->softconnect);
  free(data->softconnect_disconnect);
  free(data->softconnect_path);
  free(data->android_extra_sysfs_path);
  free(data->android_extra_sysfs_value);
  free(data->android_extra_sysfs_path2);
  free(data->android_extra_sysfs_value2);
  free(data->android_extra_sysfs_path3);
  free(data->android_extra_sysfs_value3);
  free(data->android_extra_sysfs_path4);
  free(data->android_extra_sysfs_value4);
  free(data);
}

void list_item_free(mode_list_elem *list_item)
{
  free(list_item->mode_name);
  free(list_item->mode_module);
  free(list_item->network_interface);
  free(list_item->idProduct);
  free(list_item->idVendorOverride);
#ifdef CONNMAN
  free(list_item->connman_tethering);
#endif
  if(list_item->android)
	android_mode_data_free(list_item->android);
  free(list_item);
}

void free_mode_list(GList *modelist)
{
  if(modelist)
  {
	g_list_foreach(modelist, (GFunc) list_item_free, NULL);
	g_list_free(modelist);
	modelist = 0;
  }
}

static gint compare_modes(gconstpointer a, gconstpointer b)
{
  struct mode_list_elem *aa = (struct mode_list_elem *)a;
  struct mode_list_elem *bb = (struct mode_list_elem *)b;

  return g_strcmp0(aa->mode_name, bb->mode_name);
}

GList *read_mode_list(int diag)
{
  GDir *confdir;
  GList *modelist = NULL;
  const gchar *dirname;
  struct mode_list_elem *list_item;
  gchar *full_filename = NULL;

  if(diag)
	confdir = g_dir_open(DIAG_DIR_PATH, 0, NULL);
  else
	confdir = g_dir_open(MODE_DIR_PATH, 0, NULL);
  if(confdir)
  {
    while((dirname = g_dir_read_name(confdir)) != NULL)
	{
		log_debug("Read file %s\n", dirname);
		if(diag)
			full_filename = g_strconcat(DIAG_DIR_PATH, "/", dirname, NULL);
		else
			full_filename = g_strconcat(MODE_DIR_PATH, "/", dirname, NULL);
		list_item = read_mode_file(full_filename);
		/* free full_filename immediately as we do not use it anymore */
		free(full_filename);
		if(list_item)
			modelist = g_list_append(modelist, list_item);
	}
    g_dir_close(confdir);
  }
  else
	  log_debug("Mode confdir open failed or file is incomplete/invalid.\n");

  modelist = g_list_sort (modelist, compare_modes);
  return(modelist);
}

static struct mode_list_elem *read_mode_file(const gchar *filename)
{
  GKeyFile *settingsfile;
  gboolean test = FALSE;
  struct mode_list_elem *list_item = NULL;

  settingsfile = g_key_file_new();
  test = g_key_file_load_from_file(settingsfile, filename, G_KEY_FILE_NONE, NULL);
  if(!test)
  {
      return(NULL);
  }
  list_item = malloc(sizeof(struct mode_list_elem));
  list_item->mode_name = g_key_file_get_string(settingsfile, MODE_ENTRY, MODE_NAME_KEY, NULL);
  log_debug("Dynamic mode name = %s\n", list_item->mode_name);
  list_item->mode_module = g_key_file_get_string(settingsfile, MODE_ENTRY, MODE_MODULE_KEY, NULL);
  list_item->appsync = g_key_file_get_integer(settingsfile, MODE_ENTRY, MODE_NEEDS_APPSYNC_KEY, NULL);
  list_item->mass_storage = g_key_file_get_integer(settingsfile, MODE_ENTRY, MODE_MASS_STORAGE, NULL);
  list_item->network = g_key_file_get_integer(settingsfile, MODE_ENTRY, MODE_NETWORK_KEY, NULL);
  list_item->network_interface = g_key_file_get_string(settingsfile, MODE_ENTRY, MODE_NETWORK_INTERFACE_KEY, NULL);
  if(g_key_file_has_group(settingsfile, MODE_ANDROID_ENTRY))
  {
	list_item->android = malloc(sizeof(struct android_mode_data));
	list_item->android->sysfs_path = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_SYSFS_PATH, NULL);
	list_item->android->sysfs_value = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_SYSFS_VALUE, NULL);
	list_item->android->sysfs_reset_value = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_SYSFS_RESET_VALUE, NULL);
	list_item->android->softconnect = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_SOFTCONNECT, NULL);
	list_item->android->softconnect_disconnect = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_SOFTCONNECT_DISCONNECT, NULL);
	list_item->android->softconnect_path = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_SOFTCONNECT_PATH, NULL);
	list_item->android->android_extra_sysfs_path = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_PATH, NULL);
	list_item->android->android_extra_sysfs_path2 = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_PATH2, NULL);
	list_item->android->android_extra_sysfs_path3 = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_PATH3, NULL);
	list_item->android->android_extra_sysfs_path4 = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_PATH4, NULL);
	list_item->android->android_extra_sysfs_value = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_VALUE, NULL);
	list_item->android->android_extra_sysfs_value2 = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_VALUE2, NULL);
	list_item->android->android_extra_sysfs_value3 = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_VALUE3, NULL);
	list_item->android->android_extra_sysfs_value4 = g_key_file_get_string(settingsfile, MODE_ANDROID_ENTRY, MODE_ANDROID_EXTRA_SYSFS_VALUE4, NULL);
  }
  list_item->idProduct = g_key_file_get_string(settingsfile, MODE_OPTIONS_ENTRY, MODE_IDPRODUCT, NULL);
  list_item->idVendorOverride = g_key_file_get_string(settingsfile, MODE_OPTIONS_ENTRY, MODE_IDVENDOROVERRIDE, NULL);
  list_item->nat = g_key_file_get_integer(settingsfile, MODE_OPTIONS_ENTRY, MODE_HAS_NAT, NULL);
  list_item->dhcp_server = g_key_file_get_integer(settingsfile, MODE_OPTIONS_ENTRY, MODE_HAS_DHCP_SERVER, NULL);
#ifdef CONNMAN
  list_item->connman_tethering = g_key_file_get_string(settingsfile, MODE_OPTIONS_ENTRY, MODE_CONNMAN_TETHERING, NULL);
#endif

  g_key_file_free(settingsfile);
  if(list_item->mode_name == NULL || list_item->mode_module == NULL)
  {
	/* free list_item as it will not be used */
	list_item_free(list_item);
	return NULL;
  }
  if(list_item->network && list_item->network_interface == NULL)
  {
	/* free list_item as it will not be used */
	list_item_free(list_item);
	return NULL;
  }
#if 0 //TODO: check if really needed
  if(list_item->sysfs_path && list_item->sysfs_value == NULL)
  {
	/* free list_item as it will not be used */
	list_item_free(list_item);
	return NULL;
  }
  if(list_item->softconnect &&  list_item->softconnect_path == NULL)
  {
	/* free list_item as it will not be used */
	list_item_free(list_item);
	return NULL;
  }
  else
#endif
	return(list_item);
}
