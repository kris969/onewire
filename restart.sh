#!/bin/bash
#  restart
#  this script is looking for an already service running
#  if so, it kills it and launch it again using the script "start"

TIMESTAMP="$(date +%Y_%m_%d)_$(date +%H_%M)"

TMPDIR='/tmp/'
LOG=$TMPDIR'restart.log'
START="/bin/bash "`pwd`"/start.sh"


MOIS=$(date +"%m")
SEMAINE=$(date +"%W")
let "SEMAINE_PRECEDENTE = $SEMAINE - 1"
JOUR=$(date +"%w")
HEURE=$(date +"%H")
MINUTE=$(date +"%M")


echo "========================================================="
echo "=  STARTING RESTART SCRIPT                              ="
echo "========================================================="


echo "==========================================================" > $LOG
echo "$TIMESTAMP checking if service onewire is already launched" >> $LOG
PSCMDLINE=`ps -ef | grep onewire | grep -v grep | wc -l`

echo "nb process test: ($PSCMDLINE)"

# il ne doit y avoir un seul process
if test $PSCMDLINE -ge 1
then
 echo "$TIMESTAMP a process onewire is running"
 echo "$TIMESTAMP from current process ($BASHPID) nb process ($PSCMDLINE) > 0" >> $LOG
	ps -ef | grep onewire | grep -v grep >> $LOG
	P_TO_KILL=`ps -ef | grep onewire | head -n 1 | awk '{ print $2 }' `
	echo "$TIMESTAMP kill process: $P_TO_KILL" >> $LOG
        kill $P_TO_KILL
fi

$START

exit 0;


