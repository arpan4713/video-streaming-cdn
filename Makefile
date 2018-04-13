all:
	+$(MAKE) -C nameserver
	+$(MAKE) -C miProxy

clean:
	rm nameserver/nameserver miProxy/miProxy nameserver/*.o miProxy/*.o
