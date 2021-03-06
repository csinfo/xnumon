#!/bin/sh
if [ `id -u` -ne 0 ]; then
	exec /usr/bin/sudo "$0" "$@"
fi
case "$1" in
load|start)
	/bin/launchctl load /Library/LaunchDaemons/ch.roe.xnumon.plist
	;;
unload|stop)
	/bin/launchctl unload /Library/LaunchDaemons/ch.roe.xnumon.plist
	;;
status)
	/bin/launchctl list ch.roe.xnumon
	;;
kextload)
	/sbin/kextload -b ch.roe.kext.xnumon
	;;
kextunload)
	/sbin/kextunload -b ch.roe.kext.xnumon
	;;
kextstat)
	/usr/sbin/kextstat -b ch.roe.kext.xnumon
	;;
reopen)
	kill -HUP `/bin/cat /var/run/xnumon.pid`
	;;
logstats)
	kill -USR1 `/bin/cat /var/run/xnumon.pid`
	;;
uninstall)
	exec /bin/sh '/Library/Application Support/ch.roe.xnumon/uninstall.sh'
	;;
*)
	echo "Usage: $0 load|unload|start|stop|status|kextload|kextunload|kextstat|reopen|logstats|uninstall" >&2
	exit 1
	;;
esac

