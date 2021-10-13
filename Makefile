all:
ifeq ($(PLATFORM), LINUX)
	@make --no-print-directory -f linux.mk
else ifeq ($(PLATFORM), BONSOS)
	@make --no-print-directory -f bonsos.mk
else ifeq ($(PLATFORM), WINDOWS)
	@make --no-print-directory -f windows.mk
else
	@echo "Specify a platform (make PLATFORM=<platform>)"
endif
