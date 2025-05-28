#!/usr/bin/env powershell 

# Create AppModelUnlock if it doesn't exist, required for enabling Developer Mode
$RegistryKeyPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock\"
if (-not(Get-ItemPropertyValue -Path $RegistryKeyPath -name "AllowDevelopmentWithoutDevLicense")) {
    #New-Item -Path $RegistryKeyPath -ItemType Directory -Force
    echo "Warning: Developer Mode not enabled - goto Settings, and search for Developer Mode, and enable, else ln -s calls might not work"
    echo "Settings: For Developers: Enable Developer Mode"
}
