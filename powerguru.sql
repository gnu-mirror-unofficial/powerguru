-- 
-- Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
--               2014, 2015, 2016, 2017, 2018, 2019
--	Free Software Foundation, Inc.
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 3 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
-- 
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

-- MySQL dump 10.8
--
-- Host: darkstar    Database: powerguru
-- ------------------------------------------------------
-- Server version	4.1.7

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE="NO_AUTO_VALUE_ON_ZERO" */;

--
-- Name: data; Type: TYPE; Schema: public; Owner: rob
--
DROP TYPE IF EXISTS channel_type;
CREATE TYPE public.channel_type AS ENUM (
    'A',
    'B',
    'C'
);

DROP TYPE IF EXISTS volt_type;
CREATE TYPE public.volt_type AS ENUM (
    'AC',
    'DC'
);

DROP TYPE IF EXISTS device_type;
CREATE TYPE public.device_type AS ENUM (
    'unknown',
    'onewire',
    'ownet',
    'rtl433',
    'rtlsdr',
    'usb',
    'serial',
    'gpio'
);

DROP TYPE IF EXISTS sensor_type;
CREATE TYPE public.sensor_type AS ENUM (
    'UNKNOWN',
    'ACVOLTAGE',
    'DCVOLTAGE',
    'AUTH',
    'BATTERY',
    'POWER',
    'CLOCK',
    'TEMPERATURE',
    'MOISTURE',
    'UNSUPPORTED'
);

--
-- Table structure for table `meters`
--
DROP TABLE IF EXISTS meters;
CREATE TABLE meters (
  unit integer NOT NULL default '0',
  --device_type enum('NONE','MX','FX','SW') NOT NULL default 'NONE',
  charge_amps integer NOT NULL default '0',
  ac_load_amps integer NOT NULL default '0',
  battery_volts float NOT NULL default '0',
  ac_volts_out float NOT NULL default '0',
  ac1_volts_in float NOT NULL default '0',
  ac2_volts_in float NOT NULL default '0',
  pv_amps_in integer NOT NULL default '0',
  pv_volts_in float NOT NULL default '0',
  buy_amps integer NOT NULL default '0',
  sell_amps integer NOT NULL default '0',
  daily_kwh float NOT NULL default '0',
  hertz integer NOT NULL default '0',
  battery_tempcomp float NOT NULL default '0'
);

DROP TABLE IF EXISTS sensor;
CREATE TABLE sensor (
  id varchar(12) NOT NULL default '0',
  alias varchar(12) NOT NULL default '0',
  location varchar(12) NOT NULL default '0',
  device device_type NOT NULL default '1wire',
  type sensor_type NOT NULL default 'UNKNOWN',
  channel channel_type NOT NULL default 'A'
);

DROP TABLE IF EXISTS temperature;
CREATE TABLE temperature (
  id varchar(12) NOT NULL default '0',
  temperature float NOT NULL default '0',
  temphigh float NOT NULL default '0',
  templow float NOT NULL default '0',
  humidity float NOT NULL default '0',
  scale char(1) NOT NULL default 'F',
  "timestamp" timestamp without time zone UNIQUE
);

DROP TABLE IF EXISTS power;
CREATE TABLE power (
  id char(16) NOT NULL default '0',
  current float NOT NULL default '0',
  volts float NOT NULL default '0',
  type volt_type NOT NULL default 'DC',
  "timestamp" timestamp without time zone UNIQUE
);

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

