#!/usr/bin/env powershell
Write-Warning "ScriptsLib/WarnIfNotWindowsDeveloperMode.ps1 is deprecated -- use Build/Scripts/WarnIfNotWindowsDeveloperMode.ps1 instead"
& "$PSScriptRoot\..\Build\Scripts\WarnIfNotWindowsDeveloperMode.ps1" @args
