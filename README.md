# Kaiyuan's Web Server, or Kernel-based Web Server

Kws is a personal experimental project to build high performance HTTP server. The initial idea of kws origined from kvm (Kernel-based Virtual Machine). Integrating web server into kernel tries to solve performance issue, but also raises some issues.

## Build
Download source, and cd to kws directory.
Make kws:

	~/kws $ make

Building will generate a kernel module file `kws.ko`.

If the path of kernel header file is different, you have to manually modifiy path in Makefile. Adding auto configuration script is a "ToDo" task.

## Install & Uninstall
Using `insmod` tool to insert `kws.ko` into kernel:

	~/kws $ sudo insmod kws.ko

Uninstall module:

	~/kws $ sudo rmmod kws

## Module Parameters
See `init.c` to check out all module parameters.
Following code snippet describes the name, default value, and desc of module parameters:

	static int port = 8080;
	module_param(port, int, 0644);
	MODULE_PARM_DESC(port, "The port kws will listen");

	static int workers = 0;
	module_param(workers, int, 0644);
	MODULE_PARM_DESC(workers, "");

	static int status = RUNNING;
	module_param(status, int, 0644);
	MODULE_PARM_DESC(status, "Running - 0\nRestart - 1\nStop - 2");

	static char *wwwroot = DEFAULT_WWWROOT;
	module_param(wwwroot, charp, 0644);
	MODULE_PARM_DESC(wwwroot, "WWWRoot");



## Implementation & ToDo
* Request queue
* (ToDo) Slab cache
* HTTP header parse
* Kthread pool
* (ToDo) Dynamic framework
* (ToDo) Reverse proxy
* (ToDo) Request cache
