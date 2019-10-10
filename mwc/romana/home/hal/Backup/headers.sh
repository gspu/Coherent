# create header archive as hal
# run as rcsadmin

cd /home/hal/include

for f in *.h;do
	echo $f
	(cd /source/usr/include
	rcs -i -t- -ahal,ljg,jrd,chris -L $f)
	ci -d -m"" -whal -nr10 $f /source/usr/include/$f,v
done

for d in *;do
	echo $d
	mkdir /source/usr/include/$d || exit 1
	(cd $d
	for f in *.h;do
		echo $f
		(cd /source/usr/include/$d
		rcs -i -t- -ahal,ljg,jrd,chris -L $f)
		ci -d -m"" -whal -nr10 $f /source/usr/include/$d/$f,v
	done
	)
done
