all:
ifeq ($(PLATFORM), LINUX)
	@make --no-print-directory -f linux.mk
endif
ifeq ($(PLATFORM), BONSOS)
	@make --no-print-directory -f bonsos.mk
else
	@echo "Specify a platform (make PLATFORM=<platform>)"
endif
