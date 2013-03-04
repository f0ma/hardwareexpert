import os

sudo_command = 'gksu'

ftdidevs = os.popen("lsusb | grep 'Future Technology Devices International'").read().split('\n')

for d in ftdidevs:
	ftdidev = d.split(' ')
	if len(ftdidev) < 3 :
		quit()

	bus = ftdidev[1][:3]
	dev = ftdidev[3][:3]

	print "Grant rights to",bus,dev

	acch = os.popen("ls -la /dev/bus/usb/"+bus+"/"+dev).read().split(' ')[0][1:]

	if acch != "rwxrwxrwx" :
		print "Grant access to",bus,dev
		os.system(sudo_command+" rmmod ftdi_sio && "+sudo_command+" chmod a+rwx /dev/bus/usb/"+bus+"/"+dev)
	else :
		print "Already"

if len(d) ==0:
	print "No FTDI device found!"
