#pragma once

#include "stm32wlxx.h"

// Defined at https://uofc.sharepoint.com/:x:/r/sites/CalgarySolarCarTeam/Shared%20Documents/03%20Software/UofC%20Solar%20Car%20Helios%20Communications%20Protocol%20Template.xlsx?d=w9dff557773a645469706762c31dcd890&csf=1&web=1&e=FRl1oO
// Under software files in solar car teams
#define B3_PKG_ID 4

struct LightsInputs
{
    unsigned char headlightOn;
    unsigned char signalRight;
    unsigned char signalLeft;
    unsigned char hazard;
};

struct DriverInputs
{
    unsigned char forward;
    unsigned char reverse;
    unsigned char neutral;
    unsigned char horn;
    unsigned char brakes;
    unsigned char handbrakeSwitch;
    unsigned char motorReset;
    unsigned char raceMode;
    unsigned char lap;
    unsigned char ZoomZoom;
};

struct PowerOutputs
{
    unsigned char signalRight;
    unsigned char signalLeft;
    unsigned char signalDaytimeRunningLight;
    unsigned char signalHeadlights;
    unsigned char signalBrakeLight;
    unsigned char signalHorn;
};

struct TelemetryData
{
    unsigned short int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char GPSValidityFlags;
    unsigned char fixStatusFlag;
    unsigned char additionalFlags;
    unsigned int longitude;
    unsigned int latitude;
    unsigned short int accelerationX;
    unsigned short int accelerationY;
    unsigned short int accelerationZ;
    unsigned short int rotationX;
    unsigned short int rotationY;
    unsigned short int rotationZ;
    unsigned short int temperature;
};

struct GPSFlags 
{
    unsigned char validityFlag1;
    unsigned char validityFlag2;
    unsigned char validityFlag3;
    unsigned char validityFlag4;
    unsigned char validityFlag5;
    unsigned char validityFlag6;
    unsigned char validityFlag7;
    unsigned char validityFlag8;
    unsigned char fixStatusFlag1;
    unsigned char fixStatusFlag2;
    unsigned char fixStatusFlag3;
    unsigned char fixStatusFlag4;
    unsigned char fixStatusFlag5;
    unsigned char fixStatusFlag6;
    unsigned char fixStatusFlag7;
    unsigned char fixStatusFlag8;
    unsigned char additionalFlag1;
    unsigned char additionalFlag2;
    unsigned char additionalFlag3;
    unsigned char additionalFlag4;
    unsigned char additionalFlag5;
    unsigned char additionalFlag6;
    unsigned char additionalFlag7;
    unsigned char additionalFlag8;
};

struct B3Data
{
    uint32_t lastReceived;
    struct LightsInputs lightsInputs;
    struct DriverInputs driverInputs;
    unsigned short int acceleration;
    unsigned short int regenBraking;
    struct PowerOutputs powerOutputs;
    struct TelemetryData telemetryData;
    struct GPSFlags GPSFlags;
};

extern struct B3Data B3Data;

void parseB3CanMessage(uint32_t stdId, uint8_t* data);
void parseB3Heartbeat();
void parseLightInputs(uint8_t* data);
void parseDigitalInputs(uint8_t* data);
void parseAnalogInputs(uint8_t* data);
void parseLightStatus(uint8_t* data);
void parseGpsTime(uint8_t* data);
void parseGpsFlags(uint8_t* data);
void parseGpsPosition(uint8_t* data);
void parseMpuAccel(uint8_t* data);
void parseMpuGyro(uint8_t* data);
void parseMpuTemp(uint8_t* data);
