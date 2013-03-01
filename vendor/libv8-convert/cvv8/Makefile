all:

EXAMPLE_DIR := ../../examples
ifneq (,$(wildcard $(EXAMPLE_DIR)/Makefile))
$(sort all $(MAKECMDGOALS)):
	$(MAKE) -C ../../examples $@
endif

