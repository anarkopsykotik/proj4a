import QtQuick 2.0
import QtLocation 5.5
import QtPositioning 5.2



Rectangle {
     anchors.fill: parent

     Plugin{
        id: osmplugin
        name: "osm"
    }
	function addPoi(lat,longi,type) {
			//console.log("Got message:", lat);
			//console.log("Got message:", longi);
			//console.log("Got message:");
			//var lat = lat;
			var circle = Qt.createQmlObject('import QtLocation 5.3; MapCircle {   }', map, "dynamic");
			if(circle == null) {
			   console.log("error creating object" +  circle.errorString());
			   return false;
			}
			circle.center = QtPositioning.coordinate(lat, longi);
			circle.radius = 2500.0;

			if(type == 0){
				circle.color = 'blue';
			}
			else if(type == 1){
				circle.color = 'yellow';
			}
			else if(type == 2){
				circle.color = 'red';
			}
			else if(type == 3){
				circle.color = 'green';
			}
			circle.border.width = 1;
			map.addMapItem(circle);
			map.center = QtPositioning.coordinate(lat, longi);
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
