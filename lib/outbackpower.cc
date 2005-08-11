// 
//   Copyright (C) 2005 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <map>

#include "err.h"
#include "log.h"
#include "console.h"
#include "outbackpower.h"
#include "database.h"

using namespace std;
using namespace outbackpower;

const int PACKET_SIZE = 49;

outback::outback() : _type(OUTBACK_NONE), _address('x'),_inverter_current(0), _charge_current(0), _buy_current(0), _pv_current(0), _sell_current(0), _AC_input_voltage(0), _AC_output_voltage(0), _pv_input_voltage(0), _battery_voltage(0), _daily_kwh(0), _operating_mode(SUPPORT), _AC_mode(NO_AC), _misc_byte(RESERVE1), _aux_mode(DISABLED),_charge_mode(SILENT_MODE), _error_mode(NO_ERROR), _warning_mode(NO_WARNING), _checksum(0)
{
  DEBUGLOG_REPORT_FUNCTION;  
}

outback::outback(string filespec) :  _type(OUTBACK_NONE), _address('x'),_inverter_current(0),_charge_current(0), _buy_current(0), _pv_current(0), _sell_current(0), _AC_input_voltage(0), _AC_output_voltage(0), _pv_input_voltage(0), _battery_voltage(0), _daily_kwh(0), _operating_mode(SUPPORT), _AC_mode(NO_AC), _misc_byte(RESERVE1), _aux_mode(DISABLED), _charge_mode(SILENT_MODE), _error_mode(NO_ERROR), _warning_mode(NO_WARNING), _checksum(0)

{
  DEBUGLOG_REPORT_FUNCTION;
  commInit(filespec);
}

outback::outback(const char *filespec) : _type(OUTBACK_NONE), _address('x'),_inverter_current(0),_charge_current(0), _buy_current(0), _pv_current(0), _sell_current(0), _AC_input_voltage(0), _AC_output_voltage(0), _pv_input_voltage(0), _battery_voltage(0), _daily_kwh(0), _operating_mode(SUPPORT), _AC_mode(NO_AC), _misc_byte(RESERVE1), _aux_mode(DISABLED), _charge_mode(SILENT_MODE), _error_mode(NO_ERROR), _warning_mode(NO_WARNING), _checksum(0)

{
  DEBUGLOG_REPORT_FUNCTION;
  commInit(filespec);
}

outback::~outback()
{
}

// Read from the serial port
const char *
outback::readSerial()
{
  // DEBUGLOG_REPORT_FUNCTION;

  int ret;
  char *buf = new char[PACKET_SIZE+1];
  
  memset(buf, 0, PACKET_SIZE+1);

  ret = Read(buf, PACKET_SIZE);
  
  if (ret > 0) {
    //dbglogfile << "FIXME3: Read " << ret << " bytes: " << buf << "\t" << buf << endl;
    return buf;
  }

  //  calcChecksum(packet);

  return "";
}

retcode_t
outback::commInit(string filespec)
{
  DEBUGLOG_REPORT_FUNCTION;

  if (filespec.size() == 0) {
    dbglogfile << "ERROR: no serial device specified!" << endl;
    exit(0);
  }    

#if 0
    Open(filespec);
#else
  try {
    Open(filespec);
  }
  catch (ErrCond catch_err) {
    dbglogfile << catch_err << endl;
    exit(1);
  }
#endif
  
  return commInit(GetFD());
}

retcode_t
outback::commInit(int fd)
{
  DEBUGLOG_REPORT_FUNCTION;

  SetRaw();
  SetBaud(B19200);
  
#if 0
  termios thistty;
  tcgetattr(GetFD(), &thistty);
  thistty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
  thistty.c_oflag &= ~(OPOST|~ONLCR);
  thistty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  thistty.c_cflag &= ~(CSIZE|PARENB|HUPCL);
  thistty.c_cflag |= CREAD;
  //thistty.c_cflag |= CS8;
  thistty.c_cc[VMIN] = 1;
  thistty.c_cc[VTIME] = 100;
  tcsetattr(GetFD(), TCSANOW, &thistty);
#endif
  SetDTR(true);

  return SUCCESS;
}

// The main loop for an outback device
retcode_t
outback::main()
{
  DEBUGLOG_REPORT_FUNCTION;
  Console con;
  Database db;
  return main(con, db);
}

retcode_t
outback::main(Console &con, Database &db)
{
  DEBUGLOG_REPORT_FUNCTION;
  
  string str;
  int ch;
  
  while ((ch = con.Getc()) != 'q') {
    if (ch > 0){                // If we have something, process it
      //con.Putc (ch);          // echo inputted character to screen
      
      switch (ch) {
        // Toggle the DTR state, which is as close as we get to
        // flow control.
      case 'd':
        SetDTR(false);
        break;
      case 'D':
        SetDTR(true);
        break;
      case 'Q':
      case 'q':
        return SUCCESS;
        break;
      case '?':
        con.Puts("PowerGuru - Outback Mode\r\n");
        con.Puts("\t? - help\r\n");
        con.Puts("\tq - Quit\r\n");
        con.Puts("\tQ - Quit\r\n");
        sleep(2);
      default:
        break;
      };
    }
    
    str = readSerial();
    if (str.size() > 0) {
      // con.Puts(str); con.Puts("\r\n");
      calcChecksum(str.c_str());
      {
        parse(str.c_str());
        meter_data_t md;
        exportMeterData(&md);
        db.queryInsert(&md);
      }
      str.erase();
    }
  }
  
  return SUCCESS;
}

// Send a command to the master FX inverter. Each command is a single
// character, but is sent twice for error handling.
retcode_t
outback::sendFXCommand(fx_commands_t cmd) 
{
  char buf[3];
  memset(buf,0, 3);
  buf[0] = buf[1] = cmd;

  if (Write(buf, 2) != 2) {
    return ERROR;
  }

  return SUCCESS;
}

// Parse the ASCII packet into numerical data.
//
// A packet from an Outback Power systems device is a 49 bytes ASCII
// string representing numerical data. It is framed by a LF character
// (0x0a) to start, and a CR (0x0d) to end it. Each field is separated
// by commas, but we can ignore those. The checksum is a simply the
// addition of all the characters in the message.
retcode_t outback::parse(const char *data)
{
  DEBUGLOG_REPORT_FUNCTION;

  string packet = data;
  
  // The address is byte 1
  _address = packet[1];
  if ((_address >= '0') && (_address <= '9')) {
    _type = OUTBACK_FX;
  } else if ((_address >= 'A') && (_address <= 'K')) {
    _type = OUTBACK_MX;
  } else {
    dbglogfile << "ERROR: bad address in packet!" << endl;
    return ERROR;
  }

  _type = OUTBACK_MX;           // FIXME: 
  
  // The inverter current is bytes 4 & 4 on an FX, but unused on the MX.
  _inverter_current = strtol(packet.substr(3,2).c_str(), NULL, 0);
  
  // The charger current is bytes 7 & 8.
  _charge_current = strtol(packet.substr(6,2).c_str(), NULL, 0);

  // Bytes 9 & 10 are the Buy current on an FX, and the PV current on
  // an MX.
  if (_type == OUTBACK_FX) {
    _buy_current = strtol(packet.substr(9,2).c_str(), NULL, 0);
  } else {
    _pv_current = strtol(packet.substr(9,2).c_str(), NULL, 0);;
  }   
  
  // Bytes 13, 14, & 15 are the AC Input Voltage on an FX, and the PV
  // input voltage on an MX.
  if (_type == OUTBACK_FX) {
    _AC_input_voltage = strtol(packet.substr(13,3).c_str(), NULL, 0);
  } else {
    _pv_input_voltage = strtol(packet.substr(13,3).c_str(), NULL, 0);
  }

  // Bytes 16, 17, & 18 are the AC Output Voltage on an FX, and the
  // Daily Kilowatts on an MX.
  if (_type == OUTBACK_FX) {
    _AC_output_voltage = strtol(packet.substr(16,3).c_str(), NULL, 0);
  } else {
    _daily_kwh = strtol(packet.substr(16,3).c_str(), NULL, 0);
  }

  // Bytes 20 & 21 are the Sell current on an FX, but unused on an MX.
  if (_type == OUTBACK_FX) {
    _sell_current = strtol(packet.substr(20,2).c_str(), NULL, 0);
  }

  // Bytes 23 & 24 are the Operating Mode on an FX, but is the AUX Mode
  // on an MX.
  if (_type == OUTBACK_FX) {
    _operating_mode = static_cast<op_mode_t>(strtol(packet.substr(23,2).c_str(), NULL, 0));
  } else {
    _aux_mode = static_cast<aux_mode_t>(strtol(packet.substr(23,2).c_str(), NULL, 0));
  }
  
  // Bytes 26, 27, & 28 are the Error Mode, but is unimplemented on
  // the MX at this time. (2005/7/12)
  if (_type == OUTBACK_FX) {
    _error_mode = static_cast<error_mode_t>(strtol(packet.substr(13,3).c_str(), NULL, 0));
  }

  // Bytes 30 & 31 are the AC Mode on an FX, but is the Charger Mode
  // on an MX.
  if (_type == OUTBACK_FX) {
    _AC_mode = static_cast<AC_mode_t>(strtol(packet.substr(30,2).c_str(), NULL, 0));
  } else {
    _charge_mode = static_cast<charge_mode_t>(strtol(packet.substr(30,2).c_str(), NULL, 0));
  }
  
  // Bytes 33, 34, & 35 are the battery voltage.
  _battery_voltage = strtol(packet.substr(33,3).c_str(), NULL, 0);

  // Bytes 37, 38, & 39 are the Misc Byte on an FX, but are unused on
  // an MX.
  if (_type == OUTBACK_FX) {
    _misc_byte = static_cast<misc_byte_t>(strtol(packet.substr(37,3).c_str(), NULL, 0));
  }

  // Bytes 41, 42, & 43 are the Warning Mode on an FX, but unused on an MX.
  if (_type == OUTBACK_FX) {
    _warning_mode = static_cast<warning_mode_t>(strtol(packet.substr(13,3).c_str(), NULL, 0));
  }

  // Bytes 45, 46, & 47 are the packet checksum.
  _checksum = strtol(packet.substr(45,3).c_str(), NULL, 0);

  dump();
  
  return SUCCESS;
}

//
void
outback::dump()
{
  switch (_type) {
  case OUTBACK_MX:
    dbglogfile << "Outback Power Systems MX Charge Controller" << endl;
    break;
  case OUTBACK_FX:
    dbglogfile << "Outback Power Systems FX inverter" << endl;
    break;
  case OUTBACK_NONE:
  default:
    dbglogfile << "Outback Power Systems, no type specified!" << endl;
    break;
  };
  
  // The address of the unit.
  string addr;
  addr = "Unit #";
  if ((_address >= '0') && (_address <= '9')) {
    addr += _address;
  } else if ((_address >= 'A') && (_address <= 'K')) {
    addr += _address;
  }
  dbglogfile << "The address is: " << addr.c_str() << endl;

  // Amperage readings. These range from 0-99 in 1 amp increments
  if (_inverter_current) {
    dbglogfile << "Inverter current is: " << _inverter_current << endl;
  }

  if (_charge_current) {
    dbglogfile << "Charge current is: " << _charge_current << " Amps" << endl;
  }
  if (_buy_current) {
    dbglogfile << "Buy current is: " <<  _buy_current << " Amps" << endl;
  }
  if (_pv_current) {
    dbglogfile << "PV current is: " <<  _pv_current << " Amps" << endl;
  }
  if (_sell_current) {
    dbglogfile << "Sell current is: " <<  _sell_current << " Amps" << endl;
  }

  // Voltage readings. These range from 0-256. If the Misc Mode bit is
  // set, then this number is multiplied by 2.
  if (_AC_input_voltage) {
    dbglogfile << "AC Input voltage is: " << _AC_input_voltage << " VAC" << endl;
  }
  if (_AC_output_voltage) {
    dbglogfile << "AC Output voltage is: " << _AC_output_voltage << " VAC" << endl;
  }
  // one volt increments, ignore the misc Byte. FIXME: Do we devide this by 2 ?
  if (_pv_input_voltage) {
    dbglogfile << "PV Input voltage is: " << _pv_input_voltage/2 << " VDC" << endl;
  }
  
  // the resolution is 0.1 volts for a 12VDC system, 0.2 volts for a
  // 24VDC system, and 0.4 for a 48 volt system.
  if (_battery_voltage) {
    dbglogfile << "Battery voltage is: " << _battery_voltage/10
               << "." << _battery_voltage%10 << " VDC" << endl;
  }
  // The total kilowatts used daily. This ranges from 0-999, with the
  // last digit being the fraction. ie... 99 = 99.9.
  if (_daily_kwh) {
    dbglogfile << "The daily Kilowatts used is: " << _daily_kwh/10
               << "." << _daily_kwh%10 << " KW" << endl;
  }

  // Modes. Not all modes are currently implemented on the MX
  // series. All the modes range from 0-99 except for the error and
  // warning modes which range from 0-256.
  dbglogfile << "The operating mode is: ";
  switch (_operating_mode){
  case INV_OFF:
    dbglogfile << "Inverter OFF" << endl;
    break;
  case INV_SEARCH:
    dbglogfile << "Inverter SEARCH" << endl;
    break;
  case INV_ON:
    dbglogfile << "Inverter ON" << endl;    
    break;
  case CHARGE:
    dbglogfile << "Charge" << endl;
    break;
  case SILENT:
    dbglogfile << "Silent" << endl;
    break;
  case FLOAT:
    dbglogfile << "Float" << endl;
    break;
  case EQ:
    dbglogfile << "Equalize" << endl;
    break;
  case CHARGER_OFF:
    dbglogfile << "Charger Off" << endl;
    break;
  case SUPPORT:
    dbglogfile << "Support" << endl;
    break;
  case SELL_ON:
    dbglogfile << "Sell On" << endl;
    break;
  case PASS_THRU:
    dbglogfile << "Pass Thru" << endl;
    break;
  case FX_ERR:
    dbglogfile << "FX Error" << endl;
    break;
  case AGS_ERR:
    dbglogfile << "AGS Error" << endl;
    break;
  case COM_ERR:
    dbglogfile << "Communications Error" << endl;
    break;
  default:
    dbglogfile << "No operating mode!" << endl;
    break;
  };
  
  dbglogfile << "The AC mode is: \t";
  switch (_AC_mode) {
  case NO_AC:
    dbglogfile << "No AC" << endl;
    break;
  case AC_DROP:
    dbglogfile << "AC Dropped" << endl;
    break;
  case AC_USE:
    dbglogfile << "AC In Use" << endl;    
    break;
  default:
    dbglogfile << "No AC mode!" << endl;
    break;
  };

  dbglogfile << "The Misc Byte is: \t";
  switch (_misc_byte) {
  case 0:
    dbglogfile << "No Misc byte" << endl;
  case AC_UNIT:
    dbglogfile << "AC Unit" << endl;
    break;
  case AUX_OUT_ON:
    dbglogfile << "Aux Mode On set" << endl;
    break;
  default:
    dbglogfile << "Reserved" << endl;
    break;
  };

  dbglogfile << "The Aux mode is: \t";
  switch (_aux_mode) {
  case DISABLED:
    dbglogfile << "Aux Mode Disabled" << endl;
    break;
  case DIVERSION:
    dbglogfile << "Diversion" << endl;
    break;
  case REMOTE:
    dbglogfile << "Remote" << endl;
    break;
  case MANUAL:
    dbglogfile << "Manual" << endl;
    break;
  case VENT_FAN:
    dbglogfile << "Vent Fan" << endl;
    break;
  case PV_TRIGGER:
    dbglogfile << "PV Trigger" << endl;
    break;
  default:
    dbglogfile << "No AUX mode set" << endl;
    break;
  };

  dbglogfile << "The charge mode is: \t";
  switch (_charge_mode) {
  case SILENT_MODE:
    dbglogfile << "Silent Charge mode" << endl;
    break;
  case FLOAT_MODE:
    dbglogfile << "Float Charge mode" << endl;
    break;
  case BULK_MODE:
    dbglogfile << "Bulk Charge mode" << endl;
    break;
  case ABSORB_MODE:
    dbglogfile << "Absorb Charge mode" << endl;
    break;
  case EQ_MODE:
    dbglogfile << "Equalization Charge mode" << endl;
    break;
  default:
    dbglogfile << "No Charge mode" << endl;
    break;
  };

  dbglogfile << "The error mode is: \t";
  switch (_error_mode) {
  case NO_ERROR:
    dbglogfile << "No error" << endl;
    break;
  case LOW_VAC:
    dbglogfile << "ERROR: Low Volts AC" << endl;
    break;
  case STACKING:
    dbglogfile << "ERROR: Stacking" << endl;
    break;
  case OVER_TEMP:
    dbglogfile << "ERROR: Over Temperature" << endl;
    break;
  case LOW_BATT:
    dbglogfile << "ERROR: Low Battery" << endl;
    break;
  case PHASE_LOSS:
    dbglogfile << "ERROR: Phase Loss" << endl;
    break;
  case HIGH_BATT:
    dbglogfile << "ERROR: High Battery" << endl;
    break;
  case SHORT:
    dbglogfile << "ERROR: Short" << endl;
    break;
  case BACKFEED:
    dbglogfile << "ERROR: Backfeed" << endl;
    break;
  default:
    dbglogfile << "No error mode" << endl;
    break;
  };

  dbglogfile << "The warning mode is: \t";
  switch (_warning_mode) {
  case NO_WARNING:
    dbglogfile << "No warning" << endl;
    break;
  case AC_IN_FREQ_HIGH:
    dbglogfile << "WARNING: Input Frequency High" << endl;
    break;
  case AC_IN_FREQ_LOW:
    dbglogfile << "WARNING: Input Frequency Low" << endl;
    break;
  case IN_VAC_HIGH:
    dbglogfile << "WARNING: Input Volts AC High" << endl;
    break;
  case IN_VAC_LOW:
    dbglogfile << "WARNING: Input Volts AC low" << endl;
    break;
  case BUY_IN_SIZE:
    dbglogfile << "WARNING: Buy in size" << endl;
    break;
  case TEMP_FAILED:
    dbglogfile << "WARNING: Temperature Failure" << endl;
    break;
  case COMM_ERROR:
    dbglogfile << "WARNING: Communications error" << endl;
    break;
  case FAN_FAILURE:
    dbglogfile << "WARNING: Fan Failure" << endl;
    break;
  default:
    dbglogfile << "No warning mode" << endl;
    break;
  };
}

// Calculate the checksum for a packet and see if it matches.
retcode_t
outback::calcChecksum(const char *packet)
{
  DEBUGLOG_REPORT_FUNCTION;

  const char *ptr = packet;
  int index = 45;
  int chk_field = 0;
  int checksum = 0;
  int elem = 0;

  // extract the checksum field from the packet. Also strip off any
  // leading zeros, as this makes strtol() thinks it's octal by
  // mistake.
  char chk[4];
  memset(chk,0, 4);
  while (index <= 47) {
    if (packet[index] != '0') {
      chk[elem++] = packet[index];
    }
    index++;
  }
  
  // Convert the checksum to a number
  chk_field = strtol(chk, NULL, 0);
  
  do {
    if (*ptr == ',') {
      ptr++;
      continue;
    }
    if (*ptr == '*') {
      ptr++;
      continue;
    }
    checksum+= (*ptr++) - '0';
  } while ((ptr - packet) <= 44); // Don't add the checksum itself

  //dbglogfile << "Transmitted checksum for packet is: " << chk << endl;
  //dbglogfile << "Calculated checksum for packet is: " << checksum << endl;

  // See if the calculated checksum matches the checksum from the
  // packet checksum field. (the last 3 bytes before the CR terminator)
  if (checksum != chk_field) {
    dbglogfile << "ERROR: Checksums don't match!" << endl;
    return ERROR;
  }
  
  return SUCCESS;
}


//CREATE TABLE `meters` (
//  `unit` int(11) NOT NULL default '0',
//  `timestamp` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
//  `charge_amps` int(11) NOT NULL default '0',
//  `ac_load_amps` int(11) NOT NULL default '0',
//  `battery_volts` float NOT NULL default '0',
//  `ac_volts_out` float NOT NULL default '0',
//  `ac1_volts_in` float NOT NULL default '0',
//  `ac2_volts_in` float NOT NULL default '0',
//  `buy_amps` int(11) NOT NULL default '0',
//  `sell_amps` int(11) NOT NULL default '0',
//  `daily_kwh` float NOT NULL default '0',
//  `hertz` int(11) NOT NULL default '0',
//  `battery_tempcomp` float NOT NULL default '0'
//) ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='Stores data from data logging output';
meter_data_t *
outback::exportMeterData(meter_data_t *data)
{
  DEBUGLOG_REPORT_FUNCTION;

  // The unit number for the device
  data->unit = _address;
  
  // The type of the device
  switch (_type) {
  case OUTBACK_MX:
    data->type = MX_OUTBACK;
    data->unit = _address - '0';
    break;
  case OUTBACK_FX:
    data->type = FX_OUTBACK;
    data->unit = _address - 'A';
    break;
  case SW_XANTREX:
  default:
    break;
  };
  
  // The amperage being put into the batteries
  data->charge_amps = _charge_current;
  // The load in amps
  data->ac_load_amps = _inverter_current;
  // Actual Battery Voltage
  data->battery_volts = _battery_voltage/10.0;
  // Battery Voltage temperature compensated. This isn't used by
  // Outback products.
  data->tempcomp_volts = 0;
  // The voltage the inverter is producing
  data->ac_volts_out = _AC_output_voltage;
  // Grid AC input
  data->ac1_volts_in = _AC_input_voltage;
  // The AC current taken from the Grid and used to charge the batteries.
  data->buy_amps = _buy_current;
  // The AC current the batteries are putting into the grid.
  data->sell_amps = _sell_current;
  // The daily kilowatts put into the batteries from the PV source.
  data->daily_kwh = _daily_kwh/10.0;
  // The frequency in hertz. This doesn't exist on an Outback, but does on
  // a Xantrex. It should always be 60 though, so we set it so other
  // software can handle this  the same way.
  data->hertz = 60;
  // The generator is AC2 on a Xantrex, but doesn't have a separete
  // input on an Outback.
  data->ac2_volts_in = 0.0;
  // The current coming in from the PV panels before MPPT
  data->pv_amps_in = _pv_current;
  // The Voltage coming in from the PV panels before MPPT
  data->pv_volts_in = _pv_input_voltage;

  
  return data;
}

