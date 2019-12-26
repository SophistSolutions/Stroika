all: vswhere.exe

vswhere.exe:
	powershell "invoke-webrequest -outfile vswhere.exe https://github.com/microsoft/vswhere/releases/download/2.8.4/vswhere.exe"
	
whereLatest: vswhere.exe
	./vswhere.exe -latest -property installationPath