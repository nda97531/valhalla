Name:           osmpbf
Version:        1.3.3
Release:        2%{?dist}
Summary:        C version of the PBF library code
License:        LGPL-3+
URL:            https://github.com/scrosby/OSM-binary     
Source0:        OSM-binary-%{version}.tar.bz2
BuildRequires:  protobuf-devel, protobuf-lite-devel
BuildRequires:  gcc, gcc-c++

%description
Osmpbf is a Java/C library to read and write OpenStreetMap PBF files.
PBF (Protocol buffer Binary Format) is a binary file format for OpenStreetMap
data that uses Google Protocol Buffers as low-level storage.

For more information see http://wiki.openstreetmap.org/wiki/PBF_Format
 
%prep
%setup -q -n OSM-binary-%{version}
 
%build
%{__make} -C src %{?jobs:-j%jobs}
%{__make} -C tools %{?jobs:-j%jobs}
 
%install
%{__make} -C src DESTDIR=%{buildroot} PREFIX=%{_prefix} LIBDIR=%{_libdir} install
%{__make} -C tools DESTDIR=%{buildroot} PREFIX=%{_prefix} install
install -m 644 tools/osmpbf-outline.1 $RPM_BUILD_ROOT%{_mandir}/man1/

%files
%defattr(-, root, root, -)
%doc COPYING.osmpbf README ReleaseNotes.txt
%dir %{_includedir}/osmpbf
%{_includedir}/osmpbf/*.h
%{_bindir}/osmpbf-outline
%{_mandir}/man1/osmpbf-outline.1*
%{_libdir}/libosmpbf.a

%changelog
* Wed Mar 19 2014 <kay.diam@gmail.com> - 1.3.3-2
- Bump version
- Updated spec file to use normal make

* Mon Sep 23 2013 <kay.diam@gmail.com> - 1.3.1-1
- Initial release
