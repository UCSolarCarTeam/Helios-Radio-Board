#include "cmsis_os.h"
#include "stm32wlxx.h"

#include "TelemetryReporting.h"
#include "TelemetryUtils.h"
#include "TransmitSerial.h"

#include "KeyMotorData.h"
#include "MotorDetailsData.h"
#include "MotorFaultsData.h"
#include "BatteryFaultsData.h"
#include "BatteryData.h"
#include "MpptData.h"
#include "AuxBmsData.h"
#include "CanData.h"
#include "B3Data.h"

#define KEY_MOTOR_LENGTH (43)
#define MOTOR_DETAILS_LENGTH (65)
#define DRIVER_CONTROLS_LENGTH (9)
#define MOTOR_FAULTS_LENGTH (9)
#define BATTERY_FAULTS_LENGTH (6)
#define BATTERY_DETAILS_LENGTH (48)
#define MPPT_DETAILS_LENGTH (10)
#define LIGHTS_DETAILS_LENGTH (3)
#define AUX_BMS_DETAILS_LENGTH (12)
#define B3_LENGTH (9)

#define CCS_TELEM_PERIOD_MS (200) // 5Hz == 200ms

extern struct B3Data B3Data;

void sendTelemetryTask()
{
    for (;;)
    {
        osDelay(CCS_TELEM_PERIOD_MS);

        sendB3();
    }
}

void sendB3()
{
    unsigned int unframedPacketLength = B3_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = B3_PKG_ID;

    unsigned char B3AliveArray[] = {messageIsRecent(B3Data.lastReceived)};
    writeBoolsIntoArray(packetPayload, 1, B3AliveArray, 1);
    writeBoolsIntoArray(packetPayload, 2, (unsigned char *)(&B3Data.lightsInputs), 4);
    writeBoolsIntoArray(packetPayload, 3, (unsigned char *)(&B3Data.driverInputs), 11);
    writeUShortIntoArray(packetPayload, 5, B3Data.acceleration);
    writeUShortIntoArray(packetPayload, 7, B3Data.regenBraking);
    writeBoolsIntoArray(packetPayload, 9, (unsigned char *)(&(B3Data.powerOutputs)), 6);

    addChecksum(packetPayload, B3_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendKeyMotor()
{
    unsigned int unframedPacketLength = KEY_MOTOR_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = KEY_MOTOR_PKG_ID;

    unsigned char motor0AliveArray[] = {messageIsRecent(keyMotorData.m0lastReceived)};
    writeBoolsIntoArray(packetPayload, 1, motor0AliveArray, 1);
    writeFloatIntoArray(packetPayload, 2, keyMotorData.m0SetCurrent);
    writeFloatIntoArray(packetPayload, 6, keyMotorData.m0SetVelocity);
    writeFloatIntoArray(packetPayload, 10, keyMotorData.m0BusCurrent);
    writeFloatIntoArray(packetPayload, 14, keyMotorData.m0BusVoltage);
    writeFloatIntoArray(packetPayload, 18, keyMotorData.m0VehicleVelocity);
    unsigned char motor1AliveArray[] = {messageIsRecent(keyMotorData.m0lastReceived)};
    writeBoolsIntoArray(packetPayload, 22, motor1AliveArray, 1);
    writeFloatIntoArray(packetPayload, 23, keyMotorData.m1SetCurrent);
    writeFloatIntoArray(packetPayload, 27, keyMotorData.m1SetVelocity);
    writeFloatIntoArray(packetPayload, 31, keyMotorData.m1BusCurrent);
    writeFloatIntoArray(packetPayload, 35, keyMotorData.m1BusVoltage);
    writeFloatIntoArray(packetPayload, 39, keyMotorData.m1VehicleVelocity);

    addChecksum(packetPayload, KEY_MOTOR_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendMotorDetails(int n)
{
    unsigned int unframedPacketLength = MOTOR_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    switch (n)
    {
        case 0:
            packetPayload[0] = M0_DETAILS_DATA_PKG_ID;
            writeFloatIntoArray(packetPayload, 1, motor0DetailsData.phaseCCurrent);
            writeFloatIntoArray(packetPayload, 5, motor0DetailsData.phaseBCurrent);
            writeFloatIntoArray(packetPayload, 9, motor0DetailsData.motorVoltageReal);
            writeFloatIntoArray(packetPayload, 13, motor0DetailsData.motorVoltageImaginary);
            writeFloatIntoArray(packetPayload, 17, motor0DetailsData.motorCurrentReal);
            writeFloatIntoArray(packetPayload, 21, motor0DetailsData.motorCurrentImaginary);
            writeFloatIntoArray(packetPayload, 25, motor0DetailsData.backEmfImaginary);
            writeFloatIntoArray(packetPayload, 29, motor0DetailsData.railSupply15v);
            writeFloatIntoArray(packetPayload, 33, motor0DetailsData.railSupply3_3v);
            writeFloatIntoArray(packetPayload, 37, motor0DetailsData.railSupply1_9v);
            writeFloatIntoArray(packetPayload, 41, motor0DetailsData.heatSinkTemperature);
            writeFloatIntoArray(packetPayload, 45, motor0DetailsData.motorTemperature);
            writeFloatIntoArray(packetPayload, 49, motor0DetailsData.dspBoardTemp);
            writeFloatIntoArray(packetPayload, 53, motor0DetailsData.dcBusAmpHours);
            writeFloatIntoArray(packetPayload, 57, motor0DetailsData.odometer);
            writeFloatIntoArray(packetPayload, 61, motor0DetailsData.slipSpeed);
            break;

        case 1:
            packetPayload[0] = M1_DETAILS_DATA_PKG_ID;
            writeFloatIntoArray(packetPayload, 1, motor1DetailsData.phaseCCurrent);
            writeFloatIntoArray(packetPayload, 5, motor1DetailsData.phaseBCurrent);
            writeFloatIntoArray(packetPayload, 9, motor1DetailsData.motorVoltageReal);
            writeFloatIntoArray(packetPayload, 13, motor1DetailsData.motorVoltageImaginary);
            writeFloatIntoArray(packetPayload, 17, motor1DetailsData.motorCurrentReal);
            writeFloatIntoArray(packetPayload, 21, motor1DetailsData.motorCurrentImaginary);
            writeFloatIntoArray(packetPayload, 25, motor1DetailsData.backEmfImaginary);
            writeFloatIntoArray(packetPayload, 29, motor1DetailsData.railSupply15v);
            writeFloatIntoArray(packetPayload, 33, motor1DetailsData.railSupply3_3v);
            writeFloatIntoArray(packetPayload, 37, motor1DetailsData.railSupply1_9v);
            writeFloatIntoArray(packetPayload, 41, motor1DetailsData.heatSinkTemperature);
            writeFloatIntoArray(packetPayload, 45, motor1DetailsData.motorTemperature);
            writeFloatIntoArray(packetPayload, 49, motor1DetailsData.dspBoardTemp);
            writeFloatIntoArray(packetPayload, 53, motor1DetailsData.dcBusAmpHours);
            writeFloatIntoArray(packetPayload, 57, motor1DetailsData.odometer);
            writeFloatIntoArray(packetPayload, 61, motor1DetailsData.slipSpeed);
            break;
    }

    addChecksum(packetPayload, MOTOR_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendMotorFaults()
{
    unsigned int unframedPacketLength = MOTOR_FAULTS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = MOTOR_FAULTS_PKG_ID;
    writeBoolsIntoArray(packetPayload, 1, (unsigned char *)(&motorFaultsData.m0ErrorFlags), 8);
    writeBoolsIntoArray(packetPayload, 2, (unsigned char *)(&motorFaultsData.m1ErrorFlags), 8);
    writeBoolsIntoArray(packetPayload, 3, (unsigned char *)(&motorFaultsData.m0LimitFlags), 7);
    writeBoolsIntoArray(packetPayload, 4, (unsigned char *)(&motorFaultsData.m1LimitFlags), 7);
    packetPayload[5] = motorFaultsData.m0CanRxErrorCount;
    packetPayload[6] = motorFaultsData.m0CanTxErrorCount;
    packetPayload[7] = motorFaultsData.m1CanRxErrorCount;
    packetPayload[8] = motorFaultsData.m1CanTxErrorCount;

    addChecksum(packetPayload, MOTOR_FAULTS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendBatteryFaults()
{
    unsigned int unframedPacketLength = BATTERY_FAULTS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = BATTERY_FAULTS_PKG_ID;
    writeBoolsIntoArray(packetPayload, 1, (unsigned char *)(&batteryFaultsData.batteryErrorFlags), 21);
    writeBoolsIntoArray(packetPayload, 4, (unsigned char *)(&batteryFaultsData.batteryLimitFlags), 16);
    addChecksum(packetPayload, BATTERY_FAULTS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendBattery()
{
    unsigned int unframedPacketLength = BATTERY_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = BATTERY_PKG_ID;
    unsigned char BmuAliveArray[] = {messageIsRecent(batteryData.bmsLastReceived)};
    writeBoolsIntoArray(packetPayload, 1, BmuAliveArray, 1);
    writeBoolsIntoArray(packetPayload, 2, &batteryData.bmsRelayStatus, 8);
    packetPayload[3] = batteryData.populatedCells;
    writeFloatIntoArray(packetPayload, 4, batteryData.input12volt);
    writeFloatIntoArray(packetPayload, 8, batteryData.fanVoltage);
    writeFloatIntoArray(packetPayload, 12, batteryData.packCurrent);
    writeFloatIntoArray(packetPayload, 16, batteryData.packVoltage);
    writeFloatIntoArray(packetPayload, 20, batteryData.packStateofCharge);
    writeFloatIntoArray(packetPayload, 24, batteryData.packAmphours);
    writeFloatIntoArray(packetPayload, 28, batteryData.packDepthofDischarge);
    packetPayload[32] = batteryData.highTemperature;
    packetPayload[33] = batteryData.highThermistorId;
    packetPayload[34] = batteryData.lowTemperature;
    packetPayload[35] = batteryData.lowThermistorId;
    packetPayload[36] = batteryData.averageTemperature;
    packetPayload[37] = batteryData.internalTemperature;
    packetPayload[38] = batteryData.fanSpeed;
    packetPayload[39] = batteryData.requestedFanSpeed;
    writeUShortIntoArray(packetPayload, 40, batteryData.lowCellVoltage);
    packetPayload[42] = batteryData.lowCellVoltageId;
    writeUShortIntoArray(packetPayload, 43, batteryData.highCellVoltage);
    packetPayload[45] = batteryData.highCellVoltageId;
    writeUShortIntoArray(packetPayload, 46, batteryData.averageCellVoltage);

    addChecksum(packetPayload, BATTERY_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendMppt(int n)
{
    unsigned int unframedPacketLength = MPPT_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = MPPT_PKG_ID;
    unsigned char numberAndAlive = (unsigned char)n & 0x03;

    if (messageIsRecent(mpptData[n].lastReceived))
    {
        numberAndAlive |= 0x80;
    }

    packetPayload[1] = numberAndAlive;

    writeUShortIntoArray(packetPayload, 2, mpptData[n].arrayVoltage);
    writeUShortIntoArray(packetPayload, 4, mpptData[n].arrayCurrent);
    writeUShortIntoArray(packetPayload, 6, mpptData[n].batteryVoltage);
    writeUShortIntoArray(packetPayload, 8, mpptData[n].temperature);

    addChecksum(packetPayload, MPPT_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}

void sendAuxBms()
{
    unsigned int unframedPacketLength = AUX_BMS_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = AUX_BMS_PKG_ID;
    enum BatteryPrechargeState prechargeState;

    switch (auxBmsData.prechargeState)
    {
        case 0:
            prechargeState = OFF;
            break;

        case 1:
            prechargeState = COMMON_ENGAGED;
            break;

        case 3:
            prechargeState = CHARGE_ENGAGED;
            break;

        case 5:
            prechargeState = DISCHARGE_ENGAGED;
            break;

        case 7:
            prechargeState = ALL_ENGAGED;
            break;

        default:
            prechargeState = INVALID_STATE;
            break;
    }

    packetPayload[1] = prechargeState;
    packetPayload[2] = auxBmsData.auxVoltage;
    unsigned char auxBmsAliveArray[] = {messageIsRecent(auxBmsData.auxBmsLastReceived)};
    writeBoolsIntoArray(packetPayload, 3, auxBmsAliveArray, 1);
    packetPayload[4] = auxBmsData.strobeBmsLight;
    packetPayload[5] = auxBmsData.allowCharge;
    packetPayload[6] = auxBmsData.highVoltageEnableState;
    packetPayload[7] = auxBmsData.allowDischarge;
    packetPayload[8] = auxBmsData.orionCanReceivedRecently;
    writeBoolsIntoArray(packetPayload, 9, (unsigned char *)(&auxBmsData.contactorDebugInfo), 7);
    writeBoolsIntoArray(packetPayload, 10, (unsigned char *)(&auxBmsData.auxTrip), 11);

    addChecksum(packetPayload, AUX_BMS_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packet, packetLength);
}
