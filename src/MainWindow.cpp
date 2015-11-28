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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtCore/QDebug>

struct BrowseData {
  BrowseData(MainWindow *const _self, GObjPtr<GUPnPServiceProxy> _dir, QStandardItem *const _item)
    : self(_self),
      dir(_dir),
      item(_item) {}
  MainWindow *const self;
  GObjPtr<GUPnPServiceProxy> dir;
  QStandardItem *const item;
};

MainWindow::MainWindow()
  : QMainWindow(),
    m_libraryModel(new QStandardItemModel()),
    m_rendererModel(new QStandardItemModel())
{
  ui.setupUi(this);

  ui.libraryView->setModel(m_libraryModel.get());
  ui.rendererView->setModel(m_rendererModel.get());

  m_contextManager.reset(gupnp_context_manager_new (NULL, 0));
  g_signal_connect(m_contextManager.get(),
                   "context-available",
                   G_CALLBACK(cb_context_available),
                   this);
}

void
MainWindow::cb_context_available(GUPnPContextManager* context_manager,
                                 GUPnPContext* _context,
                                 gpointer user_data)
{
  MainWindow* self(static_cast<MainWindow*>(user_data));
  GObjPtr<GUPnPContext> context(_context);
  GObjPtr<GUPnPControlPoint> rendererCP(gupnp_control_point_new(context.get(), "urn:schemas-upnp-org:device:MediaRenderer:1"));
  GObjPtr<GUPnPControlPoint> libraryCP(gupnp_control_point_new(context.get(), "urn:schemas-upnp-org:device:MediaServer:1"));

  g_signal_connect(rendererCP.get(),
                   "device-proxy-available",
                   G_CALLBACK(MainWindow::cb_new_renderer),
                   self);

  g_signal_connect(libraryCP.get(),
                   "device-proxy-available",
                   G_CALLBACK(MainWindow::cb_new_library),
                   self);

  gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (rendererCP.get()), TRUE);
  gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (libraryCP.get()), TRUE);

  self->m_rendererCPs.push_back(std::move(rendererCP));
  self->m_libraryCPs.push_back(std::move(libraryCP));
  self->m_contexts.push_back(std::move(context));
}

void
MainWindow::cb_new_renderer(GUPnPControlPoint *cp,
                            GUPnPDeviceProxy *proxy,
                            void* user_data)
{
  MainWindow* self = static_cast<MainWindow*>(user_data);
  const char* name = gupnp_device_info_get_friendly_name (GUPNP_DEVICE_INFO (proxy));
  std::unique_ptr<QStandardItem> item(new QStandardItem(name));

  self->m_rendererModel->insertRow(0, item.release());
}

void
MainWindow::cb_new_library(GUPnPControlPoint *cp,
                            GUPnPDeviceProxy *proxy,
                            void* user_data)
{
  MainWindow* self = static_cast<MainWindow*>(user_data);
  const char* name = gupnp_device_info_get_friendly_name (GUPNP_DEVICE_INFO (proxy));
  std::unique_ptr<QStandardItem> item(new QStandardItem(name));


  GObjPtr<GUPnPServiceProxy> content_dir(GUPNP_SERVICE_PROXY (gupnp_device_info_get_service(GUPNP_DEVICE_INFO(proxy), "urn:schemas-upnp-org:service:ContentDirectory:1")));

  BrowseData* data = new BrowseData(self, content_dir, item.get());
  self->browse("0", 0, 256, data);

  self->m_libraryModel->insertRow(0, item.release());
}

void
MainWindow::browse(const char* container_id, guint32 start, guint32 count, BrowseData* browseData)
{
  gupnp_service_proxy_begin_action(
      browseData->dir.get(),
      "Browse",
      cb_browse,
      browseData,
      "ObjectID",
      G_TYPE_STRING,
      container_id,
      "BrowseFlag",
      G_TYPE_STRING,
      "BrowseDirectChildren",
      "Filter",
      G_TYPE_STRING,
      "@childCount",
      "StartingIndex",
      G_TYPE_UINT,
      start,
      "RequestedCount",
      G_TYPE_UINT,
      count,
      "SortCriteria",
      G_TYPE_STRING,
      "",
      NULL);
}

void
cb_item_available(GUPnPDIDLLiteParser *parser,
                  GUPnPDIDLLiteItem *item,
                  gpointer user_data)
{
  BrowseData* browseData = static_cast<BrowseData*>(user_data);
  std::unique_ptr<QStandardItem> listItem(new QStandardItem(gupnp_didl_lite_object_get_title(GUPNP_DIDL_LITE_OBJECT(item))));
  browseData->item->insertRow(0, listItem.release());
}

void
MainWindow::cb_container_available(GUPnPDIDLLiteParser *parser,
                       GUPnPDIDLLiteContainer *item,
                       gpointer user_data)
{
  BrowseData* browseData = static_cast<BrowseData*>(user_data);
  std::unique_ptr<QStandardItem> listItem(new QStandardItem(gupnp_didl_lite_object_get_title(GUPNP_DIDL_LITE_OBJECT(item))));

  BrowseData* childData = new BrowseData(browseData->self, browseData->dir, listItem.get());
  browseData->self->browse(gupnp_didl_lite_object_get_id(GUPNP_DIDL_LITE_OBJECT(item)), 0, 256, childData);

  browseData->item->insertRow(0, listItem.release());
}

void
MainWindow::cb_browse(GUPnPServiceProxy *content_dir,
            GUPnPServiceProxyAction *action,
            gpointer user_data)
{
  char *didl_xml;
  guint32 number_returned;
  guint32 total_matches;
  GError *error;
  BrowseData* browseData = static_cast<BrowseData*>(user_data);

  gupnp_service_proxy_end_action (content_dir,
                                  action,
                                  &error,
                                  "Result",
                                  G_TYPE_STRING,
                                  &didl_xml,
                                  "NumberReturned",
                                  G_TYPE_UINT,
                                  &number_returned,
                                  "TotalMatches",
                                  G_TYPE_UINT,
                                  &total_matches,
                                  NULL);

  GObjPtr<GUPnPDIDLLiteParser> parser(gupnp_didl_lite_parser_new ());
  g_signal_connect(parser.get(),
                   "container-available", 
                   G_CALLBACK(cb_container_available),
                   browseData);
  g_signal_connect(parser.get(),
                   "item-available", 
                   G_CALLBACK(cb_item_available),
                   browseData);
  gupnp_didl_lite_parser_parse_didl (parser.get(), didl_xml, &error);

  delete browseData;
}
