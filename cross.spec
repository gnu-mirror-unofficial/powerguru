%rename link_libgcc lgcc
*link_libgcc:
-rpath=%R/usr/local -L%R/lib/arm-linux-gnueabihf -L%R/lib/arm-linux-gnueabihf -L%R/usr/lib/arm-linux-gnueabihf 
