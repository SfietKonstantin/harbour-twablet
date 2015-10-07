Name: harbour-twablet
Summary: Twablet
Version: 0.0.0
Release: 1
Group: Qt/Qt
License: BSD
URL: http://github.com/sfietkonstantin/harbour-twablet
Source0: %{name}-%{version}.tar.bz2
Source1: twitter-secrets.h
Requires: sailfishsilica-qt5 >= 0.10.9
BuildRequires: pkgconfig(sailfishapp) >= 1.0.2
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Test)
BuildRequires: desktop-file-utils
BuildRequires: cmake

%description
A Twitter application for big screens

%prep
%setup -q -n %{name}-%{version}
cp -p %{SOURCE1} src/lib

%build
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
