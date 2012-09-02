echo script to profile Oneline using the oprof system profiler
opcontrol --init
opcontrol --no-vmlinux
opcontrol --start
opcontrol --separate=library
debug/oneline
opcontrol --shutdown
opgprof -p debug/oneline
