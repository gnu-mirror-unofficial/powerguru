# 
# Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013
#               2014, 2015, 2016, 2017, 2018, 2019
#	Free Software Foundation, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

%define version 0.5dev
%define localstatedir   /var/lib

Summary: A monitoring and control program for power devices.
Name: powerguru
Version: %{version}
License: GPL
Release: %{?custom_release}%{!?custom_release:1}
Source: http://www.senecass.com/software/powerguru/powerguru-%{version}.tar.gz
URL: http://www.senecass.com/software/powerguru/
Vendor: Seneca Software & Solar, Inc.
Packager: Rob Savoye <rob@senecass.com>
Group: Applications/Engineering
BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot
Requires: owfs-shell owfs-capi owfs-server owfs-ownet owfs-devel boost-devel libxml2-devel net-snmp-devel docbook2X docbook-utils docbook5-style-xsl docbook-utils-pdf postgresql-devel

%description
Powerguru is an program for monitoring, data logging, and controlling
your inverter, charger controller, or power meter.

%prep
%setup -n powerguru-%{version}
./autogen.sh

%build
./configure --prefix=%{_prefix} \
    --bindir=%{_bindir} --mandir=%{_mandir} \
    --localstatedir=%{localstatedir} --libdir=%{_libdir} \
    --datadir=%{_datadir} --includedir=%{_includedir} \
    --sysconfdir=%{_sysconfdir}
CXXFLAGS="$RPM_OPT_FLAGS" make all
CXXFLAGS="$RPM_OPT_FLAGS" make -C doc/C install-doc

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{_prefix} bindir=$RPM_BUILD_ROOT%{_bindir} \
    mandir=$RPM_BUILD_ROOT%{_mandir} libdir=$RPM_BUILD_ROOT%{_libdir} \
    localstatedir=$RPM_BUILD_ROOT%{localstatedir} \
    datadir=$RPM_BUILD_ROOT%{_datadir} \
    includedir=$RPM_BUILD_ROOT%{_includedir} \
    sysconfdir=$RPM_BUILD_ROOT%{_sysconfdir} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc COPYING NEWS README AUTHORS INSTALL ChangeLog doc/C/powerguru.html doc/C/powerguru.pdf doc/C/images doc/C/*.xml doc/C/topic.dat
%defattr(-,root,root)
/usr/include/powerguru/*
%{_bindir}/*
#{_mandir}/man1/*
%{_libdir}/powerguru/libpguru.*
%{_datadir}/omf/powerguru/powerguru.omf

#%post
#/sbin/ldconfig
#if which scrollkeeper-update>/dev/null 2>&1; then scrollkeeper-update; fi

#%postun
#/sbin/ldconfig
#if which scrollkeeper-update>/dev/null 2>&1; then scrollkeeper-update; fi

%changelog
* Sun Jan 13 2019 Rob Savoye <rob@senecass.com>
- Update version after heavy refactoring of all the code.

* Sat Jun 11 2005 Rob Savoye <rob@senecass.com>
- Update version number for 0.1 release.

