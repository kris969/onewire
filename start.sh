#!/bin/bash
#  cmd.sh
#  ce script est téléchargé sur le client de pornichet depuis le serveur de rueil
#  puis est activé sur le client de pornichet

TIMESTAMP="$(date +%Y_%m_%d)_$(date +%H_%M)"

TMPDIR="/tmp/"

LOG="/tmp/start.log"
RUNFILE="/home/chip/bin/onewire"

MOIS=$(date +"%m")
SEMAINE=$(date +"%W")
let "SEMAINE_PRECEDENTE = $SEMAINE - 1"
JOUR=$(date +"%w")
HEURE=$(date +"%H")
MINUTE=$(date +"%M")

echo "==========================================================" > $LOG
echo "$TIMESTAMP checking if service onewire is already launched" >> $LOG
PSCMDLINE=`ps -ef | grep "onewire" | grep -v "grep" | wc -l`

echo "nb process test: ($PSCMDLINE)"

# il ne doit y avoir un seul process
if test $PSCMDLINE -gt 1
then
 echo "nb test process > 1"
 echo "$TIMESTAMP from current process ($BASHPID) nb process ($PSCMDLINE) > 1" >> $LOG
	ps -ef | grep "onewire" | grep -v "grep" >> $LOG
P_TO_KILL=`ps -ef | grep "onewire" | head -n 1 | awk '{ print $2 }' `
	echo "kill process: $P_TO_KILL" >> $LOG
        kill $P_TO_KILL
        ps -ef | grep "test" | grep -v "grep" >> $LOG
	echo "abort process $BASHPID"
	echo "--------------------------------"
        exit 0;
else

	ps -ef | grep "onewire" | grep -v "grep"

fi

PSCMDLINE=`ps -ef | grep "onewire" | grep -v "grep" | wc -l`
if test $PSCMDLINE -eq 0
then
	echo "onewire: no service running"
	echo "$TIMESTAMP starting onewire service"
	echo "$TIMESTAMP starting onewire service" >> $LOG
	$RUNFILE& > /dev/null
	ps -ef | grep "onewire" | grep -v "grep" >> $LOG
	ps -ef | grep "onewire" | grep -v "grep"
	exit 0;
fi

PSCMDLINE=`ps -ef | grep "onewire" | grep -v "grep" | wc -l`
if test $PSCMDLINE -eq 1
then
	echo "$TIMESTAMP onewire service is running" >> $LOG
	ps -ef | grep "onewire" | grep -v "grep" >> $LOG

fi

exit 0;


