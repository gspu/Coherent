.TH lp "" "" "Device Driver"
.PC "Driver for parallel ports"
\fB/dev/lpt\fIN\fR
.PP
.II /dev/lp
.II /dev/rlp
.II "parallel port^driver"
.II "port^parallel, driver"
.II "driver^parallel port"
.II "device driver^parallel port"
The device driver
.B lp
drives the parallel ports.
It has major number 3.
.PP
This driver follows the IBM PC standard in that it can only send data
out the port \(em it cannot receive data from the port.
.PP
The following script lets you install or de-install the parallel-port
driver:
To install or de-install a parallel printer, log in as the superuser
.BR root ;
then execute the following script:
.DM
	cd /etc/conf
	lp/mkdev
	/conf/mlpconfig
	bin/idmkcoh -o /\fIkernel_name\fP
.DE
.PP
.I kernel_name
should name the new kernel to build.
Then reboot to invoke the newly built
.IR kernel_name .
.SH "See Also"
.Xr "device drivers," device_dr
.Xr "printer" printer
