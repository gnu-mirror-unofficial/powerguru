// 
// Copyright (C) 2005, 2006-2018.
//      Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __OUTBACK_POWER_H__
#define __OUTBACK_POWER_H__

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstring>
#include "serial.h"
#include "console.h"
#include "database.h"

// The outback has two variations on protocols, one for the FX
// inverter series, and the other for the MX charge controller
// series. Each packet is 49 bytes, and is transmitted as ASCII
// numerals for portability, with an ASCII comma character
// seperating each field.

// Outback FX Inverter protocol
struct outback_fx
{
    char start;                   // 1. This is an ASCII(10) New Line character
    // denoting the start of the status page.
    char address;                 // 2. This is the Inverter address.
    char comma1;                  // 3. ASCII(44) a comma as a data separator.
    char inverter_current[2];     // 4. High byte of Inverter current.
    // 5. Low byte of Inverter current.
    char comma2;                  // 6. ASCII(44) a comma as a data separator.
    char charger_current[2];      // 7. High byte of Charger current..
    // 8. Low byte of Charger current.
    char comma3;                  // 9. ASCII(44) a comma as a data separator.
    char buy_current[2];          // 10. High byte of Buy current..
    // 11. Low byte of Buy current.
    char comma4;                  // 12. ASCII(44) a comma as a data separator.
    char ac_input_voltage[3];     // 13. High byte of the AC input voltage.
    // 14. Middle byte of AC input voltage.
    // 15. Low byte of AC input voltage.
    char comma5;                  // 16. ASCII(44) a comma as a data separator.
    char ac_output_voltage[3];    // 17. High byte of the AC output voltage.
    // 18. Middle byte of AC output voltage.
    // 19. Low byte of AC output voltage.
    char comma6;                  // 20. ASCII(44) a comma as a data separator.
    char sell_current[2];         // 21. High byte of Sell current.
    // 22. Low byte of Sell current.
    char comma7;                  // 23. ASCII(44) a comma as a data separator.
    char operating_mode[2];       // 24. High byte of FX operating mode.
    // 25. Low byte of FX operating mode.
    char comma8;                  // 26. ASCII(44) a comma as a data separator.
    char error_mode[3];           // 27. High byte of FX Error mode.
    // 28. Middle byte of FX Error mode.
    // 29. Low byte of FX Error mode.
    char comma9;                  // 30. ASCII(44) a comma as a data separator.
    char ac_mode[2];              // 31. High byte of FX AC mode.
    // 32. Low byte of FX AC mode
    char comma10;                 // 33. ASCII(44) a comma as a data separator.
    char battery_voltage[3];      // 34. High byte of FX battery voltage.
    // 35. Middle byte of FX battery voltage.
    // 36. Low byte of FX battery voltage.
    char comma11;                 // 37. ASCII(44) a comma as a data separator.
    char misc[3];                 // 38. High byte of FX Misc.
    // 39. Middle byte of FX Misc.
    // 40. Low byte of FX Misc.
    char comma12;                 // 41. ASCII(44) a comma as a data separator.
    char warning_mode[3];         // 42. High byte of FX Warning mode.
    // 43. Middle byte of FX Warning mode.
    // 44. Low byte of FX Warning mode.
    char comma13;                 // 45. ASCII(44) a comma as a data separator.
    char checksum[3];             // 46. High byte of Chksum.
    // 47. Middle byte of Chksum.
    // 48. Low byte of Chksum.
    char end;                     // 49. ASCII(13) carriage return. Denotes end of status page.*

};

// Outback MX charger controller protocol

struct outback_mx
{
    char start;                   // 1. This is an ASCII(10) New Line character
    // denoting the start of the status page.
    char address;                 // 2. This is the MX address.
    char comma1;                  // 3. ASCII(44) a comma as a data separator.
    char unused1[2];              // 4. Unused, ASCII(48).
    // 5. Unused, ASCII(48).
    char comma2;                  // 6. ASCII(44) a comma as a data separator.
    char charger_current[2];      // 7. High byte of Charger current.
    // 8. Low byte of Charger current.
    char comma3;                  // 9. ASCII(44) a comma as a data separator.
    char pv_current[2];           // 10. High byte of PV current.
    // 11. Low byte of PV current.
    char comma4;                  // 12. ASCII(44) a comma as a data separator.
    char pv_input_voltage[3];     // 13. High byte of the PV input voltage.
    // 14. Middle byte of PV input voltage.
    // 15. Low byte of PV input voltage.
    char comma5;                  // 16. ASCII(44) a comma as a data separator.
    char daily_kwh[3];            // 17. High byte of Daily KWH.
    // 18. Middle byte of Daily KWH.
    // 19. Low byte of Daily KWH.
    char comma6;                  // 20. ASCII(44) a comma as a data separator.
    char unused2[2];              // 21. Unused, ASCII(48).
    // 22. Unused, ASCII(48).
    char comma7;                  // 23. ASCII(44) a comma as a data separator.
    char aux_mode[2];             // 24. High byte of MX Aux mode.
    // 25. Low byte of MX Aux mode.
    char comma8;                  // 26. ASCII(44) a comma as a data separator.
    char error_mode[3];           // 27. High byte of MX Error mode.
    // 28. Middle byte of MX Error mode.
    // 29. Low byte of MX Error mode.
    char comma9;                  // 30. ASCII(44) a comma as a data separator.
    char charger_mode[2];         // 31. High byte of MX charger mode.
    // 32. Low byte of MX charger mode.
    char comma10;                 // 33. ASCII(44) a comma as a data separator.
    char battery_voltage[3];      // 34. High byte of MX battery voltage.
    // 35. Middle byte of MX battery voltage.
    // 36. Low byte of MX battery voltage.
    char comma11;                 // 37. ASCII(44) a comma as a data separator.
    char unused3[3];              // 38. Unused, ASCII(48).
    // 39. Unused, ASCII(48).
    // 40. Unused, ASCII(48).
    char comma12;                 // 41. ASCII(44) a comma as a data separator.
    char unused4[3];              // 42. Unused, ASCII(48).
    // 43. Unused, ASCII(48)
    // 44. Unused, ASCII(48).
    char comma13;                 // 45. ASCII(44) a comma as a data separator.
    char checksum[3];             // 46. High byte of Chksum.
    // 47. Middle byte of Chksum.
    // 48. Low byte of Chksum.
    char end;                     // 49. ASCII(13) carriage return. Denotes end of status page.
};

// Voltages on an Outback need to be scaled based on the battery voltage.
typedef enum {
    CONVERT_12VDC = 1,
    CONVERT_24VDC = 2,
    CONVERT_48VDC = 4
} voltage_conversion_t;


// Bit masks for warning messages
typedef enum {
    NO_WARNING      = 0,
    AC_IN_FREQ_HIGH = 1,
    AC_IN_FREQ_LOW  = 1 << 1,
    IN_VAC_HIGH     = 1 << 2,
    IN_VAC_LOW      = 1 << 3,
    BUY_IN_SIZE     = 1 << 4,
    TEMP_FAILED     = 1 << 5,
    COMM_ERROR      = 1 << 6,
    FAN_FAILURE     = 1 << 7
} warning_mode_t;

// Bit masks for error messages
typedef enum {
    NO_ERROR   = 0,
    LOW_VAC    = 1,
    STACKING   = 1 << 1,
    OVER_TEMP  = 1 << 2,
    LOW_BATT   = 1 << 3,
    PHASE_LOSS = 1 << 4,
    HIGH_BATT  = 1 << 5,
    SHORT      = 1 << 6,
    BACKFEED   = 1 << 7
} error_mode_t;

// "Misc" field bit masks
typedef enum {
    AC_UNIT    = 1,
    RESERVE1   = 1 << 1,
    RESERVE2   = 1 << 2,
    RESERVE3   = 1 << 3,
    RESERVE4   = 1 << 4,
    RESERVE5   = 1 << 5,
    RESERVE6   = 1 << 6,
    AUX_OUT_ON = 1 << 7
} misc_byte_t;

// These are the numeric equivalents for the ASCII representation
typedef enum {
    INV_OFF     = 0,
    INV_SEARCH  = 1,
    INV_ON      = 2,
    CHARGE      = 3,
    SILENT      = 4,
    FLOAT       = 5,
    EQ          = 6,
    CHARGER_OFF = 7,
    SUPPORT     = 8,
    SELL_ON     = 9,
    PASS_THRU   = 10,
    FX_ERR      = 90,
    AGS_ERR     = 91,
    COM_ERR     = 92
} op_mode_t;

typedef enum {
    DISABLED    = 0,
    DIVERSION   = 1,
    REMOTE      = 2,
    MANUAL      = 3,
    VENT_FAN    = 4,
    PV_TRIGGER  = 5
} aux_mode_t;

typedef enum {
    SILENT_MODE = 0,
    FLOAT_MODE  = 1,
    BULK_MODE   = 2,
    ABSORB_MODE = 3,
    EQ_MODE     = 4
} charge_mode_t;

typedef enum {
    NO_AC = 0,
    AC_DROP = 1,
    AC_USE = 2  
} AC_mode_t ;

// When sending the command, each of these bytes needs to be sent twice
typedef enum {
    ON          = 'O',
    SEARCH      = 'S',
    OFF         = 'F',
    USE         = 'U',
    DROP        = 'D',
    AUX_ON      = 'Z',
    AUX_OFF     = 'X'
} fx_commands_t;

typedef enum {
    OUTBACK_NONE,
    OUTBACK_MX,
    OUTBACK_FX
} outback_type_t;

class outback : public Serial
{
public:
    outback();
    outback(const char *filespec);
    outback(std::string filespec);
    ~outback();
    // Set up the object with data from the packet.
    retcode_t parse(const char *packet);

    // Initialize the serial port
    retcode_t commInit(std::string);
    retcode_t commInit(int fd);

    // The main loop for an outback device
    retcode_t main();
    retcode_t main(Console &con);
    retcode_t main(Console &con, Database &db);
  
    // calculate the ASCII packet
    retcode_t calcChecksum(const char *packet);  

    // Get the type of device
    outback_type_t typeGet();
  
    // Get the address of the unit.
    char addressGet() { return _address; }
  
    // Get the current readings
    int inverterCurrentGet()        { return _inverter_current; };
    int chargerCurrentGet()         { return _charge_current; };
    int buyCurrentGet()             { return _buy_current; };
    int pvCurrentGet()              { return _pv_current; };
    int sellCurrentGet()            { return _sell_current; };
  
    // Get the voltage readings
    int pvInputVoltageGet()         { return _pv_input_voltage; };
    int ACInputVoltageGet()         { return _AC_input_voltage; };
    int ACOutputVoltageGet()        { return _AC_output_voltage; };
    int batteryVoltageGet()         { return _battery_voltage; };

    // Get the modes
    op_mode_t operatingModeGet()    { return _operating_mode; };
    error_mode_t errorModeGet()     { return _error_mode; };
    warning_mode_t warningModeGet() { return _warning_mode; };
    AC_mode_t ACModeGet()           { return  _AC_mode; };
    misc_byte_t miscByteGet()       { return _misc_byte; };
    aux_mode_t auxModeGet()         { return _aux_mode; };
    charge_mode_t chargeModeGet()   { return _charge_mode; };

    // Read from the serial port
    const char *readSerial();

    // Write an FX command to the serial port
    retcode_t sendFXCommand(fx_commands_t cmd);

    void dump();

    meter_data_t *exportMeterData(meter_data_t *data);
  
private:
    outback_type_t _type;
    // The address of the unit.
    char _address;

    // Amperage readings. These range from 0-99 in 1 amp increments
    int _inverter_current;
    int _charge_current;
    int _buy_current;
    int _pv_current;
    int _sell_current;

    // Voltage readings. These range from 0-256. If the Misc Mode bit is
    // set, then this number is multiplied by 2.
    int _AC_input_voltage;
    int _AC_output_voltage;
    // one volt increments, ignore the misc Byte
    int _pv_input_voltage;
    // the resolution is 0.1 volts for a 12VDC system, 0.2 volts for a
    // 24VDC system, and 0.4 for a 48 volt system.
    int _battery_voltage;
    // The total kilowatts used daily. This ranges from 0-999, with the
    // last digit being the fraction. ie... 99 = 99.9.
    int _daily_kwh;

    // Modes. Not all modes are currently implemented on the MX
    // series. All the modes range from 0-99 except for the error and
    // warning modes which range from 0-256.
    op_mode_t _operating_mode;
    AC_mode_t _AC_mode;
    misc_byte_t _misc_byte;
    aux_mode_t _aux_mode;
    charge_mode_t _charge_mode;

    error_mode_t _error_mode;
    warning_mode_t _warning_mode;
    int _checksum;
};

// __OUTBACK_POWER_H__
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
