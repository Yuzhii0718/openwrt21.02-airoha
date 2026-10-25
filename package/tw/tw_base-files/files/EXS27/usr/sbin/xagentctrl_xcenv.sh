#!/bin/sh

raeStage=`uci get rae.@rae[0].RaeStage`
if [ "$raeStage" == "prod" ]; then
	xstage=$raeStage
else
	xstage="staging"	
fi	
echo xstage:$xstage > /dev/console
d2 -c XagentCtrl[0].xcenv $xstage
