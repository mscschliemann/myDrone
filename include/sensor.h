/**
*
* @file	sensor.h
* @date	2020-05-30
* @version	v1.0.0
*
*/


/******************************  IR Sensor *****************************/
/**
 * @brief This section defines the transmitter button values
 * 
 */

/** iDual Fernbedienung (NEC) */
const long TRIM_VOR = 0x407f708f;
const long TRIM_RECHTS = 0x407fe01f;
const long TRIM_RUECK = 0x407f10ef;
const long TRIM_LINKS = 0x407fa857;
const long SPEED_UP = 0x407FF00F;
const long SPEED_DOWN = 0x407f58a7;
const long HALT = 0x407F40BF;
const long MAX = 0x407F807F;

/** NAD Fernbedienung (NEC) */
const long NAD_ON = 0xE13E0DF2; // repeatable
const long NAD_OFF = 0xE13E8D72; // repeatable
const long NAD_MUTE = 0xE13E05FA; // repeatable
const long NAD_SOURCE_UP = 0xE13E5DA2; // repeatable
const long NAD_SOURCE_DOWN = 0xE13EDD22; // repeatable
const long NAD_VOL_UP = 0xE13E44BB; // repeatable
const long NAD_VOL_DOWN = 0xE13EC43B; // repeatable
const long NAD_PRESET_RIGHT = 0xE13EC53A; // repeatable
const long NAD_PRESET_LEFT = 0xE13E25DA; // repeatable
const long NAD_REW = 0xE13EA05F; // repeatable
const long NAD_PLAY = 0xE13E807F; // repeatable
const long NAD_FOR = 0xE13E609F; // repeatable

/** Car mp3 Fernbedienung (NEC) */
const long CAR_CH_MINUS = 0xFFA25D; // repeatable
const long CAR_CH = 0xFF629D; // repeatable
const long CAR_CH_PLUS = 0xFFE21D; // repeatable
const long CAR_REW = 0xFF22DD; // repeatable
const long CAR_PLAY = 0xFFC23D; // repeatable
const long CAR_FOR = 0xFF02FD; // repeatable
const long CAR_MINUS = 0xFFE01F; // repeatable
const long CAR_PLUS = 0xFFA857; // repeatable
const long CAR_EQ = 0xFF906F; // repeatable
const long CAR_0 = 0xFF6897; // repeatable
const long CAR_100 = 0xFF9867; // repeatable
const long CAR_200 = 0xFFB04F; // repeatable
const long CAR_1 = 0xFF30CF; // repeatable
const long CAR_2 = 0xFF18E7; // repeatable
const long CAR_3 = 0xFF7A85; // repeatable
const long CAR_4 = 0xFF10EF; // repeatable
const long CAR_5 = 0xFF38C7; // repeatable
const long CAR_6 = 0xFF5AA5; // repeatable
const long CAR_7 = 0xFF42BD; // repeatable
const long CAR_8 = 0xFF4AB5; // repeatable
const long CAR_9 = 0xFF52AD; // repeatable