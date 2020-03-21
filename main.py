"""
main-py
"""
from drone import Drone as DR
from mission import Mission

copter = DR()
mission = Mission()

if (mission.getMission()['default'] != "None"):
    copter.setMode(DR.MISSION_FLIGHT_MODE)

print(copter.MODE)
print(mission.getMission()['default'])

copter.calibrate()

if coper.armePrecheck() == "success":
    copter.fly()
