/**
 * Telekinesis - A UPnP ControlPoint
 * Copyright (C) 2015 Torrie Fischer <tdfischer@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <QtGui/QStandardItemModel>
#undef signals

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp/gupnp-context-manager.h>
#include <libgupnp/gupnp-context.h>

#include <libgupnp-av/gupnp-av.h>

#include "util.h"

struct BrowseData;

class LibraryModel: public QStandardItemModel {
    Q_OBJECT
public:
    LibraryModel();
    void addContext(GObjPtr<GUPnPContext>& cxt);
private:
    std::vector<GObjPtr<GUPnPControlPoint>> m_controlPoints;

    void browse(const char* container_id,
                guint32 start,
                guint32 count,
                BrowseData* browseData);

    static void cb_new_library(GUPnPControlPoint *cp, 
                        GUPnPDeviceProxy *proxy,
                        void* user_data);
    static void cb_browse(GUPnPServiceProxy *content_dir,
                   GUPnPServiceProxyAction *action,
                   gpointer user_data);
    static void cb_container_available(GUPnPDIDLLiteParser *parser,
                                GUPnPDIDLLiteContainer *item,
                                gpointer user_data);
};
