#include "LibraryItem.h"
#include "LibraryModel.h"

LibraryItem::LibraryItem(GObjPtr<GUPnPDIDLLiteItem>& item)
  : QStandardItem()
{
    GList* resources = gupnp_didl_lite_object_get_resources (GUPNP_DIDL_LITE_OBJECT(item.get()));
    const QString uri(gupnp_didl_lite_resource_get_uri (GUPNP_DIDL_LITE_RESOURCE(resources->data)));
    g_list_free (resources);
    const gchar* name = gupnp_didl_lite_object_get_title(GUPNP_DIDL_LITE_OBJECT(item.get()));

    setData(name, Qt::DisplayRole);
    setData(uri, LibraryModel::UriRole);
}
