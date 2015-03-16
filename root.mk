#
#
#

# apps for stm32f401re-nucleo

include $(PRJ_DIR)/boards/stm32f4-nucleo/apps/leds/build.mk
include $(PRJ_DIR)/boards/stm32f4-nucleo/apps/uart/build.mk
include $(PRJ_DIR)/boards/stm32f4-nucleo/apps/rf24cli/build.mk

# apps for stm32f4-discovery

include $(PRJ_DIR)/boards/stm32f4-discovery/apps/leds/build.mk
include $(PRJ_DIR)/boards/stm32f4-discovery/apps/mems/build.mk
include $(PRJ_DIR)/boards/stm32f4-discovery/apps/cdc/build.mk
include $(PRJ_DIR)/boards/stm32f4-discovery/apps/rf24srv/build.mk
