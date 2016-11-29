#
# Regular cron jobs for the siengine package
#
0 4	* * *	root	[ -x /usr/bin/siengine_maintenance ] && /usr/bin/siengine_maintenance
