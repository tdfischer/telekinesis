#include "LibraryModel.h"

struct BrowseData {
  BrowseData(LibraryModel *const _self,
             GObjPtr<GUPnPServiceProxy> _dir,
             QStandardItem *const _item)
    : self(_self),
      dir(_dir),
      item(_item) {}
  LibraryModel *const self;
  GObjPtr<GUPnPServiceProxy> dir;
  QStandardItem *const item;
};

LibraryModel::LibraryModel()
  : QStandardItemModel()
{
}

void
LibraryModel::addContext(GObjPtr<GUPnPContext>& cxt)
{
  GObjPtr<GUPnPControlPoint> libraryCP(
      gupnp_control_point_new(cxt.get(),
                              "urn:schemas-upnp-org:device:MediaServer:1")
  );

  g_signal_connect(libraryCP.get(),
                   "device-proxy-available",
                   G_CALLBACK(LibraryModel::cb_new_library),
                   this);

  gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (libraryCP.get()), TRUE);
  m_controlPoints.push_back(std::move(libraryCP));
}

void
LibraryModel::cb_new_library(GUPnPControlPoint *cp,
                             GUPnPDeviceProxy *proxy,
                             void* user_data)
{
  LibraryModel* self = static_cast<LibraryModel*>(user_data);
  const char* name = gupnp_device_info_get_friendly_name (GUPNP_DEVICE_INFO (proxy));
  std::unique_ptr<QStandardItem> item(new QStandardItem(name));


  GObjPtr<GUPnPServiceProxy> content_dir(GUPNP_SERVICE_PROXY (gupnp_device_info_get_service(GUPNP_DEVICE_INFO(proxy), "urn:schemas-upnp-org:service:ContentDirectory:1")));

  BrowseData* data = new BrowseData(self, content_dir, item.get());

  self->browse("0", 0, 256, data);
  self->insertRow(0, item.release());
}

void
LibraryModel::browse(const char* container_id,
                     guint32 start,
                     guint32 count,
                     BrowseData* browseData)
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
  GList* resources = gupnp_didl_lite_object_get_resources (GUPNP_DIDL_LITE_OBJECT(item));
  const QString uri(gupnp_didl_lite_resource_get_uri (GUPNP_DIDL_LITE_RESOURCE(resources->data)));
  g_list_free (resources);
  const gchar* name = gupnp_didl_lite_object_get_title(GUPNP_DIDL_LITE_OBJECT(item));
  std::unique_ptr<QStandardItem> listItem(new QStandardItem(name));
  listItem->setData(uri);
  browseData->item->insertRow(0, listItem.release());
}

void
LibraryModel::cb_container_available(GUPnPDIDLLiteParser *parser,
                       GUPnPDIDLLiteContainer *item,
                       gpointer user_data)
{
  BrowseData* browseData = static_cast<BrowseData*>(user_data);
  std::unique_ptr<QStandardItem> listItem(new QStandardItem(gupnp_didl_lite_object_get_title(GUPNP_DIDL_LITE_OBJECT(item))));

  BrowseData* childData = new BrowseData(browseData->self, browseData->dir, listItem.get());
  browseData->item->insertRow(0, listItem.release());
  browseData->self->browse(gupnp_didl_lite_object_get_id(GUPNP_DIDL_LITE_OBJECT(item)), 0, 256, childData);
}

void
LibraryModel::cb_browse(GUPnPServiceProxy *content_dir,
                        GUPnPServiceProxyAction *action,
                        gpointer user_data)
{
  char *didl_xml;
  guint32 number_returned;
  guint32 total_matches;
  GError *error = nullptr;
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
