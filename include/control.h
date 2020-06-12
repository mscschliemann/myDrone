/**
* @brief Controls the program flow
* 
* @file	bmp280.h
* @date	2020-01-10
* @version	v3.3.4
*
*/

/** INIT 
 * 
 * a) Reset + MPU9250::initialize()
 *    Gyro 
 *         Scale: +- 250 dps
 *         Sample Rate: 8000 Hz
 *         LPF: 250 Hz
 *    Accel
 *         Scale: +- 2 g
 *         Sample Rate: 1000 Hz
 *         LPF: 460 Hz
 *    Mag
 *         single measurement mode
 *      
 * b) Reset + Kwiner
 *    Gyro
 *         Scale: +- 250 dps
 *         Sample Rate: 200 Hz
 *         LPF: 41 Hz
 *    Accel
 *         Scale: +- 2 g
 *         Sample Rate: 1000 Hz
 *         LPF: 41 Hz
 *    Mag
 *         16-bit Resolution, continuos measurement mode 1
 */

#define MPU9250_UPDATE_OFFSET_REGISTERS
#define MPU9250_INIT
// #define KWINER_INIT

/** A fetch RAW Data
 * a) Read A,G,M Registers
 * b) use FiFO (+ FIFO Interrupt)
 * c) use DMP
 */

#define READ_RAW_DATA_BY_REGISTERS // be aware of Mag mesurement mode!!
//#define READ_RAW_DATA_BY_FIFO
//#define READ_RAW_DATA_BY_DMP

/** C apply Biases
 * - use accel, gyro Offset Registers only (if registers have been updated during init!!)
 * - use accel, gyro Offset with premeasured Bias
*/


/** Debug options
 * 
 * 
*/
#define SHOW_DEBUG     1;

/* void serial_output(decode_results IR_Signal, long prev) {
  switch (IR_Signal.decode_type) {
    case NEC:
      Serial.print("NEC: ");
      break;
    case SONY:
      Serial.print("SONY: ");
      break;
    case RC5:
      Serial.print("RC5: ");
      break;
    case RC6:
      Serial.print("RC6: ");
      break;
    case UNKNOWN:
      Serial.print("UNKNOWN: ");
      break;
    default:
      break;
  }
  Serial.println(IR_Signal.value, HEX);
  Serial.println(prev, HEX);

} */

/* void debug_output() {
  Serial.print("pid roll: ");
  Serial.println(pid_output_roll);
  Serial.print("pid pitch: ");
  Serial.println(pid_output_pitch);
  Serial.print("pid yaw: ");
  Serial.println(pid_output_yaw);
} */