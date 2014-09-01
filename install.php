<?php

$mysql_root_password = '';


/**
 * COPY .so TO TARGET DIRECTORY
 * 
 */
exec("cp udf_json_rate_converter.so /usr/lib/mysql/plugin/");



/**
 * INSTALL LIB JANNSSON IF NOT INSTALLED
 * https://github.com/akheron/jansson.git
 */

//IF JANSSON NOT INSTALLED
$jansson_dir = __DIR__."/jansson";

/*
mkdir($jansson_dir);

$zip = file_get_contents("https://github.com/akheron/jansson/archive/master.zip");
file_put_contents($jansson_dir."/master.zip", $zip);

exec("unzip ".$jansson_dir."/master.zip -d ".$jansson_dir);
*/


//AUTOMATICALLY RESPOND YES TO PROMPTS
$responses = <<<R
Y
Y
Y
R;
exec('cd '.$jansson_dir.' && echo "'.$responses.'" | ./release.sh');
exec('cd '.$jansson_dir.' && echo "Y" | ./configure && make && make install');



/**
 * INSTALL UDFS 
 * 
 */
exec('mysql -u root -p '.$mysql_root_password.' && source udfs.sql');