# ***Setup RaspberryPi to be target for remote raspberry pi testing (and debugging)***

* install Base rasberrian default OS

* sudo raspi-config
  interface options/select ssh and enable
	
* sudo adduser lewis

* sudo usermod -a -G sudo lewis

* copy my dev id_rsa.pub to raspberrypi:~/.ssh/authorized_keys (and set chmod permissions 700/600)

* sudo apt-get install apt-file

* sudo vi /etc/apt/sources.list
       add lines()
       #added for latest libc
       deb http://deb.debian.org/debian testing main

* sudo apt update

* sudo apt upgrade

* sudo apt-get install gdb valgrind libubsan1 libasan5
  
  NB:  may not longer need libubsan1 libasan5 sine these sb staticly linked;
  valgrind just needed to run (valgrind) regtests, and gdb not really needed at all, but if you are going to test
  on a raspberrypi, you probably will end up wanting to run a debugger there


