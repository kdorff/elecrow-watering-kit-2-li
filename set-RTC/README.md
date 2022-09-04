# set-RTC

This is a small bit of code to set the date/time on the Real-Time-Clock (RTC) if/when the battery is replaced.  To use it, just upload it to the device and it will set the time and then display it on the screen.

NOTE: The time is set based on the time the program was compiled, so after this runs once on the device be sure to immediately upload something else.  If you don't, the RTC will get reset each time the device resets and the time will be behind.
