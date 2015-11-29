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

MainWindow::MainWindow()
  : QMainWindow(),
    m_libraryModel(new LibraryModel()),
    m_rendererModel(new QStandardItemModel())
{
  ui.setupUi(this);

  ui.libraryView->setModel(m_libraryModel.get());
  ui.rendererView->setModel(m_rendererModel.get());

  connect(ui.playButton, &QAbstractButton::clicked, this, &MainWindow::playCurrent);

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
  self->m_libraryModel->addContext(context);

  GObjPtr<GUPnPControlPoint> rendererCP(gupnp_control_point_new(context.get(), "urn:schemas-upnp-org:device:MediaRenderer:1"));

  g_signal_connect(rendererCP.get(),
                   "device-proxy-available",
                   G_CALLBACK(MainWindow::cb_new_renderer),
                   self);

  gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (rendererCP.get()), TRUE);

  self->m_rendererCPs.push_back(std::move(rendererCP));
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
  self->m_lastProxy.reset(GUPNP_SERVICE_PROXY(gupnp_device_info_get_service(GUPNP_DEVICE_INFO(proxy), "urn:schemas-upnp-org:service:AVTransport:1")));
}

void
cb_transport_set(GUPnPServiceProxy *transport,
    GUPnPServiceProxyAction *action,
    gpointer user_data)
{
}

void
MainWindow::playCurrent()
{
  QModelIndex idx = ui.libraryView->selectionModel()->selectedIndexes()[0];
  QStandardItem* item = m_rendererModel->itemFromIndex(idx);
  const QString current_uri = item->data().toString();
  qDebug() << "Playing" << item;

  gupnp_service_proxy_begin_action(
      m_lastProxy.get(),
      "SetAVTransportURI",
      cb_transport_set,
      nullptr,
      "InstanceID",
      G_TYPE_UINT,
      0,
      "CurrentURI",
      G_TYPE_STRING,
      current_uri,
      "CurrentURIMetaData",
      G_TYPE_STRING,
      "",
      NULL);
}
