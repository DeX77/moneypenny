%define is_mandrake %(test -e /etc/mandrake-release && echo 1 || echo 0)
%define is_suse %(test -e /etc/SuSE-release && echo 1 || echo 0)
%define is_fedora %(test -e /etc/fedora-release && echo 1 || echo 0)

%define dist redhat
%define disttag rh

%if %is_mandrake
%define dist mandrake
%define disttag mdk
%endif
%if %is_suse
%define dist suse
%define disttag suse
%endif
%if %is_fedora
%define dist fedora
%define disttag rhfc
%endif

%define distver %(release="`rpm -q --queryformat='%{VERSION}' %{dist}-release 2>/dev/null`" ; if test $? != 0 ; then release="" ; fi ; echo "$release")

Summary: A Homebanking program for HBCI / Ein Homebanking Programm fuer HBCI 
Name: moneypenny
Version: 3.0
Release: 1.%{disttag}%{distver}
Source: http://www-in.fh-swf.de/Money-Penny/moneypenny.tar.gz 
BuildRequires: openssl-devel >0.9.6 gwenhywfar-devel = 1.19 gcc >= 3.0 openhbci2-devel = 1.9.3 aqmoney2-devel = 1.9.3
Group: Applications/Productivity
License: GPL
Packager: Walter Roth <moneypenny@fb-in.fh-swf.de>
URL: http://www-in.fh-swf.de/Money-Penny
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Prereq: /sbin/ldconfig

%description
English see below

MoneyPenny ist ein Programm fuer das Homebanking nach dem deutschen
HBCI Standard. Es unterstuetzt HBCI 2.0.1, 2.1 und 2.2 mit oder ohne Chipkarte
(RDH oder DDV Verfahren). PIN-TAN und HBCI PIN-TAN werden nicht unterstuetzt.
Um MoneyPenny verwenden zu koennen, muessen Sie bei Ihrer Bank
HBCI Homebanking beantragen. 
HBCI 2.0.1 ist problematisch. Bei vielen Servern, die nur HBCI 2.0.1 anbieten,
funktionieren Teile des HBCI-Protokolls nicht richtig. Die erste richtig stabile
HBCI Version ist 2.1.

Moneypenny is a program for homebanking according to the German HBCI standard.
It supports HBCI 2.0.1, 2.1 and 2.2 with or without chipcard (RDH or DDV procedures). 
PIN TAN and HBCI PIN TAN are not supported. If you want to use MoneyPenny, your
bank has to support HBCI (which is very unlikely outside Germany) and you have to 
apply for an HBCI access to your accounts.
Many servers that offer HBCI 2.0.1 only, suffer from a poor implementation. 
The first really stable HBCI version is 2.1
%prep
%setup 

%build
qmake "CONFIG+=release" moneypenny.pro
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
make INSTALL_ROOT=$RPM_BUILD_ROOT PREFIX=/opt install

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0755)
%doc AUTHORS README LIESMICH INSTALL INSTALLATION COPYING 
/opt/moneypenny/*

%changelog -n moneypenny
* Thu Jan 22 2009 Walter Roth
-MP 2.0 RC packed
