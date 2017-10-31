from redis import StrictRedis


redis_host = "127.0.0.1"
redis_storage_port = 6379
redis_volatile_port = 6378

RDYNA = StrictRedis(host=redis_host, port=redis_volatile_port, decode_responses=True)
RSTOR = StrictRedis(host=redis_host, port=redis_storage_port, decode_responses=True)
