# Deploy to DigitalOcean

## Deploy using git tags

1. Create a unique tag using format: `git tag <env>-<date>.<sequence>`  
   _Example:_ `git tag production-20240921.1`
2. Push the tag: `git push origin <env>-<date>.<sequence>`  
   _Example:_ `git push origin production-20240921.1`
3. This will trigger the CI/CD workflow and automatically deploy and build.
4. If you need to update the production `.env` file, you need to manually SSH into the droplet and copy the file.

```
cd <local-repo-root> # Where local .env is located
scp .env root@178.128.140.118:/root/
### ENTER SSH PASSWORD ###
```

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

docker-compose down
docker-compose up
```
