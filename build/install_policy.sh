#!/bin/sh

# Check if running as root
if [ "$(id -u)" != "0" ]; then
  echo "This script must be run as root" 1>&2
  exit 1
fi

# Create the Polkit policy file
cat <<EOF >/etc/polkit-1/localauthority/50-local.d/com.example.hwminfo.pkla
[HWMInfo]
Identity=unix-user:*
Action=org.freedesktop.policykit.exec
ResultAny=yes
ResultInactive=yes
ResultActive=yes
EOF

echo "Polkit policy for HWMio installed successfully."
