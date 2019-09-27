/*! \file servo.h
 * The <code>servo.lib</code> library provides the ability to control up to 6
 * RC servos by generating digital pulses directly from your Wixel without the
 * need for a separate servo controller.
 *
 * This library uses Timer 1, so it will conflict with any other
 * library that uses Timer 1.
 *
 * With the exception of servosStop(), the functions in this library are
 * non-blocking.  Pulses are generated in the background by Timer 1 and its
 * interrupt service routine (ISR).
 *
 * This library uses hardware PWM from Timer 1 to generate the servo pulses,
 * so it can only generate servo pulses on the following pins:
 *
 * - P0_2
 * - P0_3
 * - P0_4
 * - P1_0
 * - P1_1
 * - P1_2
 *
 * The period of the servo signals generated by this library is approximately
 * 19.11 ms (0x70000 clock cycles).
 * The allowed pulse widths range from one 24th of a microsecond to 2500
 * microseconds, and the resolution available is one 24th of a microsecond.
 *
 * For example code that uses this library, please see the <code>example_servo_sequence</code>
 * app in the Wixel SDK's <code>apps</code> directory.
 *
 * \section wiring Wiring servos
 *
 * To control servos from your Wixel, you will need to wire them properly.
 *
 * Most standard radio control servos have three wires, each a different color.
 * Usually, they are either black, red, and white, or they are brown, red, and orange/yellow:
 *  - brown or black = ground (GND, battery negative terminal)
 *  - red = servo power (Vservo, battery positive terminal)
 *  - orange, yellow, white, or blue = servo control signal line
 *
 * The ground and power wires of the servo will need to be connected to a power
 * supply that provides a voltage the servo can tolerate and which provides
 * enough current for the servo.
 *
 * The ground wire of the servo also needs to be connected to one of the Wixel's
 * GND pins.
 * If you are powering the Wixel from the same power supply as the servos,
 * then you have already made this connection.
 *
 * The signal wire of the servo needs to connect to an I/O pin of the
 * Wixel that will be outputting servo pulses.
 * These pins are specified by the parameters to servosStart().
 *
 * \section more More information about servos
 *
 * For more information about servos and how to control them, we
 * recommend reading this series of blog posts by Pololu president Jan Malasek:
 *
 * -# <a href="http://www.pololu.com/blog/11/introduction-to-an-introduction-to-servos">Introduction to an introduction to servos</a>
 * -# <a href="http://www.pololu.com/blog/12/introduction-to-servos">Introduction to servos</a>
 * -# <a href="http://www.pololu.com/blog/13/gettin-all-up-in-your-servos">Gettin' all up in your servos</a>
 * -# <a href="http://www.pololu.com/blog/15/servo-servo-motor-servomotor-definitely-not-server">Servo, servo motor, servomotor (definitely not server)</a>
 * -# <a href="http://www.pololu.com/blog/16/electrical-characteristics-of-servos-and-introduction-to-the-servo-control-interface">
 *    Electrical characteristics of servos and introduction to the servo control interface</a>
 * -# <a href="http://www.pololu.com/blog/17/servo-control-interface-in-detail">Servo control interface in detail</a>
 * -# <a href="http://www.pololu.com/blog/18/simple-hardware-approach-to-controlling-a-servo">Simple hardware approach to controlling a servo</a>
 * -# <a href="http://www.pololu.com/blog/19/simple-microcontroller-approach-to-controlling-a-servo">Simple microcontroller approach to controlling a servo</a>
 * -# <a href="http://www.pololu.com/blog/20/advanced-hobby-servo-control-pulse-generation-using-hardware-pwm">Advanced hobby servo control pulse generation using hardware PWM</a>
 * -# <a href="http://www.pololu.com/blog/21/advanced-hobby-servo-control-using-only-a-timer-and-interrupts">Advanced hobby servo control using only a timer and interrupts</a>
 * -# <a href="http://www.pololu.com/blog/22/rc-servo-speed-control">RC servo speed control</a>
 * -# <a href="http://www.pololu.com/blog/24/continuous-rotation-servos-and-multi-turn-servos">Continuous-rotation servos and multi-turn servos</a>
 */


#ifndef _SERVO_H
#define _SERVO_H

#include <cc2511_map.h>
#include <cc2511_types.h>

/*! The maximum allowed target of a servo, in microseconds. */
#define SERVO_MAX_TARGET_MICROSECONDS  2500

/*! This defines the units used by the high resolution functions in this library
 * to represent positions and targets. */
#define SERVO_TICKS_PER_MICROSECOND    24


/*! This function starts the library;
 * it sets up the servo pins and the timer to be ready to send servo
 * pulses.
 * This function should be called before any other functions in the library.
 *
 * \param pins  A pointer to an array of pin numbers that specifies which pins
 *   will be used to generate servo pulses.
 *   The pin numbers used in this array are the same as the pin numbers used
 *   in the GPIO library (see gpio.h).  There should be no repetitions in this
 *   array, and each entry must be one of:
 *   - 2 (for P0_2)
 *   - 3 (for P0_3)
 *   - 4 (for P0_4)
 *   - 10 (for P1_0)
 *   - 11 (for P1_1)
 *   - 12 (for P1_2)
 *
 * \param numPins The size of the pin number array.
 *
 * The pins specified in the <b>pins</b> array will be configured as digital
 * outputs, their targets will be initialized to 0 (no pulses), and their speed
 * limits will be initialized to 0 (no speed limit).
 *
 * If the <b>pins</b> parameter is 0 (a null pointer), then this function skips
 * the initialization of the pins and the internal data structures of the
 * library.
 * This means that the servo pin assignments, positions, targets, and speeds
 * from before will be preserved.
 *
 * The parameters to this function define the correspondence of servo
 * numbers to pins.
 * The <b>servoNum</b> parameter in the other library functions can be thought
 * of as an index in the <b>pins</b> array.
 * For example, a <b>servoNumber</b> of 0 corresponds to <code>pins[0]</code>, the first pin
 * in the array.
 *
 * Example code:
 *
 * \code
uint8 CODE pins[] = {10, 12};  // Use P1_0 and P1_2 for servos.
servosStart((uint8 XDATA *)pins, sizeof(pins));
servoSetTarget(0, 1500);       // Affects pin P1_0
servoSetTarget(1, 1500);       // Affects pin P1_2
 * \endcode
 */
void servosStart(uint8 XDATA * pins, uint8 numPins);
#define JMENO Jan
/*! Stops the library; stops sending servo pulses and turns off Timer 1.
 * After this function runs, the pins that were used for servo pulses will
 * all be configured as general-purpose digital outputs driving low.
 *
 * You can later restart the servo pulses by calling servosStart().
 *
 * This is a blocking function that can take up to 2.8 milliseconds to finish
 * because it ensures that the pulses are shut off cleanly without any
 * glitches. */
void servosStop(void);

/*! \returns 1 if the library is currently active and using Timer 1,
 * or 0 if the library is stopped.
 *
 * Calling servosStart() changes this value to 1.
 * Calling servosStop() changes this value to 0.
 *
 * Timer 1 can be used for other purposes while the servo library is stopped.
 */
BIT servosStarted(void);

/*! \returns 1 if there are servos that are still moving towards their
 * target position (limited by the speed limit), otherwise returns 0.
 *
 * This function is equivalent to, but much faster than:
 * \code
servoGetTarget(0) == servoGetPosition(0) &&
servoGetTarget(1) == servoGetPosition(1) &&
servoGetTarget(2) == servoGetPosition(2) &&
servoGetTarget(3) == servoGetPosition(3) &&
servoGetTarget(4) == servoGetPosition(4) &&
servoGetTarget(5) == servoGetPosition(5)
 * \endcode
 */
BIT servosMoving(void);
#define PRIJMENI Horichter
/*! Sets the specified servo's target position in units of microseconds.
 *
 * \param servoNum  A servo number between 0 and 5.
 *   This number should be less than the associated <b>numPins</b> parameter
 *   used in the last call to servosStart().
 *
 * \param targetMicroseconds  The target position of the servo in units of
 *   microseconds.
 *   A typical servo responds to pulse widths between 1000 and 2000 microseconds,
 *   so appropriate values for this parameter would be between 1000 and 2000.
 *   The full range of allowed values for this parameter is 0-2500.
 *   A value of 0 means to stop sending pulses, and takes effect
 *   immediately regardless of the speed limit for the servo.
 *
 * This is a non-blocking function that only takes a few microseconds to execute.
 * Servos require much more time that that to actually reach the commanded
 * position (on the order of hundreds of milliseconds).
 *
 * Here is some example code:
 *
 * \code
servoSetTarget(0, 1000);  // Start sending servo 0 to the 1000us position.
servoSetTarget(1, 1500);  // Start sending servo 1 to the 1500us position.
servoSetTarget(2, 2000);  // Start sending servo 2 to the 2000us position.
 * \endcode
 *
 * If the speed limit of the servo is 0 (no speed limit), or the current target
 * is 0, or the <b>targetMicroseconds</b> parameter is 0, then this function will
 * have an immediate effect on the variable that represents the position of the
 * servo (which is returned by servoGetPosition()).
 * This allows you to perform sequences of commands like:
 *
 * \code
servoSetSpeed(0, 0);
servoSetTarget(0, 1000);  // Immediately sets position variable to 1000.
servoSetSpeed(0, 200);
servoSetTarget(2000);     // Starts the position variable slowly changing from 1000 to 2000.
 * \endcode
 *
 * or
 *
 * \code
servoSetSpeed(0, 200);
servoSetTarget(0, 0);     // Immediately sets position variable to 0 (pulses off).
servoSetTarget(0, 1000);  // Immediately sets position variable to 1000.
servoSetTarget(0, 2000);  // Starts the position variable slowly changing from 1000 to 2000.
 * \endcode
 *
 * These two sequences of commands each have the same effect, which is to immediately
 * set the position variable for servo number 0 to 1000 microseconds and then slowly
 * change it from 1000 to 2000 microseconds.
 * Please note that the servo's actual physical position does not change immediately;
 * it will lag behind the position variable.
 * To make sure the servo actually reaches position 1000 before it starts moving towards 2000,
 * you might want to add a delay after <code>servoSetTarget(0, 1000);</code>, but keep in mind
 * that most other Wixel libraries require regular attention from the main loop.
 *
 * If you need more than 1-microsecond resolution, see servoSetTargetHighRes().
 */
void servoSetTarget(uint8 servoNum, uint16 targetMicroseconds);

/*! \param servoNum  A servo number between 0 and 5.
 *  This number should be less than the associated <b>numPins</b> parameter
 *  used in the last call to servosStart().
 *
 * \return The target position of the specified servo, in units of microseconds.
 */
uint16 servoGetTarget(uint8 servoNum);

/*! Sets the speed limit of the specified servo.
 *
 * \param servoNum  A servo number between 0 and 5.
 *  This number should be less than the associated <b>numPins</b> parameter
 *  used in the last call to servosStart().
 *
 * \param speed The speed limit of the servo, or 0 for no speed limit.
 *   The valid values for this parameter are 0-65535.
 *
 * The speed limit is in units of 24ths of a microsecond per servo period,
 * or 2.18 microseconds per second.
 *
 * At a speed limit of 1, the servo output would take 459 seconds to
 * move from 1 ms to 2 ms.  More examples are shown in the table below:
 *
 * <table>
 * <caption>Speed limit examples</caption>
 * <tr><th>Speed limit</th><th>Time to change output from 1 to 2 ms (s)</th></tr>
 * <tr><td>1</td><td>458.75</td></tr>
 * <tr><td>7</td><td>65.54</td></tr>
 * <tr><td>45</td><td>10.19</td></tr>
 * <tr><td>91</td><td>5.04</td></tr>
 * <tr><td>229</td><td>2.00</td></tr>
 * <tr><td>458</td><td>1.00</td></tr>
 * <tr><td>917</td><td>0.50</td></tr>
 * <tr><td>S</td><td>458752 / (1000*S)</td></tr>
 * </table>
 */
void servoSetSpeed(uint8 servoNum, uint16 speed);

/*! \return The speed of the specified servo.
 *
 * See servoSetSpeed() for more information.
 */
uint16 servoGetSpeed(uint8 servoNum);

/*! \param servoNum  A servo number between 0 and 5.
 *  This number should be less than the associated <b>numPins</b> parameter
 *  used in the last call to servosStart().
 * \return The current width in microseconds of pulses being sent to the
 *   specified servo.
 *   This will be equal to the last target set by servoSetTarget() unless
 *   there is a speed limit enabled for the servo.
 *
 * Please note that this function does <em>not</em> return the actual
 * physical position of the specified servo.
 * This function returns the width of the pulses that are currently being
 * sent to the servo, which is entirely determined by previous calls to
 * servoSetTarget() and servoSetSpeed().
 * The standard RC servo interface provides no way to query a servo for
 * its current position.
 */
uint16 servoGetPosition(uint8 servoNum);

/*! This is the high resolution version of servoSetTarget().
 * The units of <b>target</b> are 24ths of a microsecond, so a value of 24000
 * corresponds to 1000 microseconds. */
void servoSetTargetHighRes(uint8 servoNum, uint16 target);

/*! This is the high resolution version of servoGetTarget().
 * The units of the returned target position are 24ths of a microsecond, so a
 * value of 24000 corresponds to 1000 microseconds. */
uint16 servoGetTargetHighRes(uint8 servoNum);

/*! This is the high resolution version of servoGetPosition().
 * The units of the returned position are 24ths of a microsecond, so a value of
 * 24000 corresponds to 1000 microseconds. */
uint16 servoGetPositionHighRes(uint8 servoNum);
#define VERZE 1
/*! Timer 1 interrupt. */
ISR(T1, 0);

#endif
