```bash
export PWMSCAN_VERSION=1.1.10
```

# Run bash in the Docker image
```bash
docker run --rm -it pwmscan:$PWMSCAN_VERSION bash
```
### Mounting/binding a local repository (,readonly can be added to force readonly mounting)
```bash
    --mount type=bind,source=/software,target=/software
```
- PWMSCAN_VERSION is the container version
- --name assignes a name to the running container
- --rm automatically removes the container when it exits
