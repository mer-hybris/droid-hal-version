%if 0%{?_obs_build_project:1}
%define _build_flavour %(echo %{_obs_build_project} | awk -F : '{if (NF == 3) print $3; else if (NF == 2) print strdevel; else print strunknown}' strdevel=devel strunknown=unknown)
%else
%define _build_flavour unknown
%endif

# needs to match the prjconf in pj:tools
%define _obs_build_count %(echo %{release} | awk -F . '{if (NF >= 3) print $3; else print $1 }')
%define _obs_commit_count %(echo %{release} | awk -F . '{if (NF >= 2) print $2; else print $1 }')

%if %{_build_flavour} == release
%define _version_appendix (%{_target_cpu})
%else
%define _version_appendix (%{_target_cpu},%{_build_flavour})
%endif

Name:    droid-hal-version-%{rpm_device}
Version: 0.0.1
Release: 1
Summary: SailfishOS HW Adaptation droid version package %{version}.%{_obs_build_count} (%{_target_cpu},%{_build_flavour})
Group:   System/Libraries
License: BSD-3-Clause
Source:  %{name}-%{version}.tar.gz

# Generic dependencies
BuildRequires: droid-hal
BuildRequires: droid-config
BuildRequires: droid-config-sailfish
BuildRequires: libhybris
BuildRequires: mce-plugin-libhybris

%if 0%{?have_droid_bin:1}
BuildRequires: droid-bin
%endif

# Kernel & Boot
BuildRequires: droid-hal-kernel
BuildRequires: droid-hal-img-boot
BuildRequires: droid-hal-img-recovery
BuildRequires: droid-config-preinit-plugins

# Kernel modules
%if 0%{?have_kernel_modules:1}
BuildRequires: droid-hal-kernel-modules
%endif

# Audio
BuildRequires: pulseaudio-modules-droid
BuildRequires: droid-config-pulseaudio-settings

# Camera
%if 0%{?have_camera:1}
BuildRequires: gstreamer0.10-droidcamsrc
%endif

# Multimedia
%if 0%{?have_mm_omx:1}
BuildRequires: gstreamer0.10-omx
%endif

# Haptics
%if 0%{?have_vibrator:1}
BuildRequires: ngfd-plugin-droid-vibrator
BuildRequires: qt5-feedback-haptics-droid-vibrator
%endif

# Sensors
BuildRequires: hybris-libsensorfw-qt5

# Graphics
BuildRequires: qt5-qpa-hwcomposer-plugin
BuildRequires: qtscenegraph-adaptation

%description
SailfishOS HW Adaptation %{rpm_device} version/umbrella package (%{version}.%{_obs_build_count}) for %{_target_cpu} platform.

%files
%defattr(-,root,root,-)
%config %{_sysconfdir}/hw-release
%{_datadir}/sailfish-version/packagelist.d/*

%package doc
Summary: SailfishOS %{version}.%{_obs_build_count} (%{_target_cpu},%{_build_flavour})
Group: System/Libraries

%description doc
%{summary}.

%files doc
%defattr(-,root,root,-)
%doc %{_datadir}/doc/SailfishOS/*

%prep

%build

%install
echo "Building for %{_build_flavour}"
mkdir -p %{buildroot}/%{_datadir}/sailfish-version/packagelist.d
RPM_PATH=${RPM_SOURCE_DIR:-rpm}/${RPM_PACKAGE_NAME:-droid-hal-version-%{rpm_device}}.spec
for req in `rpmspec -q --buildrequires $RPM_PATH`; do
    rpm -qa $req >> %{buildroot}/%{_datadir}/sailfish-version/packagelist.d/%{name}
done
mkdir -p %{buildroot}/%{_sysconfdir}

echo Creating hw-release
# based on http://www.freedesktop.org/software/systemd/man/os-release.html
cat > %{buildroot}/%{_sysconfdir}/hw-release <<EOF
# This file is copied as hw-release (analogous to os-release)
NAME="%{vendor_pretty} %{device_pretty}"
ID=%{rpm_device}
MER_HA_DEVICE=%{rpm_device}
MER_HA_VENDOR=%{rpm_vendor}
VERSION="%{version}.%{_obs_build_count} %{_version_appendix}"
VERSION_ID=%{version}.%{_obs_build_count}
PRETTY_NAME="$DEVICE %{version}.%{_obs_build_count} %{_version_appendix}"
SAILFISH_BUILD=%{_obs_build_count}
SAILFISH_FLAVOUR=%{_build_flavour}
HOME_URL="https://sailfishos.org/"
EOF
cat %{buildroot}/%{_sysconfdir}/hw-release

mkdir -p %{buildroot}/%{_datadir}/doc/SailfishOS
cp %{buildroot}/%{_datadir}/sailfish-version/packagelist.d/* %{buildroot}/%{_sysconfdir}/hw-release %{buildroot}/%{_datadir}/doc/SailfishOS/
rpm -qa | sort > %{buildroot}/%{_datadir}/doc/SailfishOS/extended-packagelist-droid

