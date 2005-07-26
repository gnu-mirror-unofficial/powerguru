# When using rpm 4.1 or newer, the behaviour changed to terminate
# if any files are missing. This is stupid as if you have scrollkeeper
# support for the docs, they need to be included in the package. As
# these are system files, this is down right stupid, so we turn this
# "feature" off.
#
# So anyway, do this to get the rpm to build:
# echo "%_unpackaged_files_terminate_build 0" >> /etc/rpm/macros
#
# Turns out this also works.
%define _unpackaged_files_terminate_build 0

%define version 0.1
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

%description
Powerguru is an program for monitoring, data logging, and controlling
your inverter, charger controller, or power meter.

%prep
%setup -q -n powerguru-%{version}

%build
./configure --disable-gtkhtml-help --prefix=%{_prefix} \
    --bindir=%{_bindir} --mandir=%{_mandir} \
    --localstatedir=%{localstatedir} --libdir=%{_libdir} \
    --datadir=%{_datadir} --includedir=%{_includedir} \
    --sysconfdir=%{_sysconfdir}
CFLAGS="$RPM_OPT_FLAGS" make all
CFLAGS="$RPM_OPT_FLAGS" make -C doc/C alldocs

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

# %config site.exp

%post
/sbin/ldconfig
if which scrollkeeper-update>/dev/null 2>&1; then scrollkeeper-update; fi

%postun
/sbin/ldconfig
if which scrollkeeper-update>/dev/null 2>&1; then scrollkeeper-update; fi

%changelog
* Sat Jun 11 2005 Rob Savoye <rob@senecass.com>
- Update version number for 0.1 release.

