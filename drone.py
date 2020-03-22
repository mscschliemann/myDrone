"""
Class Drone

Diese Klasse bildet die Drone ab.
- Status der Drone
- Flugmodus: MISSION, RECEIVER
- Hauptloop
"""
# impoer ESC
# import ACC
# import BAR
# import GPS
# import Camera
# import Gyro (opt.)
# import Compass (opt.)
# import Receiver
from threading import Timer
from datetime import datetime
import json
from pid import PID

class Drone():

    RC_FLIGHT_MODE = "RC_FLIGHT_MODE"
    MISSION_FLIGHT_MODE = "MISSION_FLIGHT_MODE"

    def __init__(self):

        with open("config.json") as f:
            self.CONFIG = json.load(f)
        """ Setze die Start-Parameter """
        self.MODE = self.RC_FLIGHT_MODE
        self.STABALIZED = True
        """ Manche ESC's müssen kalibriert werden. S. Datasheet """
        """ Das dient dazu dem ESC die MIN und MAX pwm Werte mitzuteilen """
        self.ESC_CALIBRATED = False
        """ Acceleromwter kalibrieren """
        self.ACC_CALIBRATED = False
        """ Barometer kalibrieren / 0-Höhe setzen """
        self.BAR_CALIBRATED = False
        """ Das Arming ist dazu da, damit der Copter nicht unkontrolliert
        losfliegt. Der Copter darf erst gearmed werden, wenn die Umgebung
        gesichert ist und der Flugbetrieb beginnen kann """
        self.ARMED = False

        self.timer = Timer(self.CONFIG['FC_LOOP_RATE'], self.IMULoop)
        self.tick = False
        self.timer.start()

        print("Initialize Drone...")

    def setMode(self, mode):
        self.MODE = mode

    def contTimer(self):
        self.timer = Timer(self.CONFIG['FC_LOOP_RATE'], self.IMULoop)
        self.timer.start()


    def IMULoop(self):
        #global tick
        self.tick = True
        self.contTimer()

    def printSomething(self):
        print("Drone flies...")
        dateTimeObj = datetime.now()
        print(dateTimeObj.hour, ':', dateTimeObj.minute, ':', dateTimeObj.second, '.', dateTimeObj.microsecond)
        print(id(self.timer))

    def fly(self):
        """
        Hauptloop
        """
        try:
            while 1:
                if self.tick:
                    self.printSomething()

                    """ Lese die aktuelle Position. """
                    #current_position = (0,0,0)
                    current_position = 0
                    """ Berechne die nächste Zielposition """
                    #target_position = (1,1,1)
                    target_position = 1
                    """ Berechne den Flugvektor """
                    vector = target_position - current_position
                    """ Addiere Stabilisierung PID """
                    error = PID.getCorrection()
                    vector += error
                    """ Steuere die ESCs an """
                    print("---->  ", vector)
                    self.tick = False
        except KeyboardInterrupt:
            print("W: interrupt received, stopping…")
