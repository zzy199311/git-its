#ifndef __CONSTANT_H
#define __CONSTANT_H

#include <linux/types.h>

#define AUTO 0
#define MANAGED 1
#define ANONYMOUS 2
#define GREEDY 1
#define ADVANCED 3

// all consts convert to #define!

//const int itsGnLocalGnAddr = 1;
//const int itsGnLocalAddrConfMethod = MANAGED;
//const int itsGnProtocolVersion = 0;
const enum itsGnStationType{
    Unknown = 0,
    Pedestrian = 1,
    Cyclist = 2,
    Moped = 3,
    Motorcycle = 4,
    PassengerCar = 5,
    Bus = 6,
    LightTruck = 7,
    HeavyTruck = 8,
    Trailer = 9,
    SpecialVechicles = 10,
    Tram = 11,
    RoadSideUnit = 15

};
const enum itsGnlsMobile{
    Stationary = 0,
    Mobile = 1
};

const enum itsGnlfType{
    Unspecified = 0,
    ITS_G5 = 1
};
/*
const int itsGnMinUpdateFrequencyLPV = 1000;
const int itsGnPaiInterval = 80;
const int itsGnMaxSduSize = 1398;
const int itsGnLifetimeLocTE = 20;
*/
const enum itsGnSecurity{
    DISABLED,
    ENABLED
};
const enum itsGnSnDecapResultHandling{
    STRICT = 0,
    NON_STRICT = 1
};
/*
const int itsGnLocationServiceMaxRetrans = 10;
const int itsGnLocationServiceRetransmitTimer = 1000;
const int itsGnLocationServicePacketBufferSize = 1024;
const int itsGnBeaconServiceRetransmitTimer = 3000;
const int itsGnBeaconServiceMaxJitter = 3000/4;
const int itsGnDefaultHopLimit = 10;
const int itsGnMaxPacketLifetime = 600;
const int itsGnDefaultPacketLifetime = 60;
const int itsGnMaxPacketDataRate = 100;
const int itsGnMaxPacketDataRateEmaBeta = 90;
const int itsGnMaxGepAreaSize = 10;
const int itsGnMinPacketRepetitionInterval = 100;
const int itsGnGeoUnicasrtForwardingAlgorithm = GREEDY;
const int itsGnGeoBroadcastForwardingAlgorithm = ADVANCED;
const int itsGnGeoUnicastCnfMinTime = 1;
const int itsGnGeoUnicastCbfMaxTime = 100;
const int itsGnGeoBroadcastCbfMinTime = 1;
const int itsGnGeoBroadcastCbfMaxTime = 100;
const int itsGnDefaultMaxCommunicationRange = 1000;
const int itsGnBroadcastCBFDefSectorAngle = 30;
const int itsGnUnicastCBFDefSectorAngle = 30;
const int itsGnGeoAreaLineForwarding = 1;
const int itsGnUcForwardingPacketBufferSize = 256;
const int itsGnBcForwardingPacketBufferSzie = 1024;
const int itsGnCbfPacketBufferSize = 256;
const uint8_t itsGnDefaultTrafficeClass = 0x00;
*/

#endif
