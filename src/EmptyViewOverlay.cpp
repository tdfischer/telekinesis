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

#include "EmptyViewOverlay.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QPainter>
#include <QtCore/QEvent>

EmptyViewOverlay::EmptyViewOverlay(QAbstractItemView* view,
    const QString& caption,
    const QPixmap& icon)
  : QWidget(view),
    m_view(view)
{
    QVBoxLayout* layout(new QVBoxLayout(this));
    QLabel* iconWidget = new QLabel(this);
    QLabel* label = new QLabel(caption, this);

    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setWordWrap(true);
    iconWidget->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    iconWidget->setPixmap(icon);

    setLayout(layout);
    layout->addWidget(iconWidget);
    layout->addWidget(label);
    hide();

    setAttribute(Qt::WA_NoSystemBackground);

    connect(view->model(), &QAbstractItemModel::columnsInserted,
            this, &EmptyViewOverlay::syncToModel);
    connect(view->model(), &QAbstractItemModel::rowsInserted,
            this, &EmptyViewOverlay::syncToModel);
    syncToModel();

    m_view->installEventFilter(this);
}

void
EmptyViewOverlay::paintEvent(QPaintEvent* evt)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 128));
}

bool
EmptyViewOverlay::eventFilter(QObject* obj, QEvent* evt)
{
    if (obj != m_view) {
        return false;
    }

    if (evt->type() == QEvent::Resize) {
        QWidget* w = static_cast<QWidget*>(obj);
        QRect geom(0, 0, w->width(), w->height());
        setGeometry(geom);
    }
    return false;
}

void
EmptyViewOverlay::syncToModel()
{
    if (m_view->model()->rowCount() == 0 || m_view->model()->columnCount() == 0) {
        raise();
        show();
    } else {
        hide();
    }
}
