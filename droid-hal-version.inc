%if 0%{?_obs_build_project:1}
%define _build_flavour %{?qa_stage_name}%{!?qa_stage_name:unknown}
%else
%define _build_flavour unknown
%endif

# needs to match the prjconf in pj:tools
%define _obs_build_count %(echo %{release} | awk -F . '{if (NF >= 3) print $3; else print $1 }')
%define _obs_commit_count %(echo %{release} | awk -F . '{if (NF >= 2) print $2; else print $1 }')

%if "%{_build_flavour}" == "release"
%define _version_appendix %{nil}
%else
%define _version_appendix \ (%{_build_flavour})
%endif

%define _shortened_version %(echo %{version} | awk -F '+' '{print $1}')

%if 0%{!?rpm_device:1}
%define rpm_device %{device}
%endif
%if 0%{!?rpm_vendor:1}
%define rpm_vendor %{vendor}
%endif

%if 0%{!?home_url:1}
%define home_url "https://sailfishos.org/"
%endif

Name:    droid-hal-version-%{rpm_device}
Version: 0.0.1
%if 0%{?_obs_build_project:1}
Release: 1
%else
%define rel_date %(date +'%%Y%%m%%d%%H%%M')
Release: %{rel_date}
%endif
Summary: SailfishOS HW Adaptation droid version package %{_shortened_version}.%{_obs_build_count} (%{_build_flavour})
License: BSD
Source:  %{name}-%{version}.tar.gz
# For devices that were last reflashed before switching from patterns to
# meta-packages, otherwise they will miss any patterns changes in the future:
Requires: patterns-sailfish-device-configuration-%{rpm_device}

# Generic dependencies
%if 0%{!?native_build:1}
BuildRequires: droid-hal
%endif
BuildRequires: droid-config
BuildRequires: droid-config-sailfish
%if 0%{!?native_build:1}
BuildRequires: libhybris
%endif

# LEDs for notifications
%if 0%{?have_led:1}
BuildRequires: mce-plugin-libhybris
%endif

# If /system has been packaged to rpm package
%if 0%{?have_droid_bin:1}
BuildRequires: droid-bin
%endif

# Kernel & Boot
%if 0%{?_obs_build_project:1}
BuildRequires: droid-hal-img-boot
BuildRequires: droid-hal-img-recovery
%else
BuildRequires: droid-hal-kernel
%if 0%{?have_kernel_modules:1}
BuildRequires: droid-hal-kernel-modules
%endif
%endif
BuildRequires: droid-config-preinit-plugins

# Audio
%if 0%{!?native_build:1}
BuildRequires: pulseaudio-modules-droid
%endif
BuildRequires: droid-config-pulseaudio-settings

# Haptics native
%if 0%{?have_vibrator_native:1} && 0%{!?have_vibrator:1} && 0%{!?have_ffmemless:1}
BuildRequires: ngfd-plugin-native-vibrator
%endif

# Haptics
%if 0%{!?have_vibrator_native:1} && 0%{?have_vibrator:1} && 0%{!?have_ffmemless:1}
BuildRequires: ngfd-plugin-droid-vibrator
BuildRequires: qt5-feedback-haptics-droid-vibrator
%endif


# Sensors
%if 0%{!?native_build:1}
BuildRequires: hybris-libsensorfw-qt5
%endif

# Graphics
%if 0%{!?native_build:1}
BuildRequires: qt5-qpa-hwcomposer-plugin
BuildRequires: qtscenegraph-adaptation
%endif

%description
SailfishOS HW Adaptation %{rpm_device} version/umbrella package (%{version}.%{_obs_build_count}).

%files
%{_prefix}/lib/hw-release
%{_sysconfdir}/hw-release

%package doc
Summary: SailfishOS %{version}.%{_obs_build_count} (%{_build_flavour})

%description doc
%{summary}.

%files doc
%doc %{_datadir}/doc/SailfishOS/*

%prep

%build

%install
echo "Building for %{_build_flavour}"
mkdir -p %{buildroot}%{_prefix}/lib
mkdir -p %{buildroot}%{_sysconfdir}
echo Creating hw-release
# based on http://www.freedesktop.org/software/systemd/man/os-release.html
cat > %{buildroot}%{_prefix}/lib/hw-release <<EOF
# This file is copied as hw-release (analogous to os-release)
NAME="%{vendor_pretty} %{device_pretty}"
ID=%{rpm_device}
MER_HA_DEVICE=%{rpm_device}
MER_HA_VENDOR=%{rpm_vendor}
VERSION="%{version}.%{_obs_build_count}%{_version_appendix}"
VERSION_ID=%{version}.%{_obs_build_count}
PRETTY_NAME="%{device_pretty} %{version}.%{_obs_build_count}%{_version_appendix}"
SAILFISH_BUILD=%{_obs_build_count}
SAILFISH_FLAVOUR=%{_build_flavour}
HOME_URL=%{home_url}
EOF
cat %{buildroot}%{_prefix}/lib/hw-release
ln -s ..%{_prefix}/lib/hw-release %{buildroot}%{_sysconfdir}/hw-release

mkdir -p %{buildroot}%{_datadir}/doc/SailfishOS
cp %{buildroot}%{_sysconfdir}/hw-release %{buildroot}%{_datadir}/doc/SailfishOS/
rpm -qa | sort > %{buildroot}%{_datadir}/doc/SailfishOS/extended-packagelist-droid
