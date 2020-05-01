ifneq (yes,$(VERBOSE))
    Q=@
else
    Q=
endif

.PHONY: bench primes viewer
all: bench primes viewer
	$(Q)for i in $^; do make -s -C $$i all; done

.PHONY: bench primes viewer
clean: bench primes viewer
	$(Q)for i in $^; do make -s -C $$i clean; done

DEPENDS := $(foreach dir,$(LIBDIRS), $(wildcard $(dir)/objs/*.d) $(wildcard $(dir)/objs/iio/*.d))

.PHONY: printvars tests
printvars:
	@$(foreach V,$(.VARIABLES), $(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V))))


