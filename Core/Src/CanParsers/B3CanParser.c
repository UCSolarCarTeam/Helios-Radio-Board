#include "cmsis_os.h"
#include "B3Data.h"
#include "CanData.h"

#define B3_HEARTBEAT_ID     (0x600)
#define LIGHT_INPUTS_ID     (0x610)
#define DIGITAL_INPUTS_ID   (0x611)
#define ANALOG_INPUTS_ID    (0x612)
#define LIGHTS_STATUS_ID    (0x620)
#define GPS_TIME_ID         (0x630)
#define GPS_FLAGS_ID        (0x631)
#define GPS_POSITION_ID     (0x632)
#define MPU_ACCELERATION_ID (0x633)
#define MPU_GYROSCOPE_ID    (0x634)
#define MPU_TEMPERATURE_ID  (0x635)

struct B3Data B3Data = {0};

void parseB3CanMessage(uint32_t stdId, uint8_t* data)
{
    switch (stdId)
    {
        case B3_HEARTBEAT_ID:
            parseB3Heartbeat();
            break;

        case LIGHT_INPUTS_ID:
            parseLightInputs(data);
            break;

        case DIGITAL_INPUTS_ID:
            parseDigitalInputs(data);
            break;

        case ANALOG_INPUTS_ID:
            parseAnalogInputs(data);
            break;

        case LIGHTS_STATUS_ID:
            parseLightStatus(data);
            break;

        case GPS_TIME_ID:
            parseGpsTime(data);
            break;

        case GPS_FLAGS_ID:
            parseGpsFlags(data);
            break;

        case GPS_POSITION_ID:
            parseGpsPosition(data);
            break;

        case MPU_ACCELERATION_ID:
            parseMpuAccel(data);
            break;

        case MPU_GYROSCOPE_ID:
            parseMpuGyro(data);
            break;

        case MPU_TEMPERATURE_ID:
            parseMpuTemp(data);
            break;
    }
}

void parseB3Heartbeat()
{
    B3Data.lastReceived = osKernelSysTick();
}

void parseLightInputs(uint8_t* data)
{
    B3Data.lightsInputs.signalRight  = data[0] & DRIVER_CONTROL_RIGHT_SIGNAL_INPUT;
    B3Data.lightsInputs.signalLeft   = data[0] & DRIVER_CONTROL_LEFT_SIGNAL_INPUT;
    B3Data.lightsInputs.hazard       = data[0] & DRIVER_CONTROL_HAZARD_LIGHTS_INPUT;
    B3Data.lightsInputs.headlightOn  = data[0] & DRIVER_CONTROL_HEADLIGHTS_INPUT;
}

void parseDigitalInputs(uint8_t* data)
{
    B3Data.driverInputs.forward         = data[0] & DRIVER_CONTROL_FORWARD;
    B3Data.driverInputs.reverse         = data[0] & DRIVER_CONTROL_REVERSE;
    B3Data.driverInputs.neutral         = data[0] & DRIVER_CONTROL_NEUTRAL;
    B3Data.driverInputs.horn            = data[0] & DRIVER_CONTROL_HORN;
    B3Data.driverInputs.brakes          = data[0] & DRIVER_CONTROL_BRAKE;
    B3Data.driverInputs.handbrakeSwitch = data[0] & DRIVER_CONTROL_HANDBRAKE;
    B3Data.driverInputs.motorReset      = data[0] & DRIVER_CONTROL_MOTOR_RESET;
    B3Data.driverInputs.raceMode        = data[0] & DRIVER_CONTROL_RACE_MODE;
    B3Data.driverInputs.lap             = data[1] & DRIVER_CONTROL_LAP;
    B3Data.driverInputs.ZoomZoom        = data[1] & DRIVER_CONTROL_ZOOM_ZOOM;
}

void parseAnalogInputs(uint8_t* data)
{
    B3Data.acceleration =
        ((data[0] & 0xFF) << 0) | // bits 0-7
        ((data[1] & 0x0F) << 8);  // bits 8-11

    B3Data.regenBraking = // bits 12-23
        ((data[2] & 0xF0) >> 4) | // bits 12-19
        ((data[3] & 0xFF) << 4);  // bits 20-23
}

void parseLightStatus(uint8_t* data)
{
    B3Data.powerOutputs.signalRight               = data[0] & POWER_BOARD_RIGHT_SIGNAL;
    B3Data.powerOutputs.signalLeft                = data[0] & POWER_BOARD_LEFT_SIGNAL;
    B3Data.powerOutputs.signalDaytimeRunningLight = data[0] & POWER_BOARD_DAYTIME_LIGHT_SIGNAL;
    B3Data.powerOutputs.signalHeadlights          = data[0] & POWER_BOARD_HEADLIGHTS_SIGNAL;
    B3Data.powerOutputs.signalBrakeLight          = data[0] & POWER_BOARD_BRAKE_LIGHT_SIGNAL;
    B3Data.powerOutputs.signalHorn                = data[0] & POWER_BOARD_HORN_SIGNAL;
}

void parseGpsTime(uint8_t* data)
{
    B3Data.telemetryData.year =
        ((data[0] & 0xFF) << 0) | // bits 0-7
        ((data[1] & 0x0F) << 8);  // bits 8-11
    B3Data.telemetryData.month =
        ((data[2] & 0xFF) << 0); // bits 12-23
    B3Data.telemetryData.day =
        ((data[3] & 0xFF) << 0); // bits 24-31
    B3Data.telemetryData.hour =
        ((data[4] & 0xFF) << 0); // bits 32-39
    B3Data.telemetryData.min =
        ((data[5] & 0xFF) << 0); // bits 40-47
    B3Data.telemetryData.sec =
        ((data[6] & 0xFF) << 0); // bits 47-55
}

void parseGpsFlags(uint8_t* data)
{
    B3Data.GPSFlags.validityFlag1  = data[0] & TELEMETRY_VALIDITY_FLAG_1;
    B3Data.GPSFlags.validityFlag2  = data[0] & TELEMETRY_VALIDITY_FLAG_2;
    B3Data.GPSFlags.validityFlag3  = data[0] & TELEMETRY_VALIDITY_FLAG_3;
    B3Data.GPSFlags.validityFlag4  = data[0] & TELEMETRY_VALIDITY_FLAG_4;
    B3Data.GPSFlags.validityFlag5  = data[0] & TELEMETRY_VALIDITY_FLAG_5;
    B3Data.GPSFlags.validityFlag6  = data[0] & TELEMETRY_VALIDITY_FLAG_6;
    B3Data.GPSFlags.validityFlag7  = data[0] & TELEMETRY_VALIDITY_FLAG_7;
    B3Data.GPSFlags.validityFlag8  = data[0] & TELEMETRY_VALIDITY_FLAG_8;
    B3Data.GPSFlags.fixStatusFlag1  = data[1] & TELEMETRY_FIX_STATUS_FLAG_1;
    B3Data.GPSFlags.fixStatusFlag2  = data[1] & TELEMETRY_FIX_STATUS_FLAG_2;
    B3Data.GPSFlags.fixStatusFlag3  = data[1] & TELEMETRY_FIX_STATUS_FLAG_3;
    B3Data.GPSFlags.fixStatusFlag4  = data[1] & TELEMETRY_FIX_STATUS_FLAG_4;
    B3Data.GPSFlags.fixStatusFlag5  = data[1] & TELEMETRY_FIX_STATUS_FLAG_5;
    B3Data.GPSFlags.fixStatusFlag6  = data[1] & TELEMETRY_FIX_STATUS_FLAG_6;
    B3Data.GPSFlags.fixStatusFlag7  = data[1] & TELEMETRY_FIX_STATUS_FLAG_7;
    B3Data.GPSFlags.fixStatusFlag8  = data[1] & TELEMETRY_FIX_STATUS_FLAG_8;
    B3Data.GPSFlags.additionalFlag1  = data[2] & TELEMETRY_ADDITIONAL_FLAG_1;
    B3Data.GPSFlags.additionalFlag2  = data[2] & TELEMETRY_ADDITIONAL_FLAG_2;
    B3Data.GPSFlags.additionalFlag3  = data[2] & TELEMETRY_ADDITIONAL_FLAG_3;
    B3Data.GPSFlags.additionalFlag4  = data[2] & TELEMETRY_ADDITIONAL_FLAG_4;
    B3Data.GPSFlags.additionalFlag5  = data[2] & TELEMETRY_ADDITIONAL_FLAG_5;
    B3Data.GPSFlags.additionalFlag6  = data[2] & TELEMETRY_ADDITIONAL_FLAG_6;
    B3Data.GPSFlags.additionalFlag7  = data[2] & TELEMETRY_ADDITIONAL_FLAG_7;
    B3Data.GPSFlags.additionalFlag8  = data[2] & TELEMETRY_ADDITIONAL_FLAG_8;
}

void parseGpsPosition(uint8_t* data)
{
    B3Data.telemetryData.longitude =
        ((data[0] & 0xFF) << 0) | // bits 0-7
        ((data[1] & 0xFF) << 8) | // bits 7-15
        ((data[2] & 0xFF) << 16) | // bits 15-23
        ((data[3] & 0xFF) << 24);  // bits 23-31
    B3Data.telemetryData.latitude =
        ((data[4] & 0xFF) << 0) | // bits 32-39
        ((data[5] & 0xFF) << 8) | // bits 40-47
        ((data[6] & 0xFF) << 16) | // bits 48-55
        ((data[7] & 0xFF) << 24);  // bits 56-63
}

void parseMpuAccel(uint8_t* data)
{
    B3Data.telemetryData.accelerationX =
    ((data[0] & 0xFF) << 0) | // bits 0-7
    ((data[1] & 0xFF) << 8);  // bits 8-15
    B3Data.telemetryData.accelerationY =
    ((data[2] & 0xFF) << 0) | // bits 16-23
    ((data[3] & 0xFF) << 8);  // bits 24-31
    B3Data.telemetryData.accelerationZ =
    ((data[4] & 0xFF) << 0) | // bits 31-39
    ((data[5] & 0xFF) << 8);  // bits 40-47
}

void parseMpuGyro(uint8_t* data)
{
    B3Data.telemetryData.rotationX =
    ((data[0] & 0xFF) << 0) | // bits 0-7
    ((data[1] & 0xFF) << 8);  // bits 8-15
    B3Data.telemetryData.rotationY =
    ((data[2] & 0xFF) << 0) | // bits 16-23
    ((data[3] & 0xFF) << 8);  // bits 24-31
    B3Data.telemetryData.rotationZ =
    ((data[4] & 0xFF) << 0) | // bits 31-39
    ((data[5] & 0xFF) << 8);  // bits 40-47
}

void parseMpuTemp(uint8_t* data)
{
    B3Data.telemetryData.temperature =
    ((data[0] & 0xFF) << 0) | // bits 0-7
    ((data[1] & 0xFF) << 8);  // bits 8-15
}
