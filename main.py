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
print("FC_LOOP_RATE", copter.CONFIG['FC_LOOP_RATE'])
print(mission.getMission()['default'])

#copter.calibrate()

#if coper.armePrecheck() == "success":
#    copter.fly()

copter.fly()

"""
"""
