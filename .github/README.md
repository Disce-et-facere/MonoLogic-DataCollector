# Deploy to DigitalOcean

## Login via SSH to Droplet

1. In command line: `ssh root@178.128.140.118`
2. Enter the `"secret password"` for the droplet

## Instruction to setup a new droplet

1. Login to Droplet using SSH
2. Install Docker & Docker Compose

```
sudo apt-get update
sudo apt-get install docker.io -y
sudo apt-get install docker-compose -y
```

### Follow this guide to install Gradle on Linux:

https://linuxize.com/post/how-to-install-gradle-on-ubuntu-20-04/

## Build Docker Containers

```
docker-compose up
```

### Brute-Force Deploy Repo

```
cd <repo-root>
scp -r . root@178.128.140.118:/root/
### ENTER PASSWORD ###
ssh root@178.128.140.118
### ENTER PASSWORD ###
```
