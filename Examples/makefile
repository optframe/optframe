all: lts

lts:
	cd LTS && make

legacy:
	cd Legacy && make

fix_examples: svrpdsp

clean: clean_lts
	rm -f app_*
	rm -f *.o

clean_lts:
	cd LTS && ./clean_all.sh
