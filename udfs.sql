DROP FUNCTION IF EXISTS udf_json_low_rate;
CREATE function udf_json_low_rate returns real soname 'udf_json_rate_converter.so';
DROP FUNCTION IF EXISTS udf_json_high_rate;
CREATE function udf_json_high_rate returns real soname 'udf_json_rate_converter.so';
DROP FUNCTION IF EXISTS udf_is_json;
CREATE function udf_is_json returns integer soname 'udf_json_rate_converter.so';