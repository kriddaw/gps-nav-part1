import gpxpy
import json

# By part 4 of the series, 
# we'll have some more refine code 
# with a better user interface
# for now, this will get the job done

gpx_file = 'export.gpx'

with open(gpx_file, 'r') as f:
    data = gpxpy.parse(f)

waypoint_json = {}
coordinate_list = []

waypoint_string = '{'
for i, waypoint in enumerate(data.waypoints):
    coordinate_list.append(
        [round(waypoint.latitude,6), 
         round(waypoint.longitude,6)]
    )
    waypoint_string += f'{{{waypoint.latitude:6f}, {waypoint.longitude:6f}}}'
    if i < len(data.waypoints)-1:
        waypoint_string += ', '
waypoint_string += '};'

print(f'Array length: {len(data.waypoints)}')
print(waypoint_string)

waypoint_json['arrayLength'] = len(data.waypoints)
waypoint_json['coordinates'] = coordinate_list
# print(waypoint_json)

with open('WPS.TXT', 'w') as f:
    json.dump(waypoint_json, f)
