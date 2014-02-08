#!/bin/sh
/opt/tiva/ccs/ccsv5/utils/tiobj2bin/tiobj2bin Debug/eeprom_burner.out Debug/eeprom_burner.bin /opt/tiva/ccs/ccsv5/tools/compiler/arm_5.1.1/bin/armofd /opt/tiva/ccs/ccsv5/tools/compiler/arm_5.1.1/bin/armhex /opt/tiva/ccs/ccsv5/utils/tiobj2bin/mkhex4bin
lm4flash Debug/eeprom_burner.bin
