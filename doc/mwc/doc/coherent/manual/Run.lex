# Process the Lexicon entries for the COHERENT manual
# by fwb, 6/91
#
# copyright (c) 1991-1994 by Mark Williams Company, Northbrook Ill.

OUTPUT=/tmp/doc
INPUT=/v/doc/coherent

>pgn.r

if [ $# -ne 1 ]; then
	echo "Usage: Run letter"
	exit
fi

# extract the page number from the previous section
case $1 in
	_)	tail -1 $OUTPUT/lexicon.x >pgn.r	;;
	a)	tail -1 $OUTPUT/_.x >pgn.r		;;
	b)	tail -1 $OUTPUT/a.x >pgn.r		;;
	c)	tail -1 $OUTPUT/b.x >pgn.r		;;
	d)	tail -1 $OUTPUT/c.x >pgn.r		;;
	e)	tail -1 $OUTPUT/d.x >pgn.r		;;
	f)	tail -1 $OUTPUT/e.x >pgn.r		;;
	g)	tail -1 $OUTPUT/f.x >pgn.r		;;
	h)	tail -1 $OUTPUT/g.x >pgn.r		;;
	i)	tail -1 $OUTPUT/h.x >pgn.r		;;
	j)	tail -1 $OUTPUT/i.x >pgn.r		;;
	k)	tail -1 $OUTPUT/j.x >pgn.r		;;
	l)	tail -1 $OUTPUT/k.x >pgn.r		;;
	m)	tail -1 $OUTPUT/l.x >pgn.r		;;
	n)	tail -1 $OUTPUT/m.x >pgn.r		;;
	o)	tail -1 $OUTPUT/n.x >pgn.r		;;
	p)	tail -1 $OUTPUT/o.x >pgn.r		;;
	q)	tail -1 $OUTPUT/p.x >pgn.r		;;
	r)	tail -1 $OUTPUT/q.x >pgn.r		;;
	s)	tail -1 $OUTPUT/r.x >pgn.r		;;
	t)	tail -1 $OUTPUT/s.x >pgn.r		;;
	u)	tail -1 $OUTPUT/t.x >pgn.r		;;
	v)	tail -1 $OUTPUT/u.x >pgn.r		;;
	w)	tail -1 $OUTPUT/v.x >pgn.r		;;
	x)	tail -1 $OUTPUT/w.x >pgn.r		;;
	y)	tail -1 $OUTPUT/x.x >pgn.r		;;
	z)	tail -1 $OUTPUT/y.x >pgn.r		;;
esac

# troff the appropriate file
if [ ! -s FILES.$1 ]; then
	echo "Creating FILES.$1"
	ls $INPUT/lx/$1* >FILES.$1 2>>coh.x
fi

troff -p -Z3 -rX0 -mcohlex fonts.r pgn.r top.r \
	$INPUT/alphabet/$1.r `cat FILES.$1` \
	2>$OUTPUT/$1.x > $OUTPUT/$1.p
