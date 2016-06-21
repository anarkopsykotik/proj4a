import QtQuick 2.0
import QtPositioning 5.5
import QtLocation 5.5

Rectangle {
     anchors.fill: parent

     Plugin{
        id: osmplugin
        name: "osm"
    }

	 Map {
        anchors.fill: parent
        id: map
        plugin: osmplugin;
        zoomLevel: (maximumZoomLevel - minimumZoomLevel)/2
        center {
            // The Qt Company in Oslo
            latitude: 59.9485
            longitude: 10.7686
        }
    }


}
