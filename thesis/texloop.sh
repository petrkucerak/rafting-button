#!/bin/bash

if [ $# -lt 2 ] ; then
  echo "usage:   texloop tex-command main-file [A]"
  echo "   The main-file is file name without suffix."
  echo "   Type the letter A if you wish to run tex-command Allways,"
  echo "   no only if the main-file.tex is newer than main-file.log."
  exit 1
fi 

COMMAND=$1
MAINFILE=$2
MAINFILEBASE=`echo $2 | cut -d. -f1`
ALLWAYS=$3

echo ">>" $1 $2
$1 $2

prefix=`echo $1 | cut -b 1-3`

if [ "$prefix" = "pdf" ] ; then
   PDF="true"
   VIEWFILE="$MAINFILEBASE.pdf"
else
   PDF="false"
   VIEWFILE="$MAINFILEBASE.dvi"
fi

if [ -f $VIEWFILE ] ; then
   VIEWER="OK"
   if $PDF ; then
      xpdf -remote texloop.$$ $MAINFILEBASE.pdf &  
   else
      xdvi $MAINFILEBASE &
   fi
   VIEWPID=$!
else
   sleep 30d &
   VIEWPID=$!
   echo "texloop: The $VIEWFILE may be created later..."
fi

texrun() {
   trap "" USR1
   if [ $MAINFILEBASE.tex -nt $MAINFILEBASE.log -o $MAINFILE -nt $MAINFILEBASE.log -o "$ALLWAYS" = "A" ] ; then
      echo ">>" $COMMAND $MAINFILE
      echo x | $COMMAND $MAINFILE
      if [ $? -eq 0 ] ; then
	 if [ "$VIEWER" = "OK" ] ; then
            if $PDF ; then
               xpdf -remote texloop.$$ -reload
            else
               kill -USR1 $VIEWPID
            fi
         else
            if [ -f $VIEWFILE ] ; then
               VIEWER="OK"
               kill $VIEWPID
               if $PDF ; then
                  xpdf -remote texloop.$$ $MAINFILEBASE.pdf &
               else
            	  xdvi $MAINFILEBASE &
               fi
               VIEWPID=$!
            fi
         fi
      fi
   fi
   trap texrun USR1
}
  
echo $$ > ~/.tlpid
trap texrun USR1
trap "kill $VIEWPID" INT

while ps -p $VIEWPID > /dev/null 
do
   wait
done
