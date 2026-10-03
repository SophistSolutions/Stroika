#!/usr/bin/env powershell 

function Test-RegistryValue {
    param (
        [parameter(Mandatory=$true)][ValidateNotNullOrEmpty()]$Path,
        [parameter(Mandatory=$true)][ValidateNotNullOrEmpty()]$Value
    )
    try {
        return Get-ItemPropertyValue -Path $Path -Name $Value -ErrorAction Stop
    } catch {
        return $false   # never set means Developer Mode was never enabled
    }
}


# Developer Mode only matters to an account WITHOUT the "Create symbolic links" privilege - one that
# has it (an elevated administrator, or ContainerAdministrator in a Windows Docker container, where
# Developer Mode cannot be turned on at all) makes symlinks regardless. whoami /priv lists every
# privilege the token holds, enabled or not; the privilege name is not localized.
# Started via Process.Start, not by name: we are run from MSYS/Cygwin bash, whose PATH puts coreutils'
# whoami (no /priv) first, and whose PATHEXT can reach us as just ".CPL" - so PowerShell would not even
# run "...\whoami.exe" ("Cannot run a document in the middle of a pipeline").
$psi = New-Object System.Diagnostics.ProcessStartInfo "$env:SystemRoot\System32\whoami.exe", "/priv /fo csv"
$psi.UseShellExecute = $false
$psi.RedirectStandardOutput = $true
$whoami = [System.Diagnostics.Process]::Start($psi)
$HasSymlinkPrivilege = $whoami.StandardOutput.ReadToEnd().Contains("SeCreateSymbolicLinkPrivilege")
$whoami.WaitForExit()

# Create AppModelUnlock if it doesn't exist, required for enabling Developer Mode
$RegistryKeyPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock\"
if (-not $HasSymlinkPrivilege -and -not(Test-RegistryValue -Path $RegistryKeyPath -Value "AllowDevelopmentWithoutDevLicense")) {
    #New-Item -Path $RegistryKeyPath -ItemType Directory -Force
    echo "Warning: Developer Mode not enabled - goto Settings, and search for Developer Mode, and enable, else ln -s calls might not work"
    echo "Settings: For Developers: Enable Developer Mode"
}
