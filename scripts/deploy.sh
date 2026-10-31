#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

echo "== Build (Release) =="
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

echo "== Create service user (if missing) =="
if ! id satellite >/dev/null 2>&1; then
    sudo useradd --system --no-create-home --shell /usr/sbin/nologin satellite
fi

echo "== Install binary and unit file =="
sudo mkdir -p /opt/satellite
sudo install -m 755 build/onboard /opt/satellite/onboard
sudo install -m 644 deploy/satellite-onboard.service \
    /etc/systemd/system/satellite-onboard.service

echo "== Enable and (re)start service =="
sudo systemctl daemon-reload
sudo systemctl enable satellite-onboard
sudo systemctl restart satellite-onboard
sleep 1
systemctl --no-pager status satellite-onboard || true
