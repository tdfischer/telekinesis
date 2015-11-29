#include "LibraryItem.h"
#include "LibraryModel.h"

#include <QtNetwork/QNetworkRequest>

LibraryItem::LibraryItem(GObjPtr<GUPnPDIDLLiteItem>& item)
  : QStandardItem(),
    m_network(new QNetworkAccessManager()),
    m_metadataModel(new QStandardItemModel())

{
    GUPnPDIDLLiteObject *obj = GUPNP_DIDL_LITE_OBJECT(item.get());
    GList* resources = gupnp_didl_lite_object_get_resources (obj);
    const QString uri(gupnp_didl_lite_resource_get_uri (GUPNP_DIDL_LITE_RESOURCE(resources->data)));
    g_list_free (resources);
    const QString title(gupnp_didl_lite_object_get_title(obj));
    const QString description(gupnp_didl_lite_object_get_description(obj));
    const QString artist(gupnp_didl_lite_object_get_artist(obj));
    const QString album(gupnp_didl_lite_object_get_album(obj));
    const QString upnpClass(gupnp_didl_lite_object_get_upnp_class(obj));
    const int trackNum = gupnp_didl_lite_object_get_track_number(obj);
    const QUrl albumUri(gupnp_didl_lite_object_get_album_art(obj));

    if (albumUri.isValid()) {
        downloadAlbumArt(albumUri);
    }

    setMetadata(title, "Title", Qt::DisplayRole);
    setMetadata(uri, "URI", LibraryModel::UriRole);
    setMetadata(description, "Description", LibraryModel::DescriptionRole);
    setMetadata(artist, "Artist", LibraryModel::ArtistRole);
    setMetadata(album, "Album", LibraryModel::AlbumRole);
    setMetadata(trackNum, "Track Number", LibraryModel::TrackNumberRole);
    setMetadata(upnpClass, "UPnP Class", LibraryModel::ClassRole);

    setData(QVariant::fromValue<void*>(static_cast<void*>(this)), LibraryModel::ItemRole); 
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

void
LibraryItem::setMetadata(const QVariant& value,
                         const QString& label,
                         const int role)
{
    setData(value, role);
    QList<QStandardItem*> row;
    row << new QStandardItem(label);
    row << new QStandardItem(value.toString());
    m_metadataModel->insertRow(0, row);
}

QAbstractItemModel* LibraryItem::metadataModel() const
{
    return m_metadataModel.get();
}
