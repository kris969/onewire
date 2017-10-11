#!/bin/bash

# Apres cette ligne les commandes seront executees systematiquement

# ...

# Apres cette ligne les commandes executees en fonction du parametre passe en ligne de commande

case "$1" in

    start)

        # Commandes executees avec le parametre start (celui lors du boot)
        ps -ef | grep -v grep | grep  test
        /home/chip/projects/w1/test.sh

        ;;

    stop)

        # Commandes executees avec le parametre stop (celui lors de l'arret du systeme)

        ;;

    reload|restart)

        $0 stop

        $0 start

        ;;

    *)

        echo "Usage: $0 start|stop|restart|reload"

        exit 1

esac

exit 0
