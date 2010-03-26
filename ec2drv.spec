Name: ec2drv
Version: 0.5
Release: 1
Vendor: Ricky White
License: GPL
Summary: Linux support for the EC2 and EC3 debug adaptors from Silicon Labs
Group: Development/Debuggers
Packager: rickyw@neatstuff.co.nz
BuildRoot:  %{_tmppath}/%{name}-root 
Source: http://ec2drv.sourceforge.net/ec2drv-0.5.tar.gz

%description


%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
--disable-debug --enable-debug=no

%build
%configure
make

%install
rm -rf %{buildroot}
%makeinstall

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog NEWS README TODO
%{_bindir}/*
#%{_libdir}/*.so.*
#%{_libdir}/*.a
#%{_libdir}/*.la
%{_libdir}/libec2drv*
#%{_datadir}/%{name}
%{_datadir}/*
#%{_mandir}/man8/*
%changelog

