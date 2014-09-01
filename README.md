mysql_udf_json_rate_converter
=============================

MySQL user defined function that calculates hi and low value in a numeric JSON array


# Compiling:

[1] COMPILE main.c to udf_json_rate_converter.so

```
gcc -fPIC -Wall   -c -g -I/usr/local/lib -I/usr/include/mysql -I/usr/local/include -fPIC -Wall -MMD -MP -MF build/Debug/GNU-Linux-x86/main.o.d -o build/Debug/GNU-Linux-x86/main.o main.c
gcc -fPIC -Wall    -o dist/Debug/GNU-Linux-x86/udf_json_rate_converter.so build/Debug/GNU-Linux-x86/main.o  -shared -ljansson
```

### Compilation Notes:

- INCLUDE DIRECTORIES
```
mysql.h jansson.h
```

- LINKER OPTIONS
```
 -shared -ljansson
```

- WHAT THE FLAGS MEAN

```
-fPIC
```
If supported for the target machine, emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of the global offset table. This option makes a difference on the m68k, PowerPC and SPARC.
Position-independent code requires special support, and therefore works only on certain machines.

When this flag is set, the macros __pic__ and __PIC__ are defined to 2. 

```
-Wall
```
This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros. This also enables some language-specific warnings described in C++ Dialect Options and Objective-C and Objective-C++ Dialect Options.




# Installation:

## [n] MOVE .so file to mysql plugins directory:

//TO GET PATH TO PLUGINS DIR
```
mysql> SHOW VARIABLES LIKE 'plugin_dir';
```

//TO MOVE .so TO PLUGINS DIR
```
mv udf_json_rate_converter.so /usr/lib/mysql/plugin/
```

## [n] CHECK FUNCTIONS AND DROP EXISTING CONFLICTS
```
SELECT * FROM mysql.func;
DROP function ...
```

## [n] CREATE THE FUNCTIONS
```
create function udf_json_low_rate returns real soname 'udf_json_rate_converter.so';
create function udf_json_high_rate returns real soname 'udf_json_rate_converter.so';
create function udf_is_json returns integer soname 'udf_json_rate_converter.so';
```

## [n] RESTART MYSQL
```
service mysql restart
```

## [n] EXAMPLE TRIGGER 

```
DELIMITER //
CREATE TRIGGER update_rates BEFORE UPDATE ON sometable      
FOR EACH ROW      
BEGIN      
DECLARE j INT(2);
SET j = udf_is_json(NEW.rates);
IF j = 1 THEN SET NEW.high_rate = udf_json_high_rate(NEW.rates), NEW.low_rate = udf_json_low_rate(NEW.rates);    	
ELSE SET NEW.rates = OLD.rates, NEW.high_rate = OLD.high_rate, NEW.low_rate = OLD.low_rate;       
END IF;      
END
//
```
