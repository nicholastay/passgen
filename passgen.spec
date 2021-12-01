Name: passgen
Version: 0.1.1
Release: 1%{?dist}
License: Zlib
Summary: A simple memorable and customisable password generator
Url: https://gitlab.com/nicholastay/passgen
# Sources can be obtained by
# git clone https://gitlab.com/nicholastay/passgen.git
# cd passgen
# tito build --tgz
Source0: %{name}-%{version}.tar.gz

BuildRequires: gcc
BuildRequires: make

%description
Just a password generator that makes passwds.ninja-style passwords.

Heavily inspired by a generator I used before at work, just that this time in C. (This is my first C project, please be gentle...)

%prep
%autosetup

%build
CFLAGS='-g' %make_build

%install
%make_install PREFIX=%{_prefix}

%files
%doc README.md
%license LICENCE
%{_bindir}/%{name}

%changelog
* Wed Dec 01 2021 Nicholas Tay <nick@windblume.net> 0.1.1-1
- RPM spec: add gcc, make to BuildRequires

* Wed Dec 01 2021 Nicholas Tay <nick@windblume.net> 0.1.0-1
- New package built with tito

* Wed Dec 01 2021 Nicholas Tay <nick@windblume.net>
- Initial spec
