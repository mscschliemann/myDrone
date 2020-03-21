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
# import Camera
# import Gyro
# import Receiver

class Drone():

    RC_FLIGHT_MODE = "RC_FLIGHT_MODE"
    MISSION_FLIGHT_MODE = "MISSION_FLIGHT_MODE"

    def __init__(self):
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


        print("Initialize Drone...")

    def setMode(self, mode):
        self.MODE = mode

    def fly(self):
        """
        Hauptloop
        """
        while 1:
            print("Drone flies...")
            break
            """ Lese die aktuelle Position. """
            """ Berechne die nächste Zielposition """
            """ Berechne den Flugvektor """
            """ Addiere Stabilisierung """
            """ Steuere die ESCs an """
