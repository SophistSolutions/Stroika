#!/usr/bin/env powershell 

function Test-RegistryValue {
    param (
        [parameter(Mandatory=$true)][ValidateNotNullOrEmpty()]$Path,
        [parameter(Mandatory=$true)][ValidateNotNullOrEmpty()]$Value
    )
    try {
        return Get-ItemPropertyValue -Path $Path -Name $Value -ErrorAction Stop 
    } catch {
        return $true
    }
}


# Create AppModelUnlock if it doesn't exist, required for enabling Developer Mode
$RegistryKeyPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock\"
if (-not(Test-RegistryValue -Path $RegistryKeyPath -Value "AllowDevelopmentWithoutDevLicense")) {
    #New-Item -Path $RegistryKeyPath -ItemType Directory -Force
    echo "Warning: Developer Mode not enabled - goto Settings, and search for Developer Mode, and enable, else ln -s calls might not work"
    echo "Settings: For Developers: Enable Developer Mode"
}
