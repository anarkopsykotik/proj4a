#include <QDebug>
#include <QGeoServiceProvider>
#include <QGeoMappingManager>
#include <QGeoCoordinate>
#include <QGeoMapCircleObject>
#include <QGeoMapPixmapObject>
#include <QGeoMapGroupObject>
#include <QGraphicsSceneMouseEvent>
#include <QGeoBoundingBox>
#include <QtCore>

#include "map_widget.h"

// A widget for QML, therefore we need the parameter-less constructor.
MapWidget::MapWidget() :
QGraphicsGeoMap(createManager())
{
	setCenter(QGeoCoordinate(51.05, 13.73));
	setZoomLevel(17);
}

MapWidget::~MapWidget()
{
}

// This method is a bit a heck. We actually call it before the object
// is completely created. We need to do this, because our parent class
// needs a QGeoMappingManager passed to its constructor.
QGeoMappingManager* MapWidget::createManager()
{
	qDebug() << "INFO: Creating mapping manager";
	// We have to access this as static member, because we can't get anything
	// inside this object from the outside. The reason is, that this method
	// is called by the constructor and that we cannot add values to the
	// constructor, because QML always needs a parameter-less constructor.
	QGeoServiceProvider *serviceProvider = new QGeoServiceProvider("osm", params);
	QGeoMappingManager *mappingManager = serviceProvider->mappingManager();
	if (mappingManager == 0) {
		qDebug() << "WARN: Could not load 'osm' plugin. Falling back to 'nokia' plugin.";
		serviceProvider = new QGeoServiceProvider("nokia");
		mappingManager = serviceProvider->mappingManager();
	}

	return mappingManager;
}

void MapWidget::addPoi(PoiData *poi, bool active)
{
	QGeoCoordinate coord(poi->getLat(), poi->getLon());

	QPixmap pixmap;
	if (active) {
		pixmap = QPixmap(":qml/Common/img/poi_active.png");
	}
	else {
		pixmap = QPixmap(":qml/Common/img/poi_inactive.png");
	}

	QGeoMapPixmapObject *pixMapObject = new QGeoMapPixmapObject(coord, QPoint(-26, -65), pixmap);
	pixMapObject->setProperty("uuid", poi->getUuid());
	pixMapObject->setObjectName("poiMarker");

	QGeoMapGroupObject *poiMarker = new QGeoMapGroupObject();
	poiMarker->setProperty("uuid", poi->getUuid());
	poiMarker->addChildObject(pixMapObject);

	if (active) {
		QGeoMapCircleObject *circle = new QGeoMapCircleObject(coord, poi->getRadius());
		circle->setPen(QPen((poiColor)));
		circle->setBrush(QBrush(poiColor));
		circle->setZValue(-1);
		poiMarker->addChildObject(circle);
	}

	addMapObject(poiMarker);
}

void MapWidget::removePoi(PoiData *poi)
{
	for (int i = 0; i < poiMarkers.length(); ++i) {
		QGeoMapObject *marker = poiMarkers[i];
		if (marker->property("uuid").toString() == poi->getUuid()) {
			removeMapObject(marker);
			return;
		}
	}
}

void MapWidget::clearPois()
{
	for (int i = 0; i < poiMarkers.length(); ++i) {
		QGeoMapObject *obj = poiMarkers[i];
		removeMapObject(obj);
	}
}

/*

Remember the position. We use this in mouseReleaseEvent
/
void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent event)
{
lastPos = event->pos();
}
/*

If the mouse was moved by a maximum of 30px between press and release

we look at the press coordinates and if there is a POI we emit a signal

if there are several POIs, we only emit the signal for the first one.
/
void MapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent event)
{
QPointF newPos = event->pos();
QPointF diff = lastPos - newPos;

if (qAbs(diff.x()) > 30 || qAbs(diff.y()) > 30) {
return;
}

QList<QGeoMapObject*> objects = mapObjectsAtScreenPosition(lastPos);
if (objects.length() > 0) {
for (int i = 0; i < objects.length(); i++) {
QGeoMapObject *obj = objects[i];
if (obj->objectName() == "poiMarker") {
QString uuid = obj->property("uuid").toString();
emit poiClicked(uuid);
}
}
}
}

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
// Round to int
QPoint lastPos = event->lastPos().toPoint();
QPoint pos = event->pos().toPoint();

int dx = lastPos.x() - pos.x();
int dy = lastPos.y() - pos.y();

pan(dx, dy);
setFollowPosition(false);
}