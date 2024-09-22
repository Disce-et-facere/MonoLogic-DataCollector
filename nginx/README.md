# Nginx Reverse Proxy

## Configure self signed SSL certificate

1. Go to the `nginx` folder:

```
cd nginx/ssl
```

1. Create a self-signed cert for `localhost` development environment.

```
openssl req -x509 -sha256 -nodes -newkey rsa:2048 -days 365 -keyout localhost-dev.key -out localhost-dev.crt
# Make sure to add 'localhost' as the common-name (domain) when requested.
```

2. Create a self-signed cert for `skippings.com` production environment.

```
openssl req -x509 -sha256 -nodes -newkey rsa:2048 -days 365 -keyout skippings.com-prod.key -out skippings.com-prod.crt
# Make sure to add 'skippings.com' as the common-name (domain) when requested.
```
