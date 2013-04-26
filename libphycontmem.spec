Name:		libphycontmem
Version:	2.0
Release:	1%{?dist}
Summary:	OLPC-maintained libphycontmem

License:	GPLv2+
URL:		http://wiki.laptop.org/go/Vmeta
Source0:	libphycontmem-%{version}.tar.gz

%description
libphycontmem

%package devel
Summary: libphycontmem-devel
Requires: %{name} = %{version}-%{release}

%description devel
libphycontmem-devel

%prep
%setup -q


%build
make %{?_smp_mflags} CFLAGS="%{optflags}"


%install
mkdir -p $RPM_BUILD_ROOT/%{_libdir}
cp -a libphycontmem.so $RPM_BUILD_ROOT/%{_libdir}

mkdir -p $RPM_BUILD_ROOT/%{_includedir}
install -m 0644 phycontmem.h $RPM_BUILD_ROOT/%{_includedir}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


%files
%{_libdir}/*.so

%files devel
%{_includedir}/*


%changelog
