# Nginx Reverse Proxy

## Configure self signed SSL certificate

```
cd nginx/ssl
sudo openssl req -x509 -sha256 -nodes -newkey rsa:2048 -days 365 -keyout localhost.key -out localhost.crt
```

In case you experience reading errors, add permission for key-file:

```
sudo chmod 644 localhost.key
```
