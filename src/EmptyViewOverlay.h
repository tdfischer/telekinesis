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

#include <QtWidgets/QWidget>
#include <QtWidgets/QAbstractItemView>

class EmptyViewOverlay: public QWidget {
    Q_OBJECT
public:
    EmptyViewOverlay(QAbstractItemView *view,
                     const QString& caption,
                     const QPixmap& icon);
protected:
    bool eventFilter(QObject* obj, QEvent* evt);
    void paintEvent(QPaintEvent* evt);
private slots:
    void syncToModel();
private:
    QAbstractItemView* m_view;
};
