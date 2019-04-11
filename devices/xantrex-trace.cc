// 
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
//               2014, 2015, 2016, 2017, 2018, 2019
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

/// \copyright GNU Public License.
/// \file xantrex-trace.cc This class uses the remote control command
///                       shell in the inverter to simulate a human.

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstring>
#include <vector>
#include <map>

#include "log.h"
#include "console.h"
#include "xantrex-trace.h"
#include "database.h"

using namespace std;

static std::string menudisp;
static char buffy[100];

extern LogFile dbglogfile;

XantrexUI::XantrexUI(void)
{
    SetDefaultValues();  
}

XantrexUI::~XantrexUI(void)
{
    DEBUGLOG_REPORT_FUNCTION;
}

string &
XantrexUI::ReadSerial(void)
{
    int ret;
    //DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);

#if 0 
    int retries = 1;
  
    while (retries--) {
        if (ret = Read((char *)&buffy, 99) < 0) {
            //BOOST_LOG(lg) << "FIXME: " << retries << "\t" << buffy << endl;
            //ret = Read((char *)&buffy, 100);
            continue;
        } else {
            break;
        }
    }
#else
    ret = Read((char *)&buffy, 100);
#endif
  
    if (ret > 0) {
        //BOOST_LOG(lg) << "FIXME3: Read " << ret << " bytes: " << buffy << "\t" << buffy << endl;
        menudisp = buffy;
    } else {
        menudisp.erase();
    }
  
    return menudisp;
}

string
XantrexUI::GetLabel() {
    //DEBUGLOG_REPORT_FUNCTION;

    BOOST_LOG(lg) << "menuheading: " << menuheading << " menuitem:" << menuitem << endl;
  
    MenuItem ti = GetItem(menuheading, menuitem);
  
    return ti.GetLabel();
}

string &
XantrexUI::WriteSerial(const char *buf, int nbytes)
{
    int ret;
  
    //DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    ret = Write(buf, nbytes);
    //BOOST_LOG(lg) << "FIXME" << buffy << endl;
    ret = Read(buffy, nbytes);
  
    menudisp = buffy;
    return menudisp;
}

// This selects which inverter we want to control
string &
XantrexUI::SelectInverter(int x)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);

    Write((char *)&x, 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

// These methods all go with the Xantrex display buttons
string &
XantrexUI::MenuHeadingMinus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    //memset(buffy, 0, 100);
  
    Write("L", 1);
    //Read((char *)&buffy, 100);
    menudisp = ReadSerial();
    //BOOST_LOG(lg) << "FIXME" << menudisp << endl;

    //cerr << "FIXMEh-: \"" << menudisp  << "\"" << endl;
    //cerr << "FIXMEh-: \"" << GetLabel() << "\"" << endl;

    if (menuheading > 1) {
        menuheading--;
    }
    menuitem = 0;

    if (menudisp.find(GetLabel()) == string::npos) {
        BOOST_LOG(lg) << "Heading Minus didn't Match!!" << menuheading << endl;  
    } else {
        BOOST_LOG(lg) << "Heading Minus Matched!!" << endl;
    }


    //menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::MenuHeadingPlus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    //memset(buffy, 0, 100);
    MenuItem mi;
  
    Write("R", 1);
  
    //Read((char *)&buffy, 100);
    menudisp = ReadSerial();
    //mi = GetItem();

    //BOOST_LOG(lg) << menudisp << endl;

    menuheading++;
    menuitem = 0;

    //cerr << "FIXMEh+: \"" << menudisp  << "\"" << endl;
    //cerr << "FIXMEh+: \"" << GetLabel() << "\"" << endl;
  
    if (menudisp.find(GetLabel()) == string::npos) {
        BOOST_LOG(lg) << "Heading Plus didn't Match!!" << endl;  
    } else {
        BOOST_LOG(lg) << "Heading Plus Matched!!" << endl;
    }



    //menudisp = buffy;
    return menudisp;
}

// This decends through the sub menu items of a heading
string &
XantrexUI::MenuItemMinus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    int retries = 5;
    string label;
  
    //memset(buffy, 0, 100);
  
    //Write("D", 1);
    //Read((char *)&buffy, 100);
    //menudisp = ReadSerial();
    //BOOST_LOG(lg) << menudisp << endl;

    menuitem++;

    //cerr << "FIXME-I: \"" << menuheading << "," << menuitem << "\"" << endl;
    while (retries--) {
        Write("D", 1);
        menudisp = ReadSerial();
        //Flush();
        //cerr << "FIXMEi-: \"" << menudisp  << "\"" << endl;
        //cerr << "FIXMEi-: \"" << GetLabel() << "\"" << endl;

        label = GetLabel();
        if (label.size()) {
            if (menudisp.find(label) == string::npos) {
                BOOST_LOG(lg) << "Item Minus didn't Match!!" << endl;
            } else {
                BOOST_LOG(lg) << "Item Minus Matched!!" << endl;
                break;
            }
        }
    }

    //menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::MenuItemPlus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    int retries = 5;

    //memset(buffy, 0, 100);
  
    //Write("U", 1);
    //Read((char *)&buffy, 100);
    //menudisp = ReadSerial();
    //BOOST_LOG(lg) << menudisp << endl;
    //menudisp = buffy;
    if (menuitem > 1)
        menuitem--;

    //  if (strcmp(label.c_str(), buffy) == 0)
    //    BOOST_LOG(lg) << "AAAAHHHHHHH" << endl;
  
    while (retries--) {
        Write("U", 1);
        menudisp = ReadSerial();
        //Flush();

        //cerr << "FIXMEi+: \"" << menudisp  << "\"" << endl;
        //cerr << "FIXMEi+: \"" << GetLabel() << "\"" << endl;

        if (menudisp.find(GetLabel()) == string::npos) {
            BOOST_LOG(lg) << "Item Plus didn't Match!!" << endl;
        } else {
            BOOST_LOG(lg) << "Item Plus Matched!!" << endl;
            break;
        }
    }

    //  if (menudisp != label)
    //  BOOST_LOG(lg) << "FIXME: Wrong Item " << menuheading << ":" << menuitem << menudisp << "\t" << label << endl;
    
    return menudisp;
}

string &
XantrexUI::SetPointMinus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("-", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::SetPointPlus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("+", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::Inverter(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("|", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::Generator(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("G", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::SetupMenu(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("\003", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menuheading = 8;
    menuitem = 0;

    menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::LedStatus(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("?", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;  
}

string &
XantrexUI::Version(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("V", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

string &
XantrexUI::SetTerminalMode(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    memset(buffy, 0, 100);
  
    Write("T", 1);
    Read((char *)&buffy, 100);
    BOOST_LOG(lg) << buffy << endl;

    menudisp = buffy;
    return menudisp;
}

#if 0
// These methods are for switching menu modes
retcode_t
XantrexUI::GotoSetupMenu(void)
{
    DEBUGLOG_REPORT_FUNCTION;
}

retcode_t
XantrexUI::GotoUserMenu(void)
{
    DEBUGLOG_REPORT_FUNCTION;
}
string &
XantrexUI::UserMenu(void)
{
    DEBUGLOG_REPORT_FUNCTION;
}

#endif

// This method takes us to the beginning of the menu headers and items
// This corresponds then to being in the Inverter Mode header of the
// User menu.
retcode_t
XantrexUI::GotoMenuStart(void)
{
    DEBUGLOG_REPORT_FUNCTION;
    int i;
    for (i=0; i<22; i++) {
        // Don't use MenuheadingMinus, as it gets all confused
        Write("L", 1);
        menudisp = ReadSerial();
        if (menudisp.find("Inverter", 0) != string::npos) {
            break;
        }
    }
  
    menuheading = 1;
    menuitem = 0;
    BOOST_LOG(lg) << "At Start Of Menu" << endl;
  
    return SUCCESS;
}

// These methods manipulate values
retcode_t
XantrexUI::SetFloatValue(float )
{
    DEBUGLOG_REPORT_FUNCTION;
    cerr << "ERROR: unimplemented!" << endl;
    return ERROR;
}

float
XantrexUI::GetFloatValue(std::string &str)
{
    DEBUGLOG_REPORT_FUNCTION;

    return atof(GetValue(str).c_str());
}

retcode_t
XantrexUI::SetIntValue(int x)
{
    DEBUGLOG_REPORT_FUNCTION;

    cerr << "ERROR: unimplemented!" << endl;
    return ERROR;
}

int
XantrexUI::GetIntValue(std::string &str)
{
    DEBUGLOG_REPORT_FUNCTION;

    return atoi(GetValue(str).c_str());
}

bool
XantrexUI::GetBoolValue(std::string &str)
{
    DEBUGLOG_REPORT_FUNCTION;
  
    string tmpstr = GetValue(str).c_str();
  
    if (tmpstr == "NO" || tmpstr == "OFF")
        return false;
    else
        return true;
}

string &
XantrexUI::GetValue (std::string &str)
{
    DEBUGLOG_REPORT_FUNCTION;
    string::size_type pos, pos2;
    //string value;
    int i = 0;
    string lookfor;

    //cout << "The Raw value string is: " << str << endl;

#if 0
    pos = str.find("-00", str.length());
    if (pos != string::npos) {
        str.erase(pos, str.length() - pos);
    }
#endif
  
    do {
        lookfor = " ";
        char j = '0' + i;
        lookfor += j;
        pos = str.find(lookfor, 0);
        lookfor.erase();
    } while ((pos == string::npos) && (i++ < 10));
  
    pos2 = str.find(' ', pos+1);
    if (pos2 == string::npos) {
        pos2 = pos + 4;
    }
  
    str = str.substr(pos+1, pos2 - pos);

#if 0
    if (pos != string::npos) {
        // We want just the last data item, which is the value
        value = str.substr(pos+1, str.length() - pos);
        str = value;
    } else {
        str.erase();
    }
#endif
  
    return str;
}

vector<string> *
XantrexUI::GetValues (std::string &str)
{
    DEBUGLOG_REPORT_FUNCTION;
    string::size_type pos;
    string value;
    vector<string> *array;

    array = new vector<string>;

    //cout << "The Raw values string is: " << str << endl;

    do {
        pos = str.find("-00", str.length());
        if (pos != string::npos) {
            str.erase(pos, str.length() - pos);
        }
    
        // Find the last space in the string
        pos = str.rfind(' ' , str.length());
    
        if (pos != string::npos) {
            // We want just the last data item, which is the value
            value = str.substr(pos+1, str.length() - pos);
            str = value;
            array->push_back(value);
        } else {
            str.erase();
        }
    } while (pos != string::npos);
  
  
    return array;
}

std::string
XantrexUI::GetLabel(std::string &x)
{
    DEBUGLOG_REPORT_FUNCTION;
    string::size_type pos;

    // Find the last space in the string
    pos = x.rfind(' ' , x.length());
    //pos = x.rfind('\t' , x.length());

    if (pos != string::npos) {
        return x.substr(0, pos);
    }
  
    return x;
}


string &
XantrexUI::GotoMenuItem(int mh, int mi)
{
    DEBUGLOG_REPORT_FUNCTION;
    string tmpstr;
    int i;
    MenuItem item;
  
    GotoMenuStart();

    item = GetItem(mh, mi);
    for (i=0; i < mh-1; i++) {
        menudisp = MenuHeadingPlus();
    }
  
    for (i=0; i < mi-1; i++) {
        menudisp = MenuItemMinus();
    }

    if (menudisp.find(item.GetLabel()) != string::npos) {
        BOOST_LOG(lg) << "Didn't Match!!" << endl;  
    }

    //menudisp = CleanUpData(tmpstr);
  
    menuheading = mh;
    menuitem = mi;

    return menudisp;
}

//std::map<std::string, float> *
//std::vector< std::map< std::string, float > >
vector<meter_data_t *>
XantrexUI::PollMeters(int loops)
{
    MenuItem      ti;
    int           mh = 4;
    int           mi = 1;
    int           items = 7;                // the number of data items to get
    float         fltval;
    int           intval;
    int           i;
    string        str, label;
    meter_data_t *downdata, *updata;
    vector<meter_data_t *> meters;

    //memset(buffy, 0, 100);

    GotoMenuItem(mh, mi);
    while (loops-- > 0) {
        downdata = new meter_data_t;
        memset(downdata, 0, sizeof(meter_data_t));
    
        for (i=mi; i<= mi+items; i++) {
            str = MenuItemMinus();
            fltval = 0.0;
            intval = 0;
            if (str.size() > 1) {
                ti = GetItem();
                if (ti.GetType() != MenuItem::INFO) {
                    label = GetLabel(); 
                    cout << "The Menu Item Label down is: " << label;
                }
                ti = GetItem();
                if (ti.GetType() == MenuItem::FLOAT) {
                    fltval = GetFloatValue(str);
                    cout << ", with a float value is: " << fltval << endl;
                }
                if (ti.GetType() == MenuItem::INT) {
                    intval = GetIntValue(str);
                    cout << ", with an Integer value is: " << intval << endl;
                }
                if (ti.GetType() == MenuItem::INFO) {
                    cout << ", an INFO statement  " << endl;
                    break;
                }
                if (label.substr(0, 5) == "Input") {
                    //downdata->
                }
                if (label.substr(0, 4) == "Load") {
                    BOOST_LOG(lg) << "Matched label going down " << label;
                    BOOST_LOG(lg) << " value is: " << intval << endl;
                    downdata->ac_load_amps = intval;
                }
                if (label.substr(8, 6) == "actual") {
                    BOOST_LOG(lg) << "Matched label going down " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    downdata->battery_volts = fltval;
                }
                if (label.substr(8, 8) == "TempComp") {
                    BOOST_LOG(lg) << "Matched label going down " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    downdata->tempcomp_volts = fltval;
                }
                if (label.substr(0, 8) == "Inverter") {
                    BOOST_LOG(lg) << "Matched label going down " << label << endl;
                    BOOST_LOG(lg) << " value is: " << intval << endl;
                    downdata->ac_volts_out = intval;
                }
                if (label.substr(0, 4) == "Grid") {
                    BOOST_LOG(lg) << "Matched label going down " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    downdata->ac1_volts_in = fltval;
                }
                if (label.substr(0, 9) == "Generator") {
                    BOOST_LOG(lg) << "Matched label going down " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    downdata->ac2_volts_in = fltval;
                }
                if (label.substr(0, 9) == "Read Freq") {
                    BOOST_LOG(lg) << "Matched label going down " << label << endl;
                    BOOST_LOG(lg) << " value is: " << intval << endl;
                    downdata->hertz = intval;
                }
            }
        }

        meters.push_back(downdata);
        //MenuItemMinus();            // go one more, since the first thing
        // we do is come back up one item.
        updata = new meter_data_t;
        memset(updata, 0, sizeof(meter_data_t));
        //ptr2 = (float *)((char *)updata + sizeof(meter_data_t)-sizeof(float));
        //*ptr2-- = downdata->hertz;
    
        // Now go up through the menu
        for (i=mi+items; i>mi; i--) {
            str = MenuItemPlus();
            fltval = 0.0;
            intval = 0;
            if (str.size() > 0) {
                label = GetLabel(); 
                cout << "The Menu Item Label up is: " << label;
                ti = GetItem();
                if (ti.GetType() == MenuItem::FLOAT) {
                    fltval = GetFloatValue(str);
                    cout << ", with a float value is: " << fltval << endl;
                }
                if (ti.GetType() == MenuItem::INT) {
                    intval = GetIntValue(str);
                    cout << ", with an integer value is: " << intval << endl;
                }
                //Flush();
                //_data.push(label, );
                if (label.substr(0, 5) == "Input") {
                    //updata->
                }
                if (label.substr(0, 4) == "Load ") {
                    BOOST_LOG(lg) << "Matched label going up " << label << endl;
                    BOOST_LOG(lg) << " value is: " << intval << endl;
                    updata->ac_load_amps = intval;
                }
                if (label.substr(8, 6) == "actual") {
                    BOOST_LOG(lg) << "Matched label going up " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    updata->battery_volts = fltval;
                }
                if (label.substr(8, 8) == "TempComp") {
                    BOOST_LOG(lg) << "Matched label going up " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    updata->tempcomp_volts = fltval;
                }
                if (label.substr(0, 8) == "Inverter") {
                    BOOST_LOG(lg) << "Matched label going up " << label << endl;
                    BOOST_LOG(lg) << " value is: " << intval << endl;
                    updata->ac_volts_out = intval;
                }
                if (label.substr(0, 4) == "Grid") {
                    BOOST_LOG(lg) << "Matched label going up " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    updata->ac1_volts_in = fltval;
                }
                if (label.substr(0, 9) == "Generator") {
                    BOOST_LOG(lg) << "Matched label going up " << label << endl;
                    BOOST_LOG(lg) << " value is: " << fltval << endl;
                    updata->ac2_volts_in = fltval;
                }
                // Since this is the last and first reading, it doesn't really
                // have time to change.
                updata->hertz = downdata->hertz;
//         if (label.substr(0, 9) == "Read Freq") {
//           BOOST_LOG(lg) << "Matched label going up " << label << endl;
//           BOOST_LOG(lg) << " value is: " << intval << endl;
//           updata->hertz = intval;
//         }
            } // end of if str
        } // end of for loop
        meters.push_back(updata);
    }
    // Go back to the top
//   for (i=items; i>=mi; i--) {
//     MenuItemPlus();
//   }
  
    MenuHeadingPlus();
    MenuHeadingMinus();          
  
    return meters;
}

// This method takes the data as outputted by the Xantrex inverter,
// and munges it to be a standard form with whitespace compressed,
// newlines & carriage returns stripped out, etc... This is so
// we can analyse the string data in a consistant fashion.
std::string &
XantrexUI::CleanUpData(std::string &str)
{
    unsigned int i;

    menudisp.erase();
    for (i=0; i< str.length(); i++) {
        switch (str[i]) {
            // we ignore these characters
          case '\r':
          case '\n':
              break;
              // compress multiple spaces to be just one
          case ' ':
              if (str[i+1] == ' ') {
                  continue;
              } else {
                  menudisp += ' ';
                  break;
              }
          default:
              menudisp += str[i];
        }
    }

    return menudisp;
}

void
XantrexUI::SetDefaultValues(void)
{

    MenuItem mi;

    // We know we always have 20 major menu headings
    _items.resize(21);

    mi.SetItem ("Top of Menu", 0);
    _items[0].push_back(mi);

    // Inverter Mode - Menu Heading #1
    mi.SetItem ("Inverter Mode",                      1);
    _items[1].push_back(mi);

    mi.SetItem ("Set Inverter     OFF SRCH ON CHG",   1, 1, "SetVert"); // FIXME: enum
    _items[1].push_back(mi);
    mi.SetItem ("CHG avail. Only  in FLT mode",       1, 2);
    _items[1].push_back(mi);
    mi.SetItem ("Press red or     setpoint button",   1, 3);
    _items[1].push_back(mi);
    mi.SetItem ("to move.         Move cursor to",    1, 4);
    _items[1].push_back(mi);
    mi.SetItem ("Inverter OFF to  resetOverCurrent",  1, 5);
    _items[1].push_back(mi);

    // Generator Mode - Menu Headings #2
    mi.SetItem ("Generator Mode",                     2);
    _items[2].push_back(mi);
    mi.SetItem ("Set Generator    OFF AUTO ON EQ",    2, 1, "SetGen"); // FIXME: enum
    _items[2].push_back(mi);
    mi.SetItem ("Gen under/over   speed",             2, 2, false, "GenSpeed");
    _items[2].push_back(mi);  
    mi.SetItem ("Generator start  error",             2, 3, false, "GenStart");
    _items[2].push_back(mi);
    mi.SetItem ("Generator sync   error",             2, 4, false, "GenSync");
    _items[2].push_back(mi);
    mi.SetItem ("Gen max run Time error",             2, 5, false, "GenMax");
    _items[2].push_back(mi);
    mi.SetItem ("Load Amp Start   ready",             2, 6, false, "LoadAmp");
    _items[2].push_back(mi);
    mi.SetItem ("Voltage Start    ready",             2, 7, false, "VoltStart");
    _items[2].push_back(mi);
    mi.SetItem ("Exercise Start   ready",             2, 8, false, "ExerStart");
    _items[2].push_back(mi);
  
    // Xantrex Engineering - Menu Heading #3
    // Xantrex was Trace, so this prompt varies depending on the
    // software version on the inverter.
    mi.SetItem ("Engineering",       3);
    //mi.SetItem ("Trace            Engineering",       3);
    _items[3].push_back(mi);
    mi.SetItem ("Press reset now  for defaults",      3, 1);
    _items[3].push_back(mi);
    mi.SetItem ("Revision 4.01",                      3, 2, "XantrexRev");
    _items[3].push_back(mi);
    mi.SetItem ("5916 195th St NE Arlington, WA",     3, 3);
    _items[3].push_back(mi);
    mi.SetItem ("98223 USA",                          3, 4);
    _items[3].push_back(mi);
    mi.SetItem ("Ph  360-435-8826 Fax 360-435-2229",  3, 5);
    _items[3].push_back(mi);

    // Meters - Menu Heading #4
    mi.SetItem ("Meters",                         4);
    _items[4].push_back(mi);
    mi.SetItem ("Inverter/charger amps AC",         4,  1, 0, "ChargerAmps");
    _items[4].push_back(mi);
    mi.SetItem ("Input            amps AC",         4,  2, 0, "InputAmps");
    //mi.SetItem ("Input",                           4,  2, 0, "InputAmps");
    _items[4].push_back(mi);
    mi.SetItem ("Load             amps AC",         4,  3, 0, "LoadAmps");
    _items[4].push_back(mi);
    //mi.SetItem ("Battery actual",                   4,  4, (float)25.1, "BattVolts");
    mi.SetItem ("Battery actual   volts DC",        4,  4, (float)25.1, "BattVolts");
    _items[4].push_back(mi);
    //mi.SetItem ("Battery TempComp",                4,  5, (float)25.1, "TempComp");
    mi.SetItem ("Battery TempComp volts DC",        4,  5, (float)25.1, "TempComp");
    _items[4].push_back(mi);
    mi.SetItem ("Inverter         volts AC",        4,  6, 0, "InvertVolts");
    _items[4].push_back(mi);
    //mi.SetItem ("Grid (AC1)",                       4,  7, 0, "GridVolts");
    mi.SetItem ("Grid (AC1)       volts AC",                             4,  7, 0, "GridVolts");
    _items[4].push_back(mi);
    //mi.SetItem ("Generator (AC2)",                  4,  8, 0, "GenVolts");
    mi.SetItem ("Generator (AC2)  volts AC",                             4,  8, 0, "GenVolts");
    _items[4].push_back(mi);
    //mi.SetItem ("Read Frequency",                   4,  9, 0, "ReadHertz");
    mi.SetItem ("Read Frequency   Hertz",                           4,  9, 0, "ReadHertz");
    _items[4].push_back(mi);
    mi.SetItem ("AC1 & AC2 volts  valid only when", 4, 10);
    _items[4].push_back(mi);
    mi.SetItem ("inverter synced  to that input.",  4, 11);
    _items[4].push_back(mi);
    mi.SetItem ("Batt volt actual is used for",     4, 12);
    _items[4].push_back(mi);
    mi.SetItem ("LBCO,HBCO,LBX,   LBCI,sell volts", 4, 13);
    _items[4].push_back(mi);
    mi.SetItem ("and gen starting",                 4, 14);
    _items[4].push_back(mi);
    mi.SetItem ("Batt volt temp   comp is used,",   4, 15);
    _items[4].push_back(mi);
    mi.SetItem ("for float, bulk, eq & aux relays", 4, 16);
    _items[4].push_back(mi);
  
    // Error Cause - Menu Heading #5
    mi.SetItem ("Error Causes",                 5);
    _items[5].push_back(mi);
    mi.SetItem ("Over Current",                 5,  1, false, "OverCurrent");
    _items[5].push_back(mi);
    mi.SetItem ("Transformer      overtemp",    5,  2, false, "OverTemp");
    _items[5].push_back(mi);
    mi.SetItem ("Heatsink         overtemp",    5,  3, false, "HeatSink");
    _items[5].push_back(mi);
    mi.SetItem ("High Battery     voltage",     5,  4, false, "HighBatt");
    _items[5].push_back(mi);
    mi.SetItem ("Low Battery      voltage",     5,  5, false, "LowBatt");
    _items[5].push_back(mi);
    mi.SetItem ("Inverter breaker tripped",     5,  6, false, "Tripped");
    _items[5].push_back(mi);
    mi.SetItem ("Manual Off",                   5,  7, false, "ManualOff");
    _items[5].push_back(mi);
    mi.SetItem ("AC source wired  to output",   5,  8, false, "ACWired");
    _items[5].push_back(mi);
    mi.SetItem ("External error   (Stacked)",   5,  9, false, "ExtError");
    _items[5].push_back(mi);
    mi.SetItem ("Generator start  error",       5, 10, false, "GenError");
    _items[5].push_back(mi);
    mi.SetItem ("Generator sync   error",       5, 11, false, "SyncError");
    _items[5].push_back(mi);
    mi.SetItem ("Gen maximum run  time error",  5, 12, false, "MaxRunError");
    _items[5].push_back(mi);
    mi.SetItem ("Gen Under/Over   speed",       5, 13, false, "SpeedError");
    _items[5].push_back(mi);
    mi.SetItem ("Inverter breaker tripped",     5, 14, false, "BreakerError");
    _items[5].push_back(mi);
  
    // Time Of Day - Menu Heading #6
    mi.SetItem ("Time of Day",                 6);
    _items[6].push_back(mi);
    mi.SetItem ("Time of Day      Set Clock hour", 6, 1, 0);
    _items[6].push_back(mi);
    mi.SetItem ("Set Clock minute",            6, 2, 0, "Minute");
    _items[6].push_back(mi);
    mi.SetItem ("Set Clock second",            6, 3, 0, "Second");
    _items[6].push_back(mi);

    // Generator timer - Menu Heading #7
    mi.SetItem ("Generator Timer",           7);
    _items[7].push_back(mi);
    mi.SetItem ("Start Quiet      time h:m", 7, 1, 8 ); // FIXME: this should be a time
    _items[7].push_back(mi);  
    mi.SetItem ("End Quiet        time h:m", 7, 2, 8 ); // FIXME: this should be a time
    _items[7].push_back(mi);

    // End of User menus!
    mi.SetItem ("END USER MENU", 8);
    _items[8].push_back(mi);

    // Inverter Setup - Menu heading #9
    mi.SetItem ("Inverter Setup",               9);
    _items[9].push_back(mi);
    mi.SetItem ("Set Grid Usage",               9, 1); // FIXME: FLOAT
    _items[9].push_back(mi);
    mi.SetItem ("Set Low battery  cut out VDC", 9, 2, (float)22.0);
    _items[9].push_back(mi);
    mi.SetItem ("Set LBCO delay   minutes",     9, 3, 15);
    _items[9].push_back(mi);
    mi.SetItem ("Set Low battery  cut in VDC",  9, 4, (float)26.0);
    _items[9].push_back(mi);
    mi.SetItem ("Set High battery cut out VDC", 9, 5, (float)32.0);
    _items[9].push_back(mi);
    mi.SetItem ("Set search       watts",       9, 6, 48);
    _items[9].push_back(mi);
    mi.SetItem ("Set search       spacing",     9, 7, 59);
    _items[9].push_back(mi);

    // Battery Charging - Menu heading #10
    mi.SetItem ("Battery Charging",                10);
    _items[10].push_back(mi);
    mi.SetItem ("Set Bulk         volts DC",       10, 1, (float)28.8);
    _items[10].push_back(mi);
    mi.SetItem ("Set Absorption   time h:m",       10, 2, 2);
    _items[10].push_back(mi);
    mi.SetItem ("Set Float        volts DC",       10, 3, (float)26.8);
    _items[10].push_back(mi);
    mi.SetItem ("Set Equalize     volts DC",       10, 4, (float)28.8);
    _items[10].push_back(mi);
    mi.SetItem ("Set Equalize     time h:m",       10, 5, 2);
    _items[10].push_back(mi);
    mi.SetItem ("Set Max Charge   amps DC",        10, 6, 30);
    _items[10].push_back(mi);
    mi.SetItem ("Set Temp Comp    LeadAcid Nicad", 10, 7); // FIXME: LEADACID
    _items[10].push_back(mi);

    // AC Inputs - Menu heading #11
    mi.SetItem ("AC Inputs",                       11);
    _items[11].push_back(mi);
    mi.SetItem ("inverter synced  to that input.", 11);
    _items[11].push_back(mi);
    mi.SetItem ("Set Grid (AC1)   amps AC",        11, 1, 60);
    _items[11].push_back(mi);
    mi.SetItem ("Set Gen (AC2)    amps AC",        11, 2, 30);
    _items[11].push_back(mi);
    mi.SetItem ("Set Input lower  limit VAC",      11, 3, 108);
    _items[11].push_back(mi);
    mi.SetItem ("Set Input upper  limit VAC",      11, 4, 132);
    _items[11].push_back(mi);

    // Generator Auto Start Setup - Menu heading #12
    //  genauto.state = OFF;
    mi.SetItem ("Gen Auto Start   setup",           12);
    _items[12].push_back(mi);
    mi.SetItem ("Set Load Start   amps AC",         12, 1, 33);
    _items[12].push_back(mi);
    mi.SetItem ("Set Load Start   delay min",       12, 2, 5);
    _items[12].push_back(mi);
    mi.SetItem ("Set Load Stop    delay min",       12, 3, 5);
    _items[12].push_back(mi);
    mi.SetItem ("Set 24 hr start  volts DC",        12, 4, (float)24.6);
    _items[12].push_back(mi);
    mi.SetItem ("Set 2 hr start   volts DC",        12, 5, (float)23.6);
    _items[12].push_back(mi);
    mi.SetItem ("Set 15 min start volts DC",        12, 6, (float)22.6);
    _items[12].push_back(mi);
    mi.SetItem ("Read LBCO 30 sec start VDC",       12, 7, 22);
    _items[12].push_back(mi);
    mi.SetItem ("Set Exercise     period days",     12, 8, 30);
    _items[12].push_back(mi);
    mi.SetItem ("Set Maximum run  time h:m",        12, 9, 8);
    _items[12].push_back(mi);
    mi.SetItem ("Set Max Run time to 0 to defeat.", 12, 10, 0);
    _items[12].push_back(mi);
    mi.SetItem ("Set Exercise to 0 to defeat.",     12, 11, 0);
    _items[12].push_back(mi);
    mi.SetItem ("See menu 9 to    to set LBCO.",    12, 12, 0);
    _items[12].push_back(mi);

    //Generator Starting Details - Menu heading #13
    mi.SetItem ("Gen starting     details",      13);
    _items[13].push_back(mi);
    mi.SetItem ("Set RY7 Function GlowStop Run", 13, 1, 0);
    _items[13].push_back(mi);
    mi.SetItem ("Set Gen warmup   seconds",      13, 2, 60);
    _items[13].push_back(mi);
    mi.SetItem ("Set Pre Crank    seconds",      13, 3, 10);
    _items[13].push_back(mi);
    mi.SetItem ("Set Max Cranking seconds",      13, 4, 10);
    _items[13].push_back(mi);
    mi.SetItem ("Set Post Crank   seconds",      13, 5, 30);
    _items[13].push_back(mi);

    // Auxilary relays - Menu heading #14
    mi.SetItem ("Auxiliary Relays R9 R10 R11", 14);
    _items[14].push_back(mi);
    mi.SetItem ("Set Relay 9      volts DC",       14, 1, 29);
    _items[14].push_back(mi);
    mi.SetItem ("R9 Hysteresis    volts DC",       14, 2, 2);
    _items[14].push_back(mi);
    mi.SetItem ("Set Relay 10     volts DC",       14, 3, (float)29.5);
    _items[14].push_back(mi);
    mi.SetItem ("R10 Hysteresis   volts DC",       14, 4, 2);
    _items[14].push_back(mi);
    mi.SetItem ("Set Relay 11     volts DC",       14, 5, 30);
    _items[14].push_back(mi);
    mi.SetItem ("R11 Hysteresis   volts DC",       14, 6, 2);
    _items[14].push_back(mi);
    mi.SetItem ("Close on batt >  setpoint.",      14, 7);
    _items[14].push_back(mi);
    mi.SetItem ("Open  on  batt < setpoint - Hys", 14, 8);
    _items[14].push_back(mi);
    mi.SetItem ("Relays have 2    second delay on",14, 9);
    _items[14].push_back(mi);
    mi.SetItem ("Close, 0.1 sec   delay on open",  14, 10);
    _items[14].push_back(mi);

    // Bulk Charge Trigger Time - Menu heading #15
    mi.SetItem ("Bulk Charge      Trigger Timer",   15);
    _items[15].push_back(mi);              
    mi.SetItem ("Set Start Bulk   time",            15, 1, 0);
    _items[15].push_back(mi);              
    mi.SetItem ("To disable timer set to 00:00",    15, 2);
    _items[15].push_back(mi);              
    mi.SetItem ("If grid timer    active set bulk", 15, 3);
    _items[15].push_back(mi);              
    mi.SetItem ("time after start charge time.",    15, 4);
    _items[15].push_back(mi);              
    mi.SetItem ("In SLT mode don't disable this",   15, 5);
    _items[15].push_back(mi);              
    mi.SetItem ("timer. It is the daily chg time.", 15, 6);
    _items[15].push_back(mi);              

    // Low Battery Transfer - Menu heading #16
    mi.SetItem ("Low Battery      Transfer (LBX)",    16);
    _items[16].push_back(mi);
    mi.SetItem ("Set Low Battery  TransferVDC",       16, 1, (float)26.8);
    _items[16].push_back(mi);
    mi.SetItem ("Set Low Battery  cut in  VDC",       16, 2, (float)26);
    _items[16].push_back(mi);
    mi.SetItem ("See menu 9 to    enable LBX mode.",  16, 3);
    _items[16].push_back(mi);
    mi.SetItem ("Make sure LBX is above LBCO volts.", 16, 4);
    _items[16].push_back(mi);

    //  Battery Power Selling - Menu heading #17
    mi.SetItem ("Battery Selling",                    17);
    _items[17].push_back(mi);
    mi.SetItem ("Set Battery Sell volts",             17, 1, (float)26.8);
    _items[17].push_back(mi);
    mi.SetItem ("Set Max Sell     amps",              17, 2, 30);
    _items[17].push_back(mi);
    mi.SetItem ("See menu 9 to    enable SELL mode.", 17, 3);
    _items[17].push_back(mi);
    mi.SetItem ("Make sure LBX is above LBCO volts.", 17, 4);
    _items[17].push_back(mi);
  
    // Grid Usage Timer - Menu heading #18
    mi.SetItem ("Grid Usage Timer",                 18);
    _items[18].push_back(mi);
    mi.SetItem ("Set Charge       time",            18,  1, 21); // FIXME: TIME
    _items[18].push_back(mi);
    mi.SetItem ("End Charge       time",            18,  2, 21); // FIXME: TIME
    _items[18].push_back(mi);
    mi.SetItem ("After Start      Charge time:",    18,  3);
    _items[18].push_back(mi);
    mi.SetItem ("SELL mode        charges battery.",18,  4);
    _items[18].push_back(mi);
    mi.SetItem ("FLT mode         charges battery", 18,  5);
    _items[18].push_back(mi);
    mi.SetItem ("After End Charge time:",           18,  6);
    _items[18].push_back(mi);
    mi.SetItem ("SELL mode sells  battery to AC1.", 18,  7);
    _items[18].push_back(mi);
    mi.SetItem ("FLT mode drops   AC1 and inverts", 18,  8);
    _items[18].push_back(mi);
    mi.SetItem ("Timer on when    start < > end;",  18,  9);
    _items[18].push_back(mi);
    mi.SetItem ("timer off when   start = end",     18, 10);
    _items[18].push_back(mi);
    mi.SetItem ("Sell and float   modes use timer", 18, 11);
    _items[18].push_back(mi);
    mi.SetItem ("SLT and LBX mode ignore timer",    18, 12);
    _items[18].push_back(mi);
  
    // Information file battery - Menu Heading #19
    mi.SetItem ("Information file battery",          19);
    _items[19].push_back(mi);
    mi.SetItem ("Batt temp comp   changes battery",  19, 1);
    _items[19].push_back(mi);
    mi.SetItem ("voltage reading  away from actual", 19, 2);
    _items[19].push_back(mi);
    mi.SetItem ("HBCO resets at:  6v/48, 3v/24 and", 19, 3);
    _items[19].push_back(mi);
    mi.SetItem ("1.5v/12v under   HBCO.",            19, 4);
    _items[19].push_back(mi);
    mi.SetItem ("LowBattTransfer  used in LBX, FLT", 19, 5);
    _items[19].push_back(mi);
    mi.SetItem ("Modes only. Goes back to battery",  19, 6);
    _items[19].push_back(mi);
    mi.SetItem ("at LowBattCutIn  (aka LBCI).",      19, 7);
    _items[19].push_back(mi);
    mi.SetItem ("For LBX mode set below LBCI so",    19, 8);
    _items[19].push_back(mi);
    mi.SetItem ("charger won't    cycle batteries",  19, 9);
    _items[19].push_back(mi);
    mi.SetItem ("up and down and  set LBCO below.",  19, 10);
    _items[19].push_back(mi);
  
    // End of Setup - Menu Heading #20
    mi.SetItem ("END SETUP MENU", 20);
    _items[20].push_back(mi);
}

// Dump all the MenuItem data
void
XantrexUI::Dump (void)
{
    DEBUGLOG_REPORT_FUNCTION;

    vector< vector< MenuItem > >::iterator mh;
    vector< MenuItem >::iterator it;

    BOOST_LOG(lg) << "There are " << (int)_items.size() << " menu _items" << endl;

    for (mh = _items.begin(); mh != _items.end(); ++mh) {
        for (it = mh->begin(); it != mh->end(); ++it) {
            //it->Dump();
            cout << it->GetHeaderIndex()
                 << "," << it->GetItemIndex()
                 << ": " << it->GetLabel() << endl;
        }
    }
}

void
XantrexUI::DumpAliases (void)
{
    DEBUGLOG_REPORT_FUNCTION;
  
    vector< vector< MenuItem > >::iterator mh;
    vector< MenuItem >::iterator it;
    string tmpstr;

    cout << "Command Name\t\tMenu Item" << endl;
    cout << "------------\t\t---------" << endl;
    for (mh = _items.begin(); mh != _items.end(); ++mh) {
        for (it = mh->begin(); it != mh->end(); ++it) {
            tmpstr = it->GetLabel();
            if (it->GetAlias().size() > 0)
                cout << it->GetAlias() << "  \t- " << CleanUpData(tmpstr) << endl;
        }
    }
}

MenuItem &
XantrexUI::Match(string &str)
{
    DEBUGLOG_REPORT_FUNCTION;
  
    vector< vector< MenuItem > >::iterator mh;
    vector< MenuItem >::iterator it;
    vector< MenuItem > arg;

    for (mh = _items.begin(); mh != _items.end(); ++mh) {
        arg = *mh;
        for (it = arg.begin(); it != arg.end(); ++it) {
            BOOST_LOG(lg) << "Matching \"" << it->GetAlias() << "\""
                       << " against the argument \"" << str << "\"" << endl;
            if (it->GetAlias() == str){
                BOOST_LOG(lg) << "Matched " << it->GetLabel() << " !" << endl;
                return *it;
            }
        }
    }
    // FIXME: this is a hack to set an error condition
    str.erase();
    return *it;
}

MenuItem &
XantrexUI::GetItem(int x, int y)
{
    DEBUGLOG_REPORT_FUNCTION;
    vector< vector< MenuItem > >::iterator mh;
    vector< MenuItem >::iterator it;
    vector< MenuItem > arg;
    string tmp;
   
    for (mh = _items.begin(); mh != _items.end(); ++mh) {
        for (it = mh->begin(); it != mh->end(); ++it) {
            tmp = it->GetLabel();
            if ((it->GetHeaderIndex() == x) && (it->GetItemIndex() == y)) {
                return *it;
            }
        }
    }

    return *it;                  // FIXME: This probably isn't what we want
}

#if 0
retcode_t
XantrexUI::xantrex_main(Console &con);
{

#if 0
    if (item.size() > 0) {
        // See if the name is actually a specified index
        if (item.find(":",0) != string::npos)
            {
                menuhead = atoi(item.substr(0, item.find(":",0)).c_str());
                menuitem = atoi(item.substr(item.find(":",0) + 1, 2).c_str());
            } else {
            // lookup the alias to get the indexes
            ti = ui.Match(item);
            if (item.size() > 0) {
                menuhead = ti->GetHeaderIndex();
                menuitem = ti.GetItemIndex();
            }
        }
    }
#endif

    // Go into console mode where we can type direct commands
    // via the serial port. This just saves us from firing up
    // kermit or minicom
    if (console) {
        Console con;
        con.Open();
        //ui.SetRaw();
        while ((ch = con.Getc())!='q') {
            if (ch > 0){             // If we have something, process it
                if (echo)
                    con.Putc (ch);          // echo inputted character to screen
          
                switch (ch) {
                  case 'Q':
                  case 'q':
                      con.Reset();
                      exit(0);
                      break;
                  case 'L':
                      display = ui.MenuHeadingMinus();
                      break;
                  case 'R':
                      display = ui.MenuHeadingPlus();
                      break;
                  case 'D':
                      display = ui.MenuItemMinus();
                      break;
                  case 'U':
                      display = ui.MenuItemPlus();
                      break;
                  case '+':
                      display = ui.SetPointPlus();
                      break;
                  case '-':
                      display = ui.SetPointMinus();
                      break;
                  case '|':
                      display = ui.Inverter();
                      break;
                  case 'G':
                      display = ui.Generator();
                      break;
                  case 'S':
                      display = ui.SetupMenu();
                      break;
                  case 's':
                      display = ui.GotoMenuStart();
                      break;
                  case '/':
                      display = ui.LedStatus();
                      break;
                  case 'V':
                      display = ui.Version();
                      break;
                  case 'T':
                      display = ui.SetTerminalMode();
                      break;
                  case '?':
                      con.Puts("PowerGuru - Xantrex mode\r\n");
                      con.Puts("\t? - help\r\n");
                      con.Puts("\tL - Menu Heading Minus\r\n");
                      con.Puts("\tR - Menu Heading Plus\r\n");
                      con.Puts("\tR - Menu Heading Plus\r\n");
                      con.Puts("\tD - Menu Item Minus\r\n");
                      con.Puts("\tU - Menu Item Plus\r\n");
                      con.Puts("\t+ - Value Plus\r\n");
                      con.Puts("\t- - Value Item Plus\r\n");

                      con.Puts("\t| - Inverter Mode\r\n");
                      con.Puts("\tG - Generator Mode\r\n");
                      con.Puts("\tS - Setup Menu\r\n");
                      con.Puts("\t/ - LED Status\r\n");
                      con.Puts("\tV - Version\r\n");
                      con.Puts("\tT - TerminalMode\r\n");
                      con.Puts("\tQ - Quit\r\n");
                  default:
                      break;
                };

                //          display += "\r";
                con.Putc('\r');
                con.Puts(display);
#if 0
                switch (ui.GetDataType()) {
                  case MenuItem::NONE:
                  case MenuItem::MENUHEAD:
                  case MenuItem::MENUITEM:
                  case MenuItem::BOOL:
                      if (ui.GetBoolValue() == false)
                          BOOST_LOG(lg) << "\tBoolean value is: " << "OFF" ;
                      else
                          BOOST_LOG(lg) << "\tBoolean value is: " << "ON" ;      
                      BOOST_LOG(lg) << endl;
                      break;
                  case MenuItem::INT:
                      BOOST_LOG(lg) << "\tInteger value is: " << value.intval << endl;
                      break;
                  case MenuItem::FLOAT:
                      cerr << "\tFIXME: Float value is: " << value.floatval << endl;
                      break;
                  case MenuItem::TIME:
                      BOOST_LOG(lg) << "\tTime value is: " << value.timeval << endl;
                      break;
                  case MenuItem::EOL:
                  case MenuItem::CLOCK:
                  case MenuItem::ENUM:
                  case MenuItem::DATE:
                      BOOST_LOG(lg) << "\tFIXME: unsupported type! " << value.intval << endl;
                      break;
                  default:
                      BOOST_LOG(lg) << "Data Type out of range";
                      break;
                };
#endif
            }

            str += ui.ReadSerial();
            if (str.size() > 0) {
                con.Puts(str);
                str.erase();
            }
        }
    }
    
    // Get the value of a menu item
    if (getitem){
      
        string str = ui.GotoMenuItem(menuhead, menuitem);
        ti = ui.GetItem(menuhead, menuitem);
        //    ui.Read((char *)&buffy, 100);
        cerr << "Inverter returned: " << str << endl;

        //      BOOST_LOG(lg) << "BREAK HERE" << endl;
      
        switch (ti.GetType()) {
          case MenuItem::BOOL:
              if (ui.GetBoolValue(str) == false)
                  cout << "The Boolean value is: NO" << endl;
              else
                  cout << "The Boolean value is: YES" << endl;
              break;
          case MenuItem::INFO:
              value = ui.GetValue(str);
              cout << "The value is: " << value << endl;
              break;
          case MenuItem::INT:
              intval = ui.GetIntValue(str);
              cout << "The Integer value is: " << intval << endl;
              break;
          case MenuItem::FLOAT:
              fltval= ui.GetFloatValue(str);
              cout << "The Float value is: " << fltval << endl;
              break;
          case MenuItem::MENUHEAD:
              cout << "The Menu Header is: " << "\"" <<
                  ui.CleanUpData(ti.GetLabel()) << "\"" << endl;
              break;
          case MenuItem::MENUITEM:
              cout << "The Menu Item is: " << "\"" <<
                  ui.CleanUpData(ti.GetLabel()) << "\"" << endl;
              break;
          case MenuItem::TIME:
              value = ui.GetValue(str);
              cout << "The Time is: " << "\"" << value << endl;
              break;
          case MenuItem::EOL:
              value = ui.GetValue(str);
              cout << "The Time is: " << "\"" << value << endl;
              break;
          case MenuItem::CLOCK:
              value = ui.GetValue(str);
              cout << "The Clock value is: " << "\"" << value << endl;
              break;
          case MenuItem::ENUM:
              value = ui.GetValue(str);
              cout << "The Enum value is: " << "\"" << value << endl;
              break;
          case MenuItem::DATE:
              value = ui.GetValue(str);
              cout << "The Date is: " << "\"" << value << endl;
              break;
          default:
              cout << "The value is: " << value << endl;
              value = str;
        };
    } // endof getitem

    // Monitor an item. This means grabs successive values and
    // display them. This is for selections that constantly update
    // the value.
    if (monitor) {
        string str = ui.GotoMenuItem(menuhead, menuitem);
        ti = ui.GetItem(menuhead, menuitem);

        cout << "The Menu Header is: " << "\"" <<
            ui.CleanUpData(ti.GetLabel()) << "\"" << endl;
      
        do {
            memset(buffy, 0, 100);        
            ret = ui.Read((char *)&buffy, 100);
            str = buffy;
            if (ret < 0)            // there is no input yet, try again
                continue;

            // We got something, so extract the type, and process the
            // value.
            switch (ti.GetType()) {
              case MenuItem::BOOL:
                  if (ui.GetBoolValue(str) == false)
                      cout << "\tThe Boolean value is: NO" << endl;
                  else
                      cout << "\tThe Boolean value is: YES" << endl;
                  break;
              case MenuItem::INFO:
                  value = ui.GetValue(str);
                  cout << "\tThe value is: " << value << endl;
                  break;
              case MenuItem::INT:
                  intval = ui.GetIntValue(str);
                  cout << "\tThe Integer value is: " << intval << endl;
                  break;
              case MenuItem::FLOAT:
                  fltval= ui.GetFloatValue(str);
                  cout << "\tThe Float value is: " << fltval << endl;
                  break;
              case MenuItem::MENUHEAD:
                  cout << "\tThe Menu Header is: " << "\"" <<
                      ui.CleanUpData(ti.GetLabel()) << "\"" << endl;
                  break;
              case MenuItem::MENUITEM:
                  cout << "\tThe Menu Item is: " << "\"" <<
                      ui.CleanUpData(ti.GetLabel()) << "\"" << endl;
                  break;
              default:
                  cout << "\tThe value is: " << value << endl;
                  value = str;
            };
        } while (ret); // endof monitor   
    }
}
#endif

meter_data_t *
XantrexUI::exportMeterData(meter_data_t *data)
{
#if 0
    // The unit number for the device
    data->unit = _address;
  
    // The type of the device
    switch (_type) {
      case OUTBACK_MX:
          data->type = MX_OUTBACK;
          break;
      case OUTBACK_FX:
          data->type = FX_OUTBACK;
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
    data->battery_volts = _battery_voltage;
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
    data->daily_kwh = _daily_kwh;
    // The frequency in hertz. This doesn't exist on an Outback, but does on
    // a Xantrex. It should always be 60 though, so we set it so other
    // software can handle this  the same way.
    data->hertz = 60;
#endif

    return data;
}

#ifdef BUILD_XANTREX
    if (xantrexmode) {
        XantrexUI ui;
        // Open a console for user input
        con.Open();
        if (poll) {
            // Open the serial port
            try {
                ui.Open(filespec);
            }
            catch (ErrCond catch_err) {
                cerr << catch_err << endl;
                exit(1);
            }
            //
            for (i=0; i<1000; i++) {
                //display = ui.MenuHeadingPlus();
#if 0
                ch = con.Getc();
                switch (ch) {
                    // Toggle the DTR state, which is as close as we get to
                    // flow control.
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
#endif

                vector<meter_data_t *> data = ui.PollMeters(1);
//           ui.MenuHeadingPlus();
//           ui.MenuHeadingMinus();
#if defined(HAVE_MARIADB) && defined(HAVE_POSTGRESQL)
                pdb.queryInsert(data);
#if 0
                for (i=0; i<data->size(); i++) {
                    //cout << "Inverter/Charger amps: " << data[i]->inverter_amps << endl;
                    cout << "Input amps AC: " << data[i]->input_amps << endl;
                    cout << "Load  amps AC: " << data[i]->load_amps << endl;
                    cout << "Battery actual volts DC: " << data[i]->actual_volts << endl;
                    cout << "Battery TempComp volts DC: " << data[i]->tempcomp_volts << endl;
                    cout << "Inverter volts AC: " << data[i]->inverter_volts << endl;
                    cout << "Grid (AC1) volts AC: " << data[i]->ac1 << endl;
                    cout << "Generator (AC2) volts AC: " << data[i]->ac2 << endl;
                    cout << "Read Frequency Hertz: " << data[i]->hertz << endl;
                    //pdb.queryInsert(data[i]);
                    //delete data[i];
                }
#endif
#endif
                //sleep(1);
                cout << endl;
            }

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
