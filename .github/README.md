# Deploy Repo to DigitalOcean

### Brute-Force Deploy Repo

```
cd <repo-root>
scp -r . root@178.128.140.118:/root/
### ENTER PASSWORD ###
ssh root@178.128.140.118
### ENTER PASSWORD ###
docker-compose up -d
```

# Login to Droplet

1. In command line: `ssh root@178.128.140.118`
2. Enter the secret password for the droplet

## Instruction to setup a new droplet

1. Login to Droplet using ssh
2. Install Docker & Docker Compose

```
sudo apt-get update
sudo apt-get install docker.io -y
sudo apt-get install docker-compose -y
```

### TODO: Add DevOps instructions... // ST 2024-09-20
