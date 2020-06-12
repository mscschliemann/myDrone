
#ifndef _AK8963_H_
#define _AK8963_H_

#include "I2Cdev.h"
#include <avr/pgmspace.h>

#define AK8963_ADDRESS   0x0C
#define AK8963_WHO_AM_I  0x00 // should return 0x48
#define AK8963_INFO      0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

class AK8963 {
    public:
        AK8963();
        AK8963(uint8_t address);

        void reset();
        void initialize();
        float* getMagCalibration();
        void getMotion3();


        float magCalibration[3] = {0, 0, 0};
        float magbias[3] = {8.0, 15.0, -15.0};
        float Mxyz[3];

    private:
        enum Mscale {
            MFS_14BITS = 0, // 0.6 mG per LSB
            MFS_16BITS      // 0.15 mG per LSB
        };
        uint8_t Mscale = MFS_16BITS;
        uint8_t devAddr;
        uint8_t buffer[14];
        uint16_t word_buffer[14];
        uint8_t Mmode;
};

#endif /* _AK8963_H_ */