rate: rate.c
	@ gcc $< -o $@

.PHONY: clean

clean:
	@ rm rate