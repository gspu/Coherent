llpp -- Device Driver

Driver for parallel ports
/ddeevv/llpptt_N

The device driver llpp drives the parallel ports.  It has major number 3.

This driver follows  the IBM PC standard in that  it can only send data out
the port -- it cannot receive data from the port.

The  following script  lets  you install  or  de-install the  parallel-port
driver:  To  install  or de-install  a  parallel  printer,  log  in as  the
superuser rroooott; then execute the following script:

    cd /etc/conf
    lp/mkdev
    /conf/mlpconfig
    bin/idmkcoh -o /_k_e_r_n_e_l__n_a_m_e
_k_e_r_n_e_l__n_a_m_e should name the new kernel to build.  Then reboot to invoke the
newly built _k_e_r_n_e_l__n_a_m_e.

_S_e_e _A_l_s_o
ddeevviiccee ddrriivveerrss, pprriinntteerr
