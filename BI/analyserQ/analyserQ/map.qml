import QtQuick 2.0
import QtLocation 5.5



Rectangle {
     anchors.fill: parent

     Plugin{
        id: osmplugin
        name: "osm"
    }
	function addPoi(latitude, longitude, type) {
			console.log("Got message:");
			var circle = Qt.createQmlObject('import QtLocation 5.3; MapCircle {  center{latitude: 59.9485; longitude: 10.7686} }', map, "dynamic");
			if(circle == null) {
			   console.log("error creating object" +  circle.errorString());
			   return false;
			}
			circle.radius = 5000.0;
			circle.color = 'blue';
			circle.border.width = 3;
			map.addMapItem(circle);
			console.log("success creating object");
			return true;
		}
	
	 Map {
        anchors.fill: parent
        id: map
        plugin: osmplugin;
        zoomLevel: (maximumZoomLevel - minimumZoomLevel)/2//16
        center {
            // The Qt Company in Oslo
            latitude: 59.9485
            longitude: 10.7686
        }

		/*MapCircle {
			center {
				latitude: 59.9485
				longitude: 10.7686
			}
			radius: 500.0
			color: 'green'
			border.width: 3
		}*/

    }




}
