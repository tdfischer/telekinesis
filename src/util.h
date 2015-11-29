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

#include <gobject/gobject.h>
#include <memory>

struct DeleteLaterDeleter {
  template<typename T>
    inline void
    operator()(T *p)
    {
      if (p) p->deleteLater();
    }
};

struct GObjectDeleter {
  template<typename T>
    inline void
    operator()(T *p)
    {
      g_object_unref(p);
    }
  template<typename T>
    inline void
    ref(T *p)
    {
      g_object_ref_sink(p);
    }
};

template<typename T>
class DeleteLaterPtr: public std::unique_ptr<T, DeleteLaterDeleter> {
  public:
    DeleteLaterPtr()
      : std::unique_ptr<T, DeleteLaterDeleter>() {}
    DeleteLaterPtr(T *p)
      : std::unique_ptr<T, DeleteLaterDeleter>(p) {}
};

template<typename T, typename D>
class RefPtr : public std::unique_ptr<T, D> {
  public:
    constexpr
      RefPtr()
      : std::unique_ptr<T, D>()
      {
      }
    RefPtr(T *p)
      : std::unique_ptr<T, D>(p)
    {
      this->get_deleter().ref(p);
    }
    RefPtr(const RefPtr &other)
      : RefPtr(other.get())
    {
    }
    RefPtr(RefPtr &&other)
      : std::unique_ptr<T, D>(std::move(other))
    {
    }
};

template<typename G>
class GObjPtr: public RefPtr<G, GObjectDeleter> {
  public:
    using RefPtr<G, GObjectDeleter>::RefPtr;
};
