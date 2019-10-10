FORMAT=fva
id=Coh_42X
ndisks=6

# Kluge edit for number of diskettes in install.

/bin/echo "Fix /etc/brc.install:"
do_edit.sh "__PRODUCT=$id" "__BOOT_DEV=/dev/${FORMAT}0" "__NDISKS=$ndisks" \
  brc.install /etc/brc.install

# /bin/echo "/etc/install -b $id /dev/${FORMAT}0 **$ndisks** ..... [hit enter]"
# read xxxx
# $EDITOR $MNT/etc/brc.install

# Kluge edit for number of diskettes in update.

/bin/echo "Fix /etc/brc.update:"
do_edit.sh "__PRODUCT=$id" "__BOOT_DEV=/dev/${FORMAT}0" "__NDISKS=$ndisks" \
  brc.update /etc/brc.update

# /bin/echo "/etc/install -u $id /dev/${FORMAT}0 **$ndisks** ..... [hit enter]"
# read xxxx
# $EDITOR $MNT/etc/brc.update

# Kluge edit for mount devices.

case ${FORMAT} in
fva)
	BDRIVE=/dev/fha1
	FHAV=#
	FVAV=
	;;
fha)
	BDRIVE=/dev/fva1
	FHAV=
	FVAV=#
	;;
*)
	echo "$0: Unrecognized name ${FORMAT} for boot floppy!" >&2
	exit 1
	;;
esac

/bin/echo -n "Edit /bin/mount and set alias f0 to be /dev/${FORMAT}0"
do_edit.sh "__BOOT_DEV=/dev/${FORMAT}0" "__SECOND_FLOPPY=$BDRIVE" \
  mount /bin/mount

# read x
# $EDITOR $MNT/bin/mount

# Kluge edit for dos diskette drive names.

/bin/echo -n "Edit /etc/default/msdos and uncomment appropriate A:/B"
do_edit.sh "__FHA_VERSION=$FHAV" "__FVA_VERSION=$FVAV" \
  msdos /etc/default/msdos

# read x
# $EDITOR $MNT/etc/default/msdos
