/*
  Copyright (C) 2011 Nokia Corporation. All rights reserved.

  Author: Philippe De Swert <philippe.de-swert@nokia.com>

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

  usb-moded_network : (De)activates network depending on the network setting system.
*/

#ifndef USB_MODED_NETWORK_H_
#define USB_MODED_NETWORK_H_

#include "usb_moded-dyn-config.h"

int usb_network_up(struct mode_list_elem *data);
int usb_network_down(struct mode_list_elem *data);
int usb_network_update(void);
int usb_network_set_up_dhcpd(struct mode_list_elem *data);

#ifdef CONNMAN
gboolean connman_set_tethering(const char *path, gboolean on);
#endif

#endif /* USB_MODED_NETWORK_H_ */
