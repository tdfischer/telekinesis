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

#include <QtWidgets/QMainWindow>
#undef signals

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp/gupnp-context-manager.h>
#include <libgupnp/gupnp-context.h>

#include "ui_MainWindow.h"
#include "util.h"
#include "LibraryModel.h"

#include <memory>

class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void playCurrent();
    void onCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    void initUPNP();

    static void cb_new_renderer(GUPnPControlPoint *cp,
                                GUPnPDeviceProxy *proxy,
                                gpointer user_data);
    static void cb_context_available (GUPnPContextManager* context_manager,
                                      GUPnPContext* context,
                                      gpointer user_data);

    Ui::MainWindow ui;
    std::unique_ptr<LibraryModel> m_libraryModel;
    std::unique_ptr<QStandardItemModel> m_rendererModel;
    GObjPtr<GUPnPContextManager> m_contextManager;
    GObjPtr<GUPnPServiceProxy> m_lastProxy;
    std::vector<GObjPtr<GUPnPContext>> m_contexts;
    std::vector<GObjPtr<GUPnPControlPoint>> m_rendererCPs;
};
