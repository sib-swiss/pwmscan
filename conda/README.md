# Conda build HOWTO
https://docs.conda.io/projects/conda-build/en/latest/user-guide/tutorials/build-pkgs.html


## Build locally chiseq in conda
```bash
conda build meta.yaml -c sibswiss -c bioconda
```


## Install locally built chiseq in local conda
```bash
conda install --use-local pwmscan
```
See https://stackoverflow.com/questions/56895109/how-to-fix-unsatisfiableerror-the-following-specifications-were-found-to-be-in if unsatisfiableerror


## Converting a package for use on all platforms
```bash
conda convert --platform all ~/miniconda3/conda-bld/linux-64/pwmscan-*.tar.bz2 -o outputdir/
#OR TO SKIP win BUILDS
for proc in {osx-64,osx-arm64,linux-32,linux-64,linux-ppc64,linux-ppc64le,linux-s390x,linux-armv6l,linux-armv7l,linux-aarch64};
    do conda convert --platform $proc ~/miniconda3/conda-bld/linux-64/pwmscan-*.tar.bz2 -o outputdir/;
done
```

## Uploading new packages to Anaconda.org
After converting your files for use on other platforms, you may choose to upload your files to Anaconda.org.

1. If you have not done so already, open a free Anaconda.org account and record your new user name and password
2. Run the command `conda install anaconda-client`, and then enter your Anaconda.org username and password
3. Log into your Anaconda.org account with the command:
```bash
anaconda login
```
4. Upload your package to Anaconda.org (with `-u sibswiss` to upload in the sibswiss organization)
```bash
anaconda upload -u sibswiss ~/miniconda3/conda-bld/linux-64/pwmscan-*.tar.bz2 outputdir/*/pwmscan-*.tar.bz2
```
5. Logout
```bash
anaconda logout
```

## Install the chiseq package from anaconda
```bash
conda create --name pwmscan                       # Create an isolated environment
conda activate pwmscan                            # Use this environment
conda install -c sibswiss -c bioconda pwmscan     # Install pwmscan using the sibswiss and secondaryly the bioconda organizations
matrix_prob -h                                    # Test pwmscan tools
conda deactivate                                  # Go out of the pwmscan environment
```
