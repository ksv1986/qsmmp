#
# spec file for package qsmmp
#
# Copyright (c) 2015 ksv1986
#

%global commit c85ce287ed73a126ace8f42fa65f7dcd92559428
%global shortcommit %(c=%{commit}; echo ${c:0:7})

%define soname 0

Name:           qsmmp
Version:        20150404+g%{shortcommit}
Release:        0
Summary:        QSMMP is alternative UI for QMMP implemented as a plugin with native Qt look.
License:        GPL-2.0
Group:          Productivity/Multimedia/Sound/Players
Url:            https://github.com/ksv1986/qsmmp
Source0:        https://github.com/ksv1986/qsmmp/archive/%{commit}.zip
BuildRequires:  unzip
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig(QtCore)
BuildRequires:  pkgconfig(QtGui)
BuildRequires:  pkgconfig(qmmp)
BuildRequires:  pkgconfig(qmmpui)
BuildRequires:  libqxt-devel
Provides:       qmmp(%{soname})(Ui)

%description
QSMMP is alternative UI for QMMP implemented as a plugin with native Qt look.
You need to have QMMP installed before building QSMMP.
You could download QMMP sources from http://code.google.com/p/qmmp/

%prep
%setup -qn %{name}-%{commit}

%build
qmake QMAKE_CFLAGS="%{optflags}" QMAKE_CXXFLAGS="%{optflags}"
make %{?_smp_mflags}

%install
make INSTALL_ROOT=%{buildroot} install

%files
%defattr(-,root,root,-)
%doc /usr/share/doc/qsmmp
%dir %{_libdir}/qmmp
%dir %{_libdir}/qmmp/Ui
%{_libdir}/qmmp/Ui/lib%{name}.so

%changelog
