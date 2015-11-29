#include "LibraryItem.h"
#include "LibraryModel.h"

#include <QtNetwork/QNetworkRequest>

LibraryItem::LibraryItem(GObjPtr<GUPnPDIDLLiteItem>& item)
  : QStandardItem(),
    m_network(new QNetworkAccessManager())
{
    GUPnPDIDLLiteObject *obj = GUPNP_DIDL_LITE_OBJECT(item.get());
    GList* resources = gupnp_didl_lite_object_get_resources (obj);
    const QString uri(gupnp_didl_lite_resource_get_uri (GUPNP_DIDL_LITE_RESOURCE(resources->data)));
    g_list_free (resources);
    const gchar* name = gupnp_didl_lite_object_get_title(obj);
    QUrl albumUri(gupnp_didl_lite_object_get_album_art(obj));

    if (albumUri.isValid()) {
        downloadAlbumArt(albumUri);
    }

    setData(name, Qt::DisplayRole);
    setData(uri, LibraryModel::UriRole);
}

void
LibraryItem::downloadAlbumArt(const QUrl& url)
{
    QNetworkRequest req(url);
    DeleteLaterPtr<QNetworkReply> reply(m_network->get(req));
    QObject::connect(reply.get(), &QNetworkReply::finished, [&]{
        setData(QPixmap(reply->readAll()), Qt::DecorationRole);
    });
}
