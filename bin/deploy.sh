#!/bin/sh
orig_dir=`dirname \`readlink -f $0\``
target_list=`cat $orig_dir/deploy.lst | grep -v '^#'`
prog_dir=mccsystem1
for target in $target_list
do
	echo deploy to $target
	ssh $target "mkdir -p /mccsystem1 && mkdir -p /mccsystem1/lib && rm -rf /mccsystem1/mcc"
	scp mcc root@$target:/mccsystem1/
	ssh $target "killall mcc && killall mcc"
#	scp lib/*.so root@$target:/mccsystem1/lib 
done
