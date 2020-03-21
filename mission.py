"""

"""
import json

class Mission():
    def __init__(self):
        """
        TAKE_OFF
        TOUCH_DOWN
        HOLD_ALT
        RETURN_TO_LAUNCH
        HOLD_POSITION
        FOLLOW_GROUND_OBJECT
        FOLLOW_FLYING_OBJECT
        MOVE_XYZ_BY_DISTANCE
        ROTATE_BY_ANGLE
        GOTO_POSITION
        """
        pass

    def getMission(self):
        with open("mission.json") as f:
            mission = json.load(f)
        return mission
