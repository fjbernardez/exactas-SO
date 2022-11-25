#/bin/bash

STRATS=('secuencial' 'rr' 'shortest' 'custom')

RED='\033[0;31m'
GREEN='\033[0;32m'
NOCOLOR='\033[0m'

for CONFIGFILE in ./config/*.csv
do
	for STRAT in "${STRATS[@]}"
	do
		echo -n Corriendo config "${CONFIGFILE} strat ${STRAT}"...
		./test_tp_sistemas -strat ${STRAT} -config ${CONFIGFILE} > /dev/null
		if [ $? -eq 0 ]
		then
			printf "${GREEN}JOYA${NOCOLOR}\n"
		else
			printf "${RED}FALLO!${NOCOLOR}\n"
		fi
	done
done
